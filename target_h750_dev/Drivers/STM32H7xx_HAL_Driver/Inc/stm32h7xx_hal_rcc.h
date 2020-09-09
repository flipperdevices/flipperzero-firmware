/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rcc.h
  * @author  MCD Application Team
  * @brief   Header file of RCC HAL module.
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
#ifndef STM32H7xx_HAL_RCC_H
#define STM32H7xx_HAL_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup RCC
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup RCC_Exported_Types RCC Exported Types
  * @{
  */

/**
  * @brief  RCC PLL configuration structure definition
  */
typedef struct
{
  uint32_t PLLState;   /*!< The new state of the PLL.
                            This parameter can be a value of @ref RCC_PLL_Config                      */

  uint32_t PLLSource;  /*!< RCC_PLLSource: PLL entry clock source.
                            This parameter must be a value of @ref RCC_PLL_Clock_Source               */

  uint32_t PLLM;       /*!< PLLM: Division factor for PLL VCO input clock.
                            This parameter must be a number between Min_Data = 1 and Max_Data = 63    */

  uint32_t PLLN;       /*!< PLLN: Multiplication factor for PLL VCO output clock.
                            This parameter must be a number between Min_Data = 4 and Max_Data = 512 
                            or between Min_Data = 8 and Max_Data = 420(*)
                            (*) : For stm32h7a3xx and stm32h7b3xx family lines.                       */                        

  uint32_t PLLP;       /*!< PLLP: Division factor for system clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 128
                          odd division factors are not allowed                                        */

  uint32_t PLLQ;       /*!< PLLQ: Division factor for peripheral clocks.
                            This parameter must be a number between Min_Data = 1 and Max_Data = 128   */

  uint32_t PLLR;       /*!< PLLR: Division factor for peripheral clocks.
                            This parameter must be a number between Min_Data = 1 and Max_Data = 128   */
  uint32_t PLLRGE;     /*!<PLLRGE: PLL1 clock Input range
                         This parameter must be a value of @ref RCC_PLL1_VCI_Range                    */
  uint32_t PLLVCOSEL;  /*!<PLLVCOSEL: PLL1 clock Output range
                         This parameter must be a value of @ref RCC_PLL1_VCO_Range                    */

  uint32_t PLLFRACN;   /*!<PLLFRACN: Specifies Fractional Part Of The Multiplication Factor for
                        PLL1 VCO It should be a value between 0 and 8191                              */

}RCC_PLLInitTypeDef;

/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, CSI, LSE and LSI) configuration structure definition
  */
typedef struct
{
  uint32_t OscillatorType;       /*!< The oscillators to be configured.
                                      This parameter can be a value of @ref RCC_Oscillator_Type                   */

  uint32_t HSEState;             /*!< The new state of the HSE.
                                      This parameter can be a value of @ref RCC_HSE_Config                        */

  uint32_t LSEState;             /*!< The new state of the LSE.
                                      This parameter can be a value of @ref RCC_LSE_Config                        */

  uint32_t HSIState;             /*!< The new state of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Config                        */

  uint32_t HSICalibrationValue;  /*!< The calibration trimming value.
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x3F for STM32H7 rev.Y
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x7F for STM32H7 rev.B and above */

  uint32_t LSIState;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

 uint32_t HSI48State;            /*!< The new state of the HSI48.
                                      This parameter can be a value of @ref RCC_HSI48_Config                      */

  uint32_t CSIState;             /*!< The new state of the CSI.
                                      This parameter can be a value of @ref RCC_CSI_Config */

  uint32_t CSICalibrationValue;  /*!< The calibration trimming value.
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F for STM32H7 rev.Y
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x3F for STM32H7 rev.B and above */

  RCC_PLLInitTypeDef PLL;        /*!< PLL structure parameters                                                    */

}RCC_OscInitTypeDef;

/**
  * @brief  RCC System, AHB and APB busses clock configuration structure definition
  */
typedef struct
{
  uint32_t ClockType;             /*!< The clock to be configured.
                                       This parameter can be a value of @ref RCC_System_Clock_Type                          */

  uint32_t SYSCLKSource;          /*!< The clock source (SYSCLKS) used as system clock.
                                       This parameter can be a value of @ref RCC_System_Clock_Source                        */

  uint32_t SYSCLKDivider;         /*!< The system clock  divider. This parameter can be
                                       a value of @ref RCC_SYS_Clock_Source                                                 */

  uint32_t AHBCLKDivider;         /*!< The AHB clock (HCLK) divider. This clock is derived from the system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_HCLK_Clock_Source                          */

  uint32_t APB3CLKDivider;        /*!< The APB3 clock (D1PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB3_Clock_Source                        */

  uint32_t APB1CLKDivider;        /*!< The APB1 clock (PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_Clock_Source                        */
  uint32_t APB2CLKDivider;        /*!< The APB2 clock (PCLK2) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB2_Clock_Source                        */
  uint32_t APB4CLKDivider;      /*!< The APB4 clock (D3PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB4_Clock_Source                        */
}RCC_ClkInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup RCC_Exported_Constants RCC Exported Constants
  * @{
  */

/** @defgroup RCC_Oscillator_Type  RCC Oscillator Type
  * @{
  */
#define RCC_OSCILLATORTYPE_NONE        (0x00000000U)
#define RCC_OSCILLATORTYPE_HSE         (0x00000001U)
#define RCC_OSCILLATORTYPE_HSI         (0x00000002U)
#define RCC_OSCILLATORTYPE_LSE         (0x00000004U)
#define RCC_OSCILLATORTYPE_LSI         (0x00000008U)
#define RCC_OSCILLATORTYPE_CSI         (0x00000010U)
#define RCC_OSCILLATORTYPE_HSI48       (0x00000020U)

/**
  * @}
  */

/** @defgroup RCC_HSE_Config  RCC HSE Config
  * @{
  */
#define RCC_HSE_OFF                    (0x00000000U)
#define RCC_HSE_ON                     RCC_CR_HSEON
#define RCC_HSE_BYPASS                 ((uint32_t)(RCC_CR_HSEBYP | RCC_CR_HSEON))
#if defined(RCC_CR_HSEEXT)
#define RCC_HSE_BYPASS_DIGITAL         ((uint32_t)(RCC_CR_HSEEXT | RCC_CR_HSEBYP | RCC_CR_HSEON))
#endif /* RCC_CR_HSEEXT */

/**
  * @}
  */

/** @defgroup RCC_LSE_Config  RCC LSE Config
  * @{
  */
#define RCC_LSE_OFF                    (0x00000000U)
#define RCC_LSE_ON                     RCC_BDCR_LSEON
#define RCC_LSE_BYPASS                 ((uint32_t)(RCC_BDCR_LSEBYP | RCC_BDCR_LSEON))
#if defined(RCC_BDCR_LSEEXT)
#define RCC_LSE_BYPASS_DIGITAL         ((uint32_t)(RCC_BDCR_LSEEXT | RCC_BDCR_LSEBYP | RCC_BDCR_LSEON))
#endif /* RCC_BDCR_LSEEXT */

/**
  * @}
  */

/** @defgroup RCC_HSI_Config  RCC HSI Config
  * @{
  */
#define RCC_HSI_OFF                      (0x00000000U)           /*!< HSI clock deactivation */
#define RCC_HSI_ON                       RCC_CR_HSION                     /*!< HSI clock activation */

#define RCC_HSI_DIV1                     (RCC_CR_HSIDIV_1 | RCC_CR_HSION) /*!< HSI_DIV1 clock activation */
#define RCC_HSI_DIV2                     (RCC_CR_HSIDIV_2 | RCC_CR_HSION) /*!< HSI_DIV2 clock activation */
#define RCC_HSI_DIV4                     (RCC_CR_HSIDIV_4 | RCC_CR_HSION) /*!< HSI_DIV4 clock activation */
#define RCC_HSI_DIV8                     (RCC_CR_HSIDIV | RCC_CR_HSION)   /*!< HSI_DIV8 clock activation */


#if defined(RCC_HSICFGR_HSITRIM_6)
#define RCC_HSICALIBRATION_DEFAULT     (0x40U)         /* Default HSI calibration trimming value, for STM32H7 rev.V and above  */
#else
#define RCC_HSICALIBRATION_DEFAULT     (0x20U)         /* Default HSI calibration trimming value, for STM32H7 rev.Y */
#endif
/**
  * @}
  */

/** @defgroup RCC_HSI48_Config  RCC HSI48 Config
  * @{
  */
#define RCC_HSI48_OFF                      ((uint8_t)0x00)
#define RCC_HSI48_ON                       ((uint8_t)0x01)

/**
  * @}
  */

/** @defgroup RCC_LSI_Config  RCC LSI Config
  * @{
  */
#define RCC_LSI_OFF                    (0x00000000U)
#define RCC_LSI_ON                     RCC_CSR_LSION

/**
  * @}
  */

/** @defgroup RCC_CSI_Config  RCC CSI Config
  * @{
  */
#define RCC_CSI_OFF                    (0x00000000U)
#define RCC_CSI_ON                     RCC_CR_CSION

#if defined(RCC_CSICFGR_CSITRIM_5)
#define RCC_CSICALIBRATION_DEFAULT     (0x20U)         /* Default CSI calibration trimming value */
#else
#define RCC_CSICALIBRATION_DEFAULT     (0x10U)         /* Default CSI calibration trimming value */
#endif /* RCC_CSICFGR_CSITRIM_5 */
/**
  * @}
  */

/** @defgroup RCC_PLL_Config  RCC PLL Config
  * @{
  */
#define RCC_PLL_NONE                   (0x00000000U)
#define RCC_PLL_OFF                    (0x00000001U)
#define RCC_PLL_ON                     (0x00000002U)

/**
  * @}
  */


/** @defgroup RCC_PLL_Clock_Source  RCC PLL Clock Source
  * @{
  */
#define RCC_PLLSOURCE_HSI              (0x00000000U)
#define RCC_PLLSOURCE_CSI              (0x00000001U)
#define RCC_PLLSOURCE_HSE              (0x00000002U)
#define RCC_PLLSOURCE_NONE             (0x00000003U)
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Output  RCC PLL Clock Output
  * @{
  */
#define RCC_PLL1_DIVP                RCC_PLLCFGR_DIVP1EN
#define RCC_PLL1_DIVQ                RCC_PLLCFGR_DIVQ1EN
#define RCC_PLL1_DIVR                RCC_PLLCFGR_DIVR1EN

/**
  * @}
  */



/** @defgroup RCC_PLL1_VCI_Range  RCC PLL1 VCI Range
  * @{
  */
#define RCC_PLL1VCIRANGE_0                RCC_PLLCFGR_PLL1RGE_0       /*!< Clock range frequency between 1 and 2 MHz  */
#define RCC_PLL1VCIRANGE_1                RCC_PLLCFGR_PLL1RGE_1       /*!< Clock range frequency between 2 and 4 MHz  */
#define RCC_PLL1VCIRANGE_2                RCC_PLLCFGR_PLL1RGE_2       /*!< Clock range frequency between 4 and 8 MHz  */
#define RCC_PLL1VCIRANGE_3                RCC_PLLCFGR_PLL1RGE_3       /*!< Clock range frequency between 8 and 16 MHz */


/**
  * @}
  */


/** @defgroup RCC_PLL1_VCO_Range  RCC PLL1 VCO Range
  * @{
  */
#define RCC_PLL1VCOWIDE                 (0x00000000U)
#define RCC_PLL1VCOMEDIUM               RCC_PLLCFGR_PLL1VCOSEL

/**
  * @}
  */


/** @defgroup RCC_System_Clock_Type  RCC System Clock Type
  * @{
  */
#define RCC_CLOCKTYPE_SYSCLK           (0x00000001U)
#define RCC_CLOCKTYPE_HCLK             (0x00000002U)
#define RCC_CLOCKTYPE_D1PCLK1          (0x00000004U)
#define RCC_CLOCKTYPE_PCLK1            (0x00000008U)
#define RCC_CLOCKTYPE_PCLK2            (0x00000010U)
#define RCC_CLOCKTYPE_D3PCLK1          (0x00000020U)

/**
  * @}
  */

/** @defgroup RCC_System_Clock_Source  RCC System Clock Source
  * @{
  */
#define RCC_SYSCLKSOURCE_CSI           RCC_CFGR_SW_CSI
#define RCC_SYSCLKSOURCE_HSI           RCC_CFGR_SW_HSI
#define RCC_SYSCLKSOURCE_HSE           RCC_CFGR_SW_HSE
#define RCC_SYSCLKSOURCE_PLLCLK        RCC_CFGR_SW_PLL1

/**
  * @}
  */

/** @defgroup RCC_System_Clock_Source_Status System Clock Source Status
  * @{
  */
#define RCC_SYSCLKSOURCE_STATUS_CSI    RCC_CFGR_SWS_CSI   /*!< CSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSI    RCC_CFGR_SWS_HSI   /*!< HSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSE    RCC_CFGR_SWS_HSE   /*!< HSE used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_PLLCLK RCC_CFGR_SWS_PLL1  /*!< PLL1 used as system clock */
/**
  * @}
  */

/** @defgroup RCC_SYS_Clock_Source  RCC SYS Clock Source
  * @{
  */
#if defined(RCC_D1CFGR_D1CPRE_DIV1)
#define RCC_SYSCLK_DIV1                RCC_D1CFGR_D1CPRE_DIV1
#define RCC_SYSCLK_DIV2                RCC_D1CFGR_D1CPRE_DIV2
#define RCC_SYSCLK_DIV4                RCC_D1CFGR_D1CPRE_DIV4
#define RCC_SYSCLK_DIV8                RCC_D1CFGR_D1CPRE_DIV8
#define RCC_SYSCLK_DIV16               RCC_D1CFGR_D1CPRE_DIV16
#define RCC_SYSCLK_DIV64               RCC_D1CFGR_D1CPRE_DIV64
#define RCC_SYSCLK_DIV128              RCC_D1CFGR_D1CPRE_DIV128
#define RCC_SYSCLK_DIV256              RCC_D1CFGR_D1CPRE_DIV256
#define RCC_SYSCLK_DIV512              RCC_D1CFGR_D1CPRE_DIV512
#else
#define RCC_SYSCLK_DIV1                RCC_CDCFGR1_CDCPRE_DIV1
#define RCC_SYSCLK_DIV2                RCC_CDCFGR1_CDCPRE_DIV2
#define RCC_SYSCLK_DIV4                RCC_CDCFGR1_CDCPRE_DIV4
#define RCC_SYSCLK_DIV8                RCC_CDCFGR1_CDCPRE_DIV8
#define RCC_SYSCLK_DIV16               RCC_CDCFGR1_CDCPRE_DIV16
#define RCC_SYSCLK_DIV64               RCC_CDCFGR1_CDCPRE_DIV64
#define RCC_SYSCLK_DIV128              RCC_CDCFGR1_CDCPRE_DIV128
#define RCC_SYSCLK_DIV256              RCC_CDCFGR1_CDCPRE_DIV256
#define RCC_SYSCLK_DIV512              RCC_CDCFGR1_CDCPRE_DIV512
#endif
/**
  * @}
  */


/** @defgroup RCC_HCLK_Clock_Source  RCC HCLK Clock Source
  * @{
  */
#if defined(RCC_D1CFGR_HPRE_DIV1)
#define RCC_HCLK_DIV1                RCC_D1CFGR_HPRE_DIV1
#define RCC_HCLK_DIV2                RCC_D1CFGR_HPRE_DIV2
#define RCC_HCLK_DIV4                RCC_D1CFGR_HPRE_DIV4
#define RCC_HCLK_DIV8                RCC_D1CFGR_HPRE_DIV8
#define RCC_HCLK_DIV16               RCC_D1CFGR_HPRE_DIV16
#define RCC_HCLK_DIV64               RCC_D1CFGR_HPRE_DIV64
#define RCC_HCLK_DIV128              RCC_D1CFGR_HPRE_DIV128
#define RCC_HCLK_DIV256              RCC_D1CFGR_HPRE_DIV256
#define RCC_HCLK_DIV512              RCC_D1CFGR_HPRE_DIV512
#else
#define RCC_HCLK_DIV1                RCC_CDCFGR1_HPRE_DIV1
#define RCC_HCLK_DIV2                RCC_CDCFGR1_HPRE_DIV2
#define RCC_HCLK_DIV4                RCC_CDCFGR1_HPRE_DIV4
#define RCC_HCLK_DIV8                RCC_CDCFGR1_HPRE_DIV8
#define RCC_HCLK_DIV16               RCC_CDCFGR1_HPRE_DIV16
#define RCC_HCLK_DIV64               RCC_CDCFGR1_HPRE_DIV64
#define RCC_HCLK_DIV128              RCC_CDCFGR1_HPRE_DIV128
#define RCC_HCLK_DIV256              RCC_CDCFGR1_HPRE_DIV256
#define RCC_HCLK_DIV512              RCC_CDCFGR1_HPRE_DIV512
#endif
/**
  * @}
  */

/** @defgroup RCC_APB3_Clock_Source  RCC APB3 Clock Source
  * @{
  */
#if defined (RCC_D1CFGR_D1PPRE_DIV1)
#define RCC_APB3_DIV1                  RCC_D1CFGR_D1PPRE_DIV1
#define RCC_APB3_DIV2                  RCC_D1CFGR_D1PPRE_DIV2
#define RCC_APB3_DIV4                  RCC_D1CFGR_D1PPRE_DIV4
#define RCC_APB3_DIV8                  RCC_D1CFGR_D1PPRE_DIV8
#define RCC_APB3_DIV16                 RCC_D1CFGR_D1PPRE_DIV16
#else
#define RCC_APB3_DIV1                  RCC_CDCFGR1_CDPPRE_DIV1
#define RCC_APB3_DIV2                  RCC_CDCFGR1_CDPPRE_DIV2
#define RCC_APB3_DIV4                  RCC_CDCFGR1_CDPPRE_DIV4
#define RCC_APB3_DIV8                  RCC_CDCFGR1_CDPPRE_DIV8
#define RCC_APB3_DIV16                 RCC_CDCFGR1_CDPPRE_DIV16
#endif
/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Source  RCC APB1 Clock Source
  * @{
  */
#if defined (RCC_D2CFGR_D2PPRE1_DIV1)
#define RCC_APB1_DIV1                  RCC_D2CFGR_D2PPRE1_DIV1
#define RCC_APB1_DIV2                  RCC_D2CFGR_D2PPRE1_DIV2
#define RCC_APB1_DIV4                  RCC_D2CFGR_D2PPRE1_DIV4
#define RCC_APB1_DIV8                  RCC_D2CFGR_D2PPRE1_DIV8
#define RCC_APB1_DIV16                 RCC_D2CFGR_D2PPRE1_DIV16
#else
#define RCC_APB1_DIV1                  RCC_CDCFGR2_CDPPRE1_DIV1
#define RCC_APB1_DIV2                  RCC_CDCFGR2_CDPPRE1_DIV2
#define RCC_APB1_DIV4                  RCC_CDCFGR2_CDPPRE1_DIV4
#define RCC_APB1_DIV8                  RCC_CDCFGR2_CDPPRE1_DIV8
#define RCC_APB1_DIV16                 RCC_CDCFGR2_CDPPRE1_DIV16
#endif

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Source  RCC APB2 Clock Source
  * @{
  */
#if defined (RCC_D2CFGR_D2PPRE2_DIV1)
#define RCC_APB2_DIV1                  RCC_D2CFGR_D2PPRE2_DIV1
#define RCC_APB2_DIV2                  RCC_D2CFGR_D2PPRE2_DIV2
#define RCC_APB2_DIV4                  RCC_D2CFGR_D2PPRE2_DIV4
#define RCC_APB2_DIV8                  RCC_D2CFGR_D2PPRE2_DIV8
#define RCC_APB2_DIV16                 RCC_D2CFGR_D2PPRE2_DIV16
#else
#define RCC_APB2_DIV1                  RCC_CDCFGR2_CDPPRE2_DIV1
#define RCC_APB2_DIV2                  RCC_CDCFGR2_CDPPRE2_DIV2
#define RCC_APB2_DIV4                  RCC_CDCFGR2_CDPPRE2_DIV4
#define RCC_APB2_DIV8                  RCC_CDCFGR2_CDPPRE2_DIV8
#define RCC_APB2_DIV16                 RCC_CDCFGR2_CDPPRE2_DIV16
#endif
/**
  * @}
  */

/** @defgroup RCC_APB4_Clock_Source  RCC APB4 Clock Source
  * @{
  */
#if defined(RCC_D3CFGR_D3PPRE_DIV1)
#define RCC_APB4_DIV1                  RCC_D3CFGR_D3PPRE_DIV1
#define RCC_APB4_DIV2                  RCC_D3CFGR_D3PPRE_DIV2
#define RCC_APB4_DIV4                  RCC_D3CFGR_D3PPRE_DIV4
#define RCC_APB4_DIV8                  RCC_D3CFGR_D3PPRE_DIV8
#define RCC_APB4_DIV16                 RCC_D3CFGR_D3PPRE_DIV16
#else
#define RCC_APB4_DIV1                  RCC_SRDCFGR_SRDPPRE_DIV1
#define RCC_APB4_DIV2                  RCC_SRDCFGR_SRDPPRE_DIV2
#define RCC_APB4_DIV4                  RCC_SRDCFGR_SRDPPRE_DIV4
#define RCC_APB4_DIV8                  RCC_SRDCFGR_SRDPPRE_DIV8
#define RCC_APB4_DIV16                 RCC_SRDCFGR_SRDPPRE_DIV16
#endif
/**
  * @}
  */

/** @defgroup RCC_RTC_Clock_Source  RCC RTC Clock Source
  * @{
  */
#define RCC_RTCCLKSOURCE_NO_CLK          (0x00000000U)
#define RCC_RTCCLKSOURCE_LSE             (0x00000100U)
#define RCC_RTCCLKSOURCE_LSI             (0x00000200U)
#define RCC_RTCCLKSOURCE_HSE_DIV2        (0x00002300U)
#define RCC_RTCCLKSOURCE_HSE_DIV3        (0x00003300U)
#define RCC_RTCCLKSOURCE_HSE_DIV4        (0x00004300U)
#define RCC_RTCCLKSOURCE_HSE_DIV5        (0x00005300U)
#define RCC_RTCCLKSOURCE_HSE_DIV6        (0x00006300U)
#define RCC_RTCCLKSOURCE_HSE_DIV7        (0x00007300U)
#define RCC_RTCCLKSOURCE_HSE_DIV8        (0x00008300U)
#define RCC_RTCCLKSOURCE_HSE_DIV9        (0x00009300U)
#define RCC_RTCCLKSOURCE_HSE_DIV10       (0x0000A300U)
#define RCC_RTCCLKSOURCE_HSE_DIV11       (0x0000B300U)
#define RCC_RTCCLKSOURCE_HSE_DIV12       (0x0000C300U)
#define RCC_RTCCLKSOURCE_HSE_DIV13       (0x0000D300U)
#define RCC_RTCCLKSOURCE_HSE_DIV14       (0x0000E300U)
#define RCC_RTCCLKSOURCE_HSE_DIV15       (0x0000F300U)
#define RCC_RTCCLKSOURCE_HSE_DIV16       (0x00010300U)
#define RCC_RTCCLKSOURCE_HSE_DIV17       (0x00011300U)
#define RCC_RTCCLKSOURCE_HSE_DIV18       (0x00012300U)
#define RCC_RTCCLKSOURCE_HSE_DIV19       (0x00013300U)
#define RCC_RTCCLKSOURCE_HSE_DIV20       (0x00014300U)
#define RCC_RTCCLKSOURCE_HSE_DIV21       (0x00015300U)
#define RCC_RTCCLKSOURCE_HSE_DIV22       (0x00016300U)
#define RCC_RTCCLKSOURCE_HSE_DIV23       (0x00017300U)
#define RCC_RTCCLKSOURCE_HSE_DIV24       (0x00018300U)
#define RCC_RTCCLKSOURCE_HSE_DIV25       (0x00019300U)
#define RCC_RTCCLKSOURCE_HSE_DIV26       (0x0001A300U)
#define RCC_RTCCLKSOURCE_HSE_DIV27       (0x0001B300U)
#define RCC_RTCCLKSOURCE_HSE_DIV28       (0x0001C300U)
#define RCC_RTCCLKSOURCE_HSE_DIV29       (0x0001D300U)
#define RCC_RTCCLKSOURCE_HSE_DIV30       (0x0001E300U)
#define RCC_RTCCLKSOURCE_HSE_DIV31       (0x0001F300U)
#define RCC_RTCCLKSOURCE_HSE_DIV32       (0x00020300U)
#define RCC_RTCCLKSOURCE_HSE_DIV33       (0x00021300U)
#define RCC_RTCCLKSOURCE_HSE_DIV34       (0x00022300U)
#define RCC_RTCCLKSOURCE_HSE_DIV35       (0x00023300U)
#define RCC_RTCCLKSOURCE_HSE_DIV36       (0x00024300U)
#define RCC_RTCCLKSOURCE_HSE_DIV37       (0x00025300U)
#define RCC_RTCCLKSOURCE_HSE_DIV38       (0x00026300U)
#define RCC_RTCCLKSOURCE_HSE_DIV39       (0x00027300U)
#define RCC_RTCCLKSOURCE_HSE_DIV40       (0x00028300U)
#define RCC_RTCCLKSOURCE_HSE_DIV41       (0x00029300U)
#define RCC_RTCCLKSOURCE_HSE_DIV42       (0x0002A300U)
#define RCC_RTCCLKSOURCE_HSE_DIV43       (0x0002B300U)
#define RCC_RTCCLKSOURCE_HSE_DIV44       (0x0002C300U)
#define RCC_RTCCLKSOURCE_HSE_DIV45       (0x0002D300U)
#define RCC_RTCCLKSOURCE_HSE_DIV46       (0x0002E300U)
#define RCC_RTCCLKSOURCE_HSE_DIV47       (0x0002F300U)
#define RCC_RTCCLKSOURCE_HSE_DIV48       (0x00030300U)
#define RCC_RTCCLKSOURCE_HSE_DIV49       (0x00031300U)
#define RCC_RTCCLKSOURCE_HSE_DIV50       (0x00032300U)
#define RCC_RTCCLKSOURCE_HSE_DIV51       (0x00033300U)
#define RCC_RTCCLKSOURCE_HSE_DIV52       (0x00034300U)
#define RCC_RTCCLKSOURCE_HSE_DIV53       (0x00035300U)
#define RCC_RTCCLKSOURCE_HSE_DIV54       (0x00036300U)
#define RCC_RTCCLKSOURCE_HSE_DIV55       (0x00037300U)
#define RCC_RTCCLKSOURCE_HSE_DIV56       (0x00038300U)
#define RCC_RTCCLKSOURCE_HSE_DIV57       (0x00039300U)
#define RCC_RTCCLKSOURCE_HSE_DIV58       (0x0003A300U)
#define RCC_RTCCLKSOURCE_HSE_DIV59       (0x0003B300U)
#define RCC_RTCCLKSOURCE_HSE_DIV60       (0x0003C300U)
#define RCC_RTCCLKSOURCE_HSE_DIV61       (0x0003D300U)
#define RCC_RTCCLKSOURCE_HSE_DIV62       (0x0003E300U)
#define RCC_RTCCLKSOURCE_HSE_DIV63       (0x0003F300U)


/**
  * @}
  */


/** @defgroup RCC_MCO_Index  RCC MCO Index
  * @{
  */
#define RCC_MCO1                         (0x00000000U)
#define RCC_MCO2                         (0x00000001U)

/**
  * @}
  */

/** @defgroup RCC_MCO1_Clock_Source  RCC MCO1 Clock Source
  * @{
  */
#define RCC_MCO1SOURCE_HSI               (0x00000000U)
#define RCC_MCO1SOURCE_LSE               RCC_CFGR_MCO1_0
#define RCC_MCO1SOURCE_HSE               RCC_CFGR_MCO1_1
#define RCC_MCO1SOURCE_PLL1QCLK          ((uint32_t)RCC_CFGR_MCO1_0 | RCC_CFGR_MCO1_1)
#define RCC_MCO1SOURCE_HSI48              RCC_CFGR_MCO1_2

/**
  * @}
  */

/** @defgroup RCC_MCO2_Clock_Source  RCC MCO2 Clock Source
  * @{
  */
#define RCC_MCO2SOURCE_SYSCLK            (0x00000000U)
#define RCC_MCO2SOURCE_PLL2PCLK          RCC_CFGR_MCO2_0
#define RCC_MCO2SOURCE_HSE               RCC_CFGR_MCO2_1
#define RCC_MCO2SOURCE_PLLCLK            ((uint32_t)RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1)
#define RCC_MCO2SOURCE_CSICLK            RCC_CFGR_MCO2_2
#define RCC_MCO2SOURCE_LSICLK            ((uint32_t)RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_2)

/**
  * @}
  */

/** @defgroup RCC_MCOx_Clock_Prescaler  RCC MCOx Clock Prescaler
  * @{
  */
#define RCC_MCODIV_1                    RCC_CFGR_MCO1PRE_0
#define RCC_MCODIV_2                    RCC_CFGR_MCO1PRE_1
#define RCC_MCODIV_3                    ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_1)
#define RCC_MCODIV_4                    RCC_CFGR_MCO1PRE_2
#define RCC_MCODIV_5                    ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCODIV_6                    ((uint32_t)RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCODIV_7                    ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCODIV_8                    RCC_CFGR_MCO1PRE_3
#define RCC_MCODIV_9                    ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_3)
#define RCC_MCODIV_10                   ((uint32_t)RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_3)
#define RCC_MCODIV_11                   ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_3)
#define RCC_MCODIV_12                   ((uint32_t)RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_3)
#define RCC_MCODIV_13                   ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_3)
#define RCC_MCODIV_14                   ((uint32_t)RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_3)
#define RCC_MCODIV_15                   RCC_CFGR_MCO1PRE


/**
  * @}
  */

/** @defgroup RCC_Interrupt  RCC Interrupt
  * @{
  */
#define RCC_IT_LSIRDY                  (0x00000001U)
#define RCC_IT_LSERDY                  (0x00000002U)
#define RCC_IT_HSIRDY                  (0x00000004U)
#define RCC_IT_HSERDY                  (0x00000008U)
#define RCC_IT_CSIRDY                  (0x00000010U)
#define RCC_IT_HSI48RDY                (0x00000020U)
#define RCC_IT_PLLRDY                  (0x00000040U)
#define RCC_IT_PLL2RDY                 (0x00000080U)
#define RCC_IT_PLL3RDY                 (0x00000100U)
#define RCC_IT_LSECSS                  (0x00000200U)
#define RCC_IT_CSS                     (0x00000400U)
/**
  * @}
  */

/** @defgroup RCC_Flag  RCC Flag
  *        Elements values convention: XXXYYYYYb
  *           - YYYYY  : Flag position in the register
  *           - XXX  : Register index
  *                 - 001: CR register
  *                 - 010: BDCR register
  *                 - 011: CSR register
  *                 - 100: RSR register
  * @{
  */
/* Flags in the CR register */
#define RCC_FLAG_HSIRDY                ((uint8_t)0x22)
#define RCC_FLAG_HSIDIV                ((uint8_t)0x25)
#define RCC_FLAG_CSIRDY                ((uint8_t)0x28)
#define RCC_FLAG_HSI48RDY              ((uint8_t)0x2D)
#if defined(RCC_CR_D1CKRDY)
#define RCC_FLAG_D1CKRDY               ((uint8_t)0x2E)
#else
#define RCC_FLAG_CPUCKRDY              ((uint8_t)0x2E)
#define RCC_FLAG_D1CKRDY               RCC_FLAG_CPUCKRDY   /* alias */
#endif /* RCC_CR_D1CKRDY */
#if defined(RCC_CR_D2CKRDY)
#define RCC_FLAG_D2CKRDY               ((uint8_t)0x2F)
#else
#define RCC_FLAG_CDCKRDY               ((uint8_t)0x2F)
#define RCC_FLAG_D2CKRDY               RCC_FLAG_CDCKRDY    /* alias */
#endif /* RCC_CR_D2CKRDY */
#define RCC_FLAG_HSERDY                ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                ((uint8_t)0x39)
#define RCC_FLAG_PLL2RDY               ((uint8_t)0x3B)
#define RCC_FLAG_PLL3RDY               ((uint8_t)0x3D)
/* Flags in the BDCR register */
#define RCC_FLAG_LSERDY                ((uint8_t)0x41)

/* Flags in the CSR register */
#define RCC_FLAG_LSIRDY                ((uint8_t)0x61)

/* Flags in the RSR register */
#if defined(RCC_RSR_CPURSTF)
#define RCC_FLAG_CPURST                ((uint8_t)0x91)
#endif /* RCC_RSR_CPURSTF */

#if defined(RCC_RSR_D1RSTF)
#define RCC_FLAG_D1RST                 ((uint8_t)0x93)
#else
#define RCC_FLAG_CDRST                 ((uint8_t)0x93)
#endif /* RCC_RSR_D1RSTF */
#if defined(RCC_RSR_D2RSTF)
#define RCC_FLAG_D2RST                 ((uint8_t)0x94)
#endif /* RCC_RSR_D2RSTF */
#define RCC_FLAG_BORRST                ((uint8_t)0x95)
#define RCC_FLAG_PINRST                ((uint8_t)0x96)
#define RCC_FLAG_PORRST                ((uint8_t)0x97)
#define RCC_FLAG_SFTRST                ((uint8_t)0x98)
#define RCC_FLAG_IWDG1RST              ((uint8_t)0x9A)
#define RCC_FLAG_WWDG1RST              ((uint8_t)0x9C)
#define RCC_FLAG_LPWR1RST              ((uint8_t)0x9E)
#define RCC_FLAG_LPWR2RST              ((uint8_t)0x9F)

#if defined(DUAL_CORE)
#define RCC_FLAG_C1RST                 (RCC_FLAG_CPURST)
#define RCC_FLAG_C2RST                 ((uint8_t)0x92)
#define RCC_FLAG_SFTR1ST               (RCC_FLAG_SFTRST)
#define RCC_FLAG_SFTR2ST               ((uint8_t)0x99)
#define RCC_FLAG_WWDG2RST              ((uint8_t)0x9D)
#define RCC_FLAG_IWDG2RST              ((uint8_t)0x9B)
#endif /*DUAL_CORE*/


/**
  * @}
  */

/** @defgroup RCC_LSEDrive_Config LSE Drive Config
  * @{
  */
#define RCC_LSEDRIVE_LOW                 (0x00000000U) /*!< LSE low drive capability */
#define RCC_LSEDRIVE_MEDIUMLOW           RCC_BDCR_LSEDRV_0      /*!< LSE medium low drive capability */
#define RCC_LSEDRIVE_MEDIUMHIGH          RCC_BDCR_LSEDRV_1      /*!< LSE medium high drive capability */
#define RCC_LSEDRIVE_HIGH                RCC_BDCR_LSEDRV        /*!< LSE high drive capability */
/**
  * @}
  */

/** @defgroup RCC_Stop_WakeUpClock  RCC Stop WakeUpClock
  * @{
  */
#define RCC_STOP_WAKEUPCLOCK_HSI       (0x00000000U)
#define RCC_STOP_WAKEUPCLOCK_CSI       RCC_CFGR_STOPWUCK

/**
  * @}
  */

/** @defgroup RCC_Stop_KernelWakeUpClock  RCC Stop KernelWakeUpClock
  * @{
  */
#define RCC_STOP_KERWAKEUPCLOCK_HSI       (0x00000000U)
#define RCC_STOP_KERWAKEUPCLOCK_CSI        RCC_CFGR_STOPKERWUCK


/**
  * @}
  */

#if defined(RCC_VER_X)
#define HAL_RCC_REV_Y_HSITRIM_Pos  (12U)
#define HAL_RCC_REV_Y_HSITRIM_Msk  (0x3F000U)
#define HAL_RCC_REV_Y_CSITRIM_Pos  (26U)
#define HAL_RCC_REV_Y_CSITRIM_Msk  (0x7C000000U)
#endif /* RCC_VER_X */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup RCC_Exported_Macros RCC Exported Macros
  * @{
  */

/** @brief  Enable or disable the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_MDMA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_MDMAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_MDMAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_DMA2D_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_DMA2DEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_DMA2DEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(JPEG)
#define __HAL_RCC_JPGDECEN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_JPGDECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_JPGDECEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* JPEG */

#define __HAL_RCC_FMC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(QUADSPI)
#define __HAL_RCC_QSPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* QUADSPI */
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OSPI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OSPI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* OCTOSPI1 */
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OSPI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OSPI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* OCTOSPI2 */
#if defined(OCTOSPIM)
#define __HAL_RCC_OCTOSPIM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IOMNGREN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IOMNGREN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* OCTOSPIM */
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OTFDEC1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OTFDEC1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* OTFDEC1 */
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OTFDEC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_OTFDEC2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* OTFDEC2 */
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_GFXMMUEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_GFXMMUEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* GFXMMU */
#define __HAL_RCC_SDMMC1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_SDMMC1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_SDMMC1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_MDMA_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_MDMAEN))
#define __HAL_RCC_DMA2D_CLK_DISABLE()           (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_DMA2DEN))
#if defined(JPEG)
#define __HAL_RCC_JPGDECEN_CLK_DISABLE()        (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_JPGDECEN))
#endif /* JPEG */
#define __HAL_RCC_FMC_CLK_DISABLE()             (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_FMCEN))

#if defined(QUADSPI)
#define __HAL_RCC_QSPI_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_QSPIEN))
#endif /* QUADSPI */
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_OSPI1EN))
#endif /* OCTOSPII */
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_OSPI2EN))
#endif /* OCTOSPI2 */
#define __HAL_RCC_SDMMC1_CLK_DISABLE()          (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_SDMMC1EN))
#if defined(OCTOSPIM)
#define __HAL_RCC_OCTOSPIM_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_IOMNGREN))
#endif /* OCTOSPIM */
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_OTFDEC1EN))
#endif /* OTOFDEC1 */
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_OTFDEC2EN))
#endif /* OTOFDEC2 */
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_DISABLE()            (RCC->AHB3ENR &= ~ (RCC_AHB3ENR_GFXMMUEN))
#endif /* GFXMMU */

/** @brief  Get the enable or disable status of the AHB3 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_MDMA_IS_CLK_ENABLED()          ((RCC->AHB3ENR & RCC_AHB3ENR_MDMAEN)   != 0U)
#define __HAL_RCC_DMA2D_IS_CLK_ENABLED()         ((RCC->AHB3ENR & RCC_AHB3ENR_DMA2DEN)  != 0U)
#if defined(JPEG)
#define __HAL_RCC_JPGDECEN_IS_CLK_ENABLED()      ((RCC->AHB3ENR & RCC_AHB3ENR_JPGDECEN) != 0U)
#endif /* JPEG */
#define __HAL_RCC_FMC_IS_CLK_ENABLED()           ((RCC->AHB3ENR & RCC_AHB3ENR_FMCEN)    != 0U)
#if defined (QUADSPI)
#define __HAL_RCC_QSPI_IS_CLK_ENABLED()          ((RCC->AHB3ENR & RCC_AHB3ENR_QSPIEN)   != 0U)
#endif /* QUADSPI */
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_IS_CLK_ENABLED()          ((RCC->AHB3ENR & RCC_AHB3ENR_OSPI1EN)   != 0U)
#endif /* OCTOSPII */
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_IS_CLK_ENABLED()          ((RCC->AHB3ENR & RCC_AHB3ENR_OSPI2EN)   != 0U)
#endif /* OCTOSPI2 */
#define __HAL_RCC_SDMMC1_IS_CLK_ENABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_SDMMC1EN) != 0U)
#if defined(OCTOSPIM)
#define __HAL_RCC_OCTOSPIM_IS_CLK_ENABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_IOMNGREN) != 0U)
#endif /* OCTOSPIM */
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_IS_CLK_ENABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_OTFDEC1EN) != 0U)
#endif /* OTOFDEC1 */
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_IS_CLK_ENABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_OTFDEC2EN) != 0U)
#endif /* OTOFDEC2 */
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_IS_CLK_ENABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_GFXMMUEN) != 0U)
#endif /* GFXMMU */

#define __HAL_RCC_MDMA_IS_CLK_DISABLED()         ((RCC->AHB3ENR & RCC_AHB3ENR_MDMAEN)   == 0U)
#define __HAL_RCC_DMA2D_IS_CLK_DISABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_DMA2DEN)  == 0U)
#if defined(JPEG)
#define __HAL_RCC_JPGDECEN_IS_CLK_DISABLED()     ((RCC->AHB3ENR & RCC_AHB3ENR_JPGDECEN) == 0U)
#endif /* JPEG */
#define __HAL_RCC_FMC_IS_CLK_DISABLED()          ((RCC->AHB3ENR & RCC_AHB3ENR_FMCEN)    == 0U)
#if defined (QUADSPI)
#define __HAL_RCC_QSPI_IS_CLK_DISABLED()         ((RCC->AHB3ENR & RCC_AHB3ENR_QSPIEN)   == 0U)
#endif /* QUADSPI */
#define __HAL_RCC_SDMMC1_IS_CLK_DISABLED()       ((RCC->AHB3ENR & RCC_AHB3ENR_SDMMC1EN) == 0U)
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_IS_CLK_DISABLED()        ((RCC->AHB3ENR & RCC_AHB3ENR_OSPI1EN) == 0U)
#endif
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_IS_CLK_DISABLED()     ((RCC->AHB3ENR & RCC_AHB3ENR_OSPI2EN) == 0U)
#endif
#if defined(OCTOSPIM)
#define __HAL_RCC_OCTOSPIM_IS_CLK_DISABLED()     ((RCC->AHB3ENR & RCC_AHB3ENR_IOMNGREN) == 0U)
#endif
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_IS_CLK_DISABLED()      ((RCC->AHB3ENR & RCC_AHB3ENR_OTFDEC1EN) == 0U)
#endif
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_IS_CLK_DISABLED()      ((RCC->AHB3ENR & RCC_AHB3ENR_OTFDEC2EN) == 0U)
#endif
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_IS_CLK_DISABLED()       ((RCC->AHB3ENR & RCC_AHB3ENR_GFXMMUEN) == 0U)
#endif
/** @brief  Enable or disable the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_DMA1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_DMA2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_ADC12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ADC12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ADC12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(DUAL_CORE)
#define __HAL_RCC_ART_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ARTEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ARTEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*DUAL_CORE*/

#if defined(RCC_AHB1ENR_CRCEN)
#define __HAL_RCC_CRC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#if defined(ETH)
#define __HAL_RCC_ETH1MAC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETH1MACEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETH1MACEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_ETH1TX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETH1TXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETH1TXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_ETH1RX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETH1RXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETH1RXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#define __HAL_RCC_USB1_OTG_HS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_USB1_OTG_HS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_USB2_OTG_FS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#define __HAL_RCC_DMA1_CLK_DISABLE()             (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_DMA1EN))
#define __HAL_RCC_DMA2_CLK_DISABLE()             (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_DMA2EN))
#define __HAL_RCC_ADC12_CLK_DISABLE()            (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_ADC12EN))
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_CLK_DISABLE()              (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_ARTEN))
#endif /*DUAL_CORE*/
#if defined(RCC_AHB1ENR_CRCEN)
#define __HAL_RCC_CRC_CLK_DISABLE()              (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_CRCEN))
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_CLK_DISABLE()          (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1MACEN))
#define __HAL_RCC_ETH1TX_CLK_DISABLE()           (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1TXEN))
#define __HAL_RCC_ETH1RX_CLK_DISABLE()           (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1RXEN))
#endif
#define __HAL_RCC_USB1_OTG_HS_CLK_DISABLE()      (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_USB1OTGHSEN))
#define __HAL_RCC_USB1_OTG_HS_ULPI_CLK_DISABLE() (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_USB1OTGHSULPIEN))
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_CLK_DISABLE()      (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_USB2OTGHSEN))
#define __HAL_RCC_USB2_OTG_FS_ULPI_CLK_DISABLE() (RCC->AHB1ENR &= ~ (RCC_AHB1ENR_USB2OTGHSULPIEN))
#endif /* USB2_OTG_FS */

/** @brief  Get the enable or disable status of the AHB1 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_DMA1_IS_CLK_ENABLED()              ((RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN)          != 0U)
#define __HAL_RCC_DMA2_IS_CLK_ENABLED()              ((RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN)          != 0U)
#define __HAL_RCC_ADC12_IS_CLK_ENABLED()             ((RCC->AHB1ENR & RCC_AHB1ENR_ADC12EN)         != 0U)
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_IS_CLK_ENABLED()               ((RCC->AHB1ENR & RCC_AHB1ENR_ARTEN)           != 0U)
#endif /*DUAL_CORE*/
#if defined(RCC_AHB1ENR_CRCEN)
#define __HAL_RCC_CRC_IS_CLK_ENABLED()              ((RCC->AHB1ENR & RCC_AHB1ENR_CRCEN)       != 0U)
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_IS_CLK_ENABLED()           ((RCC->AHB1ENR & RCC_AHB1ENR_ETH1MACEN)       != 0U)
#define __HAL_RCC_ETH1TX_IS_CLK_ENABLED()            ((RCC->AHB1ENR & RCC_AHB1ENR_ETH1TXEN)        != 0U)
#define __HAL_RCC_ETH1RX_IS_CLK_ENABLED()            ((RCC->AHB1ENR & RCC_AHB1ENR_ETH1RXEN)        != 0U)
#endif
#define __HAL_RCC_USB1_OTG_HS_IS_CLK_ENABLED()       ((RCC->AHB1ENR & RCC_AHB1ENR_USB1OTGHSEN)     != 0U)
#define __HAL_RCC_USB1_OTG_HS_ULPI_IS_CLK_ENABLED()  ((RCC->AHB1ENR & RCC_AHB1ENR_USB1OTGHSULPIEN) != 0U)
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_IS_CLK_ENABLED()       ((RCC->AHB1ENR & RCC_AHB1ENR_USB2OTGHSEN)     != 0U)
#define __HAL_RCC_USB2_OTG_FS_ULPI_IS_CLK_ENABLED()  ((RCC->AHB1ENR & RCC_AHB1ENR_USB2OTGHSULPIEN) != 0U)
#endif /* USB2_OTG_FS */

#define __HAL_RCC_DMA1_IS_CLK_DISABLED()             ((RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN)          == 0U)
#define __HAL_RCC_DMA2_IS_CLK_DISABLED()             ((RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN)          == 0U)
#define __HAL_RCC_ADC12_IS_CLK_DISABLED()            ((RCC->AHB1ENR & RCC_AHB1ENR_ADC12EN)         == 0U)
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_IS_CLK_DISABLED()              ((RCC->AHB1ENR & RCC_AHB1ENR_ARTEN)           == 0U)
#endif /*DUAL_CORE*/
#if defined(RCC_AHB1ENR_CRCEN)
#define __HAL_RCC_CRC_IS_CLK_DISABLED()              ((RCC->AHB1ENR & RCC_AHB1ENR_CRCEN)           == 0U)
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_IS_CLK_DISABLED()          ((RCC->AHB1ENR & RCC_AHB1ENR_ETH1MACEN)       == 0U)
#define __HAL_RCC_ETH1TX_IS_CLK_DISABLED()           ((RCC->AHB1ENR & RCC_AHB1ENR_ETH1TXEN)        == 0U)
#define __HAL_RCC_ETH1RX_IS_CLK_DISABLED()           ((RCC->AHB1ENR & RCC_AHB1ENR_ETH1RXEN)        == 0U)
#endif
#define __HAL_RCC_USB1_OTG_HS_IS_CLK_DISABLED()      ((RCC->AHB1ENR & RCC_AHB1ENR_USB1OTGHSEN)     == 0U)
#define __HAL_RCC_USB1_OTG_HS_ULPI_IS_CLK_DISABLED() ((RCC->AHB1ENR & RCC_AHB1ENR_USB1OTGHSULPIEN) == 0U)
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_IS_CLK_DISABLED()      ((RCC->AHB1ENR & RCC_AHB1ENR_USB2OTGHSEN)     == 0U)
#define __HAL_RCC_USB2_OTG_FS_ULPI_IS_CLK_DISABLED() ((RCC->AHB1ENR & RCC_AHB1ENR_USB2OTGHSULPIEN) == 0U)
#endif /* USB2_OTG_FS */

/** @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMI_PSSIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMI_PSSIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_DCMI_CLK_ENABLE()  __HAL_RCC_DCMI_PSSI_CLK_ENABLE()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* DCMI && PSSI */

#if defined(CRYP)
#define __HAL_RCC_CRYP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* CRYP */

#if defined(HASH)
#define __HAL_RCC_HASH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* HASH */

#define __HAL_RCC_RNG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SDMMC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_SDMMC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_SDMMC2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(RCC_AHB2ENR_D2SRAM1EN)
#define __HAL_RCC_D2SRAM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_D2SRAM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_D2SRAM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#else
#define __HAL_RCC_AHBSRAM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AHBSRAM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AHBSRAM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* RCC_AHB2ENR_D2SRAM1EN */

#if defined(RCC_AHB2ENR_D2SRAM2EN)
#define __HAL_RCC_D2SRAM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_D2SRAM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_D2SRAM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#else
#define __HAL_RCC_AHBSRAM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AHBSRAM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AHBSRAM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* RCC_AHB2ENR_D2SRAM2EN */

#if defined(RCC_AHB2ENR_D2SRAM3EN)
#define __HAL_RCC_D2SRAM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_D2SRAM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_D2SRAM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#if defined(RCC_AHB2ENR_HSEMEN)
#define __HAL_RCC_HSEM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_HSEMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_HSEMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* RCC_AHB2ENR_HSEMEN */

#if defined(BDMA1)
#define __HAL_RCC_BDMA1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_BDMA1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_BDMA1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* BDMA1 */

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_CLK_DISABLE()         (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_DCMI_PSSIEN))
#define __HAL_RCC_DCMI_CLK_DISABLE()              __HAL_RCC_DCMI_PSSI_CLK_DISABLE()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_CLK_DISABLE()             (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_DCMIEN))
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_CLK_DISABLE()             (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_CRYPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_CLK_DISABLE()             (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_HASHEN))
#endif /* HASH */
#define __HAL_RCC_RNG_CLK_DISABLE()              (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_RNGEN))
#define __HAL_RCC_SDMMC2_CLK_DISABLE()           (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_SDMMC2EN))
#if defined(RCC_AHB2ENR_D2SRAM1EN)
#define __HAL_RCC_D2SRAM1_CLK_DISABLE()          (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM1EN))
#else
#define __HAL_RCC_AHBSRAM1_CLK_DISABLE()         (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_AHBSRAM1EN))
#endif /* RCC_AHB2ENR_D2SRAM1EN */
#if defined(RCC_AHB2ENR_D2SRAM2EN)
#define __HAL_RCC_D2SRAM2_CLK_DISABLE()          (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM2EN))
#else
#define __HAL_RCC_AHBSRAM2_CLK_DISABLE()         (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_AHBSRAM2EN))
#endif /* RCC_AHB2ENR_D2SRAM2EN */
#if defined(RCC_AHB2ENR_D2SRAM3EN)
#define __HAL_RCC_D2SRAM3_CLK_DISABLE()          (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM3EN))
#endif
#if defined(RCC_AHB2ENR_HSEMEN)
#define __HAL_RCC_HSEM_CLK_DISABLE()             (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_HSEMEN))
#endif
#if defined(BDMA1)
#define __HAL_RCC_BDMA1_CLK_DISABLE()            (RCC->AHB2ENR &= ~ (RCC_AHB2ENR_BDMA1EN))
#endif

/** @brief  Get the enable or disable status of the AHB2 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_IS_CLK_ENABLED()         ((RCC->AHB2ENR & RCC_AHB2ENR_DCMI_PSSIEN)    != 0U)
#define __HAL_RCC_DCMI_IS_CLK_ENABLED()              __HAL_RCC_DCMI_PSSI_IS_CLK_ENABLED()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_IS_CLK_ENABLED()              ((RCC->AHB2ENR & RCC_AHB2ENR_DCMIEN)    != 0U)
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_IS_CLK_ENABLED()              ((RCC->AHB2ENR & RCC_AHB2ENR_CRYPEN)    != 0U)
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_IS_CLK_ENABLED()              ((RCC->AHB2ENR & RCC_AHB2ENR_HASHEN)    != 0U)
#endif /* HASH */
#define __HAL_RCC_RNG_IS_CLK_ENABLED()               ((RCC->AHB2ENR & RCC_AHB2ENR_RNGEN)     != 0U)
#define __HAL_RCC_SDMMC2_IS_CLK_ENABLED()            ((RCC->AHB2ENR & RCC_AHB2ENR_SDMMC2EN)  != 0U)
#if defined(RCC_AHB2ENR_D2SRAM1EN)
#define __HAL_RCC_D2SRAM1_IS_CLK_ENABLED()           ((RCC->AHB2ENR & RCC_AHB2ENR_D2SRAM1EN) != 0U)
#else
#define __HAL_RCC_AHBSRAM1_IS_CLK_ENABLED()          ((RCC->AHB2ENR & RCC_AHB2ENR_AHBSRAM1EN) != 0U)
#endif /* RCC_AHB2ENR_D2SRAM1EN */
#if defined(RCC_AHB2ENR_D2SRAM2EN)
#define __HAL_RCC_D2SRAM2_IS_CLK_ENABLED()           ((RCC->AHB2ENR & RCC_AHB2ENR_D2SRAM2EN) != 0U)
#else
#define __HAL_RCC_AHBSRAM2_IS_CLK_ENABLED()          ((RCC->AHB2ENR & RCC_AHB2ENR_AHBSRAM2EN) != 0U)
#endif /* RCC_AHB2ENR_D2SRAM2EN */
#if defined(RCC_AHB2ENR_D2SRAM3EN)
#define __HAL_RCC_D2SRAM3_IS_CLK_ENABLED()           ((RCC->AHB2ENR & RCC_AHB2ENR_D2SRAM3EN) != 0U)
#endif
#if defined(RCC_AHB2ENR_HSEMEN)
#define __HAL_RCC_HSEM_IS_CLK_ENABLED()              ((RCC->AHB2ENR & RCC_AHB2ENR_HSEMEN)    != 0U)
#endif
#if defined(BDMA1)
#define __HAL_RCC_BDMA1_IS_CLK_ENABLED()             ((RCC->AHB2ENR & RCC_AHB2ENR_BDMA1EN)  != 0U)
#endif

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_IS_CLK_DISABLED()        ((RCC->AHB2ENR & RCC_AHB2ENR_DCMI_PSSIEN)    == 0U)
#define __HAL_RCC_DCMI_IS_CLK_DISABLED()             __HAL_RCC_DCMI_PSSI_IS_CLK_DISABLED()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_IS_CLK_DISABLED()             ((RCC->AHB2ENR & RCC_AHB2ENR_DCMIEN)    == 0U)
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_IS_CLK_DISABLED()             ((RCC->AHB2ENR & RCC_AHB2ENR_CRYPEN)    == 0U)
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_IS_CLK_DISABLED()             ((RCC->AHB2ENR & RCC_AHB2ENR_HASHEN)    == 0U)
#endif /* HASH */
#define __HAL_RCC_RNG_IS_CLK_DISABLED()              ((RCC->AHB2ENR & RCC_AHB2ENR_RNGEN)     == 0U)
#define __HAL_RCC_SDMMC2_IS_CLK_DISABLED()           ((RCC->AHB2ENR & RCC_AHB2ENR_SDMMC2EN)  == 0U)
#if defined(RCC_AHB2ENR_D2SRAM1EN)
#define __HAL_RCC_D2SRAM1_IS_CLK_DISABLED()          ((RCC->AHB2ENR & RCC_AHB2ENR_D2SRAM1EN) == 0U)
#else
#define __HAL_RCC_AHBSRAM1_IS_CLK_DISABLED()         ((RCC->AHB2ENR & RCC_AHB2ENR_AHBSRAM1EN) == 0U)
#endif /* RCC_AHB2ENR_D2SRAM1EN */
#if defined(RCC_AHB2ENR_D2SRAM2EN)
#define __HAL_RCC_D2SRAM2_IS_CLK_DISABLED()          ((RCC->AHB2ENR & RCC_AHB2ENR_D2SRAM2EN) == 0U)
#else
#define __HAL_RCC_AHBSRAM2_IS_CLK_DISABLED()         ((RCC->AHB2ENR & RCC_AHB2ENR_AHBSRAM2EN) == 0U)
#endif /* RCC_AHB2ENR_D2SRAM2EN */
#if defined(RCC_AHB2ENR_D2SRAM3EN)
#define __HAL_RCC_D2SRAM3_IS_CLK_DISABLED()          ((RCC->AHB2ENR & RCC_AHB2ENR_D2SRAM3EN) == 0U)
#endif
#if defined(RCC_AHB2ENR_HSEMEN)
#define __HAL_RCC_HSEM_IS_CLK_DISABLED()             ((RCC->AHB2ENR & RCC_AHB2ENR_HSEMEN)    == 0U)
#endif
#if defined(BDMA1)
#define __HAL_RCC_BDMA1_IS_CLK_DISABLED()            ((RCC->AHB2ENR & RCC_AHB2ENR_BDMA1EN)   == 0U)
#endif

/** @brief  Enable or disable the AHB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_GPIOA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOB_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOBEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOD_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIODEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIODEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOE_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOFEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOJ_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOJEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOJEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_GPIOK_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOKEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOKEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(RCC_AHB4ENR_CRCEN)
#define __HAL_RCC_CRC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_CRCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_CRCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#if defined(BDMA2)
#define __HAL_RCC_BDMA2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_BDMA2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_BDMA2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_BDMA_CLK_ENABLE()  __HAL_RCC_BDMA2_CLK_ENABLE()  /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_BDMAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_BDMAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#if defined(ADC3)
#define __HAL_RCC_ADC3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_ADC3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_ADC3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#if defined(RCC_AHB4ENR_HSEMEN)
#define __HAL_RCC_HSEM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#if defined(RCC_AHB4ENR_SRDSRAMEN)
#define __HAL_RCC_SRDSRAM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_SRDSRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_SRDSRAMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif

#define __HAL_RCC_BKPRAM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_BKPRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_BKPRAMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_GPIOA_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOAEN)
#define __HAL_RCC_GPIOB_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOBEN)
#define __HAL_RCC_GPIOC_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOCEN)
#define __HAL_RCC_GPIOD_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIODEN)
#define __HAL_RCC_GPIOE_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOEEN)
#define __HAL_RCC_GPIOF_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOFEN)
#define __HAL_RCC_GPIOG_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOGEN)
#define __HAL_RCC_GPIOH_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOHEN)
#define __HAL_RCC_GPIOI_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOIEN)
#define __HAL_RCC_GPIOJ_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOJEN)
#define __HAL_RCC_GPIOK_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOKEN)
#if defined(RCC_AHB4ENR_CRCEN)
#define __HAL_RCC_CRC_CLK_DISABLE()             (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_CRCEN)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_CLK_DISABLE()           (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_BDMA2EN)
#define __HAL_RCC_BDMA_CLK_DISABLE()            __HAL_RCC_BDMA2_CLK_DISABLE()  /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_CLK_DISABLE()            (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_BDMAEN)
#endif
#if defined(ADC3)
#define __HAL_RCC_ADC3_CLK_DISABLE()            (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_ADC3EN)
#endif
#if defined(RCC_AHB4ENR_HSEMEN)
#define __HAL_RCC_HSEM_CLK_DISABLE()            (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_HSEMEN)
#endif
#if defined(RCC_AHB4ENR_SRDSRAMEN)
#define __HAL_RCC_SRDSRAM_CLK_DISABLE()         (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_SRDSRAMEN)
#endif
#define __HAL_RCC_BKPRAM_CLK_DISABLE()          (RCC->AHB4ENR) &= ~ (RCC_AHB4ENR_BKPRAMEN)


/** @brief  Get the enable or disable status of the AHB4 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_GPIOA_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOAEN)  != 0U)
#define __HAL_RCC_GPIOB_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOBEN)  != 0U)
#define __HAL_RCC_GPIOC_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOCEN)  != 0U)
#define __HAL_RCC_GPIOD_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIODEN)  != 0U)
#define __HAL_RCC_GPIOE_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOEEN)  != 0U)
#define __HAL_RCC_GPIOF_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOFEN)  != 0U)
#define __HAL_RCC_GPIOG_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOGEN)  != 0U)
#define __HAL_RCC_GPIOH_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOHEN)  != 0U)
#define __HAL_RCC_GPIOI_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOIEN)  != 0U)
#define __HAL_RCC_GPIOJ_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOJEN)  != 0U)
#define __HAL_RCC_GPIOK_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOKEN)  != 0U)
#if defined(RCC_AHB4ENR_CRCEN)
#define __HAL_RCC_CRC_IS_CLK_ENABLED()              ((RCC->AHB4ENR & RCC_AHB4ENR_CRCEN)    != 0U)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_IS_CLK_ENABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_BDMA2EN)   != 0U)
#define __HAL_RCC_BDMA_IS_CLK_ENABLED()  __HAL_RCC_BDMA2_IS_CLK_ENABLED()            /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_IS_CLK_ENABLED()             ((RCC->AHB4ENR & RCC_AHB4ENR_BDMAEN)   != 0U)
#endif
#if defined(ADC3)
#define __HAL_RCC_ADC3_IS_CLK_ENABLED()             ((RCC->AHB4ENR & RCC_AHB4ENR_ADC3EN)   != 0U)
#endif
#if defined(RCC_AHB4ENR_HSEMEN)
#define __HAL_RCC_HSEM_IS_CLK_ENABLED()             ((RCC->AHB4ENR & RCC_AHB4ENR_HSEMEN)   != 0U)
#endif
#if defined(RCC_AHB4ENR_SRDSRAMEN)
#define __HAL_RCC_SRDSRAM_IS_CLK_ENABLED()          ((RCC->AHB4ENR & RCC_AHB4ENR_SRDSRAMEN) != 0U)
#endif
#define __HAL_RCC_BKPRAM_IS_CLK_ENABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_BKPRAMEN) != 0U)

#define __HAL_RCC_GPIOA_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOAEN)  == 0U)
#define __HAL_RCC_GPIOB_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOBEN)  == 0U)
#define __HAL_RCC_GPIOC_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOCEN)  == 0U)
#define __HAL_RCC_GPIOD_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIODEN)  == 0U)
#define __HAL_RCC_GPIOE_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOEEN)  == 0U)
#define __HAL_RCC_GPIOF_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOFEN)  == 0U)
#define __HAL_RCC_GPIOG_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOGEN)  == 0U)
#define __HAL_RCC_GPIOH_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOHEN)  == 0U)
#define __HAL_RCC_GPIOI_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOIEN)  == 0U)
#define __HAL_RCC_GPIOJ_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOJEN)  == 0U)
#define __HAL_RCC_GPIOK_IS_CLK_DISABLED()           ((RCC->AHB4ENR & RCC_AHB4ENR_GPIOKEN)  == 0U)

#if defined(RCC_AHB4ENR_CRCEN)
#define __HAL_RCC_CRC_IS_CLK_DISABLED()             ((RCC->AHB4ENR & RCC_AHB4ENR_CRCEN)    == 0U)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_IS_CLK_DISABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_BDMA2EN)   == 0U)
#define __HAL_RCC_BDMA_IS_CLK_DISABLED()  __HAL_RCC_BDMA2_IS_CLK_DISABLED()           /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_IS_CLK_DISABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_BDMAEN)   == 0U)
#endif
#if defined(ADC3)
#define __HAL_RCC_ADC3_IS_CLK_DISABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_ADC3EN)   == 0U)
#endif
#if defined(RCC_AHB4ENR_HSEMEN)
#define __HAL_RCC_HSEM_IS_CLK_DISABLED()            ((RCC->AHB4ENR & RCC_AHB4ENR_HSEMEN)   == 0U)
#endif
#if defined(RCC_AHB4ENR_SRDSRAMEN)
#define __HAL_RCC_SRDSRAM_IS_CLK_DISABLED()         ((RCC->AHB4ENR & RCC_AHB4ENR_SRDSRAMEN)   == 0U)
#endif
#define __HAL_RCC_BKPRAM_IS_CLK_DISABLED()          ((RCC->AHB4ENR & RCC_AHB4ENR_BKPRAMEN) == 0U)


/** @brief  Enable or disable the APB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#if defined(LTDC)
#define __HAL_RCC_LTDC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB3ENR, RCC_APB3ENR_LTDCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB3ENR, RCC_APB3ENR_LTDCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* LTDC */

#if defined(DSI)
#define __HAL_RCC_DSI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB3ENR, RCC_APB3ENR_DSIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB3ENR, RCC_APB3ENR_DSIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*DSI*/

#define __HAL_RCC_WWDG1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB3ENR, RCC_APB3ENR_WWDG1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB3ENR, RCC_APB3ENR_WWDG1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(LTDC)
#define __HAL_RCC_LTDC_CLK_DISABLE()           (RCC->APB3ENR) &= ~ (RCC_APB3ENR_LTDCEN)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_CLK_DISABLE()            (RCC->APB3ENR) &= ~ (RCC_APB3ENR_DSIEN)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_CLK_DISABLE()          (RCC->APB3ENR) &= ~ (RCC_APB3ENR_WWDG1EN)

/** @brief  Get the enable or disable status of the APB3 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#if defined(LTDC)
#define __HAL_RCC_LTDC_IS_CLK_ENABLED()            ((RCC->APB3ENR & RCC_APB3ENR_LTDCEN)  != 0U)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_IS_CLK_ENABLED()             ((RCC->APB3ENR & RCC_APB3ENR_DSIEN)   != 0U)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_IS_CLK_ENABLED()           ((RCC->APB3ENR & RCC_APB3ENR_WWDG1EN) != 0U)
#if defined(LTDC)
#define __HAL_RCC_LTDC_IS_CLK_DISABLED()           ((RCC->APB3ENR & RCC_APB3ENR_LTDCEN)  == 0U)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_IS_CLK_DISABLED()            ((RCC->APB3ENR & RCC_APB3ENR_DSIEN)   == 0U)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_IS_CLK_DISABLED()          ((RCC->APB3ENR & RCC_APB3ENR_WWDG1EN) == 0U)


/** @brief  Enable or disable the APB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_TIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM7EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM13_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM13EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM13EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM14_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM14EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_TIM14EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_LPTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_LPTIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_LPTIM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_WWDG2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_WWDG2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*DUAL_CORE*/

#define __HAL_RCC_SPI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_SPI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_SPI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SPI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_SPI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_SPI3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SPDIFRX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_SPDIFRXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_SPDIFRXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_USART2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_USART2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_USART2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_USART3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_USART3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_USART3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_UART4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_UART4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_UART4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_UART5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_UART5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_UART5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_I2C1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_I2C1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_I2C1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_I2C2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_I2C2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_I2C2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_I2C3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_I2C3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_I2C3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_CEC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_CECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_CECEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_DAC12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_DAC12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_DAC12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_UART7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_UART7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_UART7EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_UART8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1LENR, RCC_APB1LENR_UART8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_UART8EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_CRS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1HENR, RCC_APB1HENR_CRSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1HENR, RCC_APB1HENR_CRSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SWPMI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1HENR, RCC_APB1HENR_SWPMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1HENR, RCC_APB1HENR_SWPMIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_OPAMP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1HENR, RCC_APB1HENR_OPAMPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1HENR, RCC_APB1HENR_OPAMPEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_MDIOS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1HENR, RCC_APB1HENR_MDIOSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1HENR, RCC_APB1HENR_MDIOSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_FDCAN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1HENR, RCC_APB1HENR_FDCANEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1HENR, RCC_APB1HENR_FDCANEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_TIM2_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM2EN)
#define __HAL_RCC_TIM3_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM3EN)
#define __HAL_RCC_TIM4_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM4EN)
#define __HAL_RCC_TIM5_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM5EN)
#define __HAL_RCC_TIM6_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM6EN)
#define __HAL_RCC_TIM7_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM7EN)
#define __HAL_RCC_TIM12_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM12EN)
#define __HAL_RCC_TIM13_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM13EN)
#define __HAL_RCC_TIM14_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_TIM14EN)
#define __HAL_RCC_LPTIM1_CLK_DISABLE()         (RCC->APB1LENR) &= ~ (RCC_APB1LENR_LPTIM1EN)

#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_WWDG2EN)
#endif /*DUAL_CORE*/

#define __HAL_RCC_SPI2_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_SPI2EN)
#define __HAL_RCC_SPI3_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_SPI3EN)
#define __HAL_RCC_SPDIFRX_CLK_DISABLE()        (RCC->APB1LENR) &= ~ (RCC_APB1LENR_SPDIFRXEN)
#define __HAL_RCC_USART2_CLK_DISABLE()         (RCC->APB1LENR) &= ~ (RCC_APB1LENR_USART2EN)
#define __HAL_RCC_USART3_CLK_DISABLE()         (RCC->APB1LENR) &= ~ (RCC_APB1LENR_USART3EN)
#define __HAL_RCC_UART4_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_UART4EN)
#define __HAL_RCC_UART5_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_UART5EN)
#define __HAL_RCC_I2C1_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_I2C1EN)
#define __HAL_RCC_I2C2_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_I2C2EN)
#define __HAL_RCC_I2C3_CLK_DISABLE()           (RCC->APB1LENR) &= ~ (RCC_APB1LENR_I2C3EN)
#define __HAL_RCC_CEC_CLK_DISABLE()            (RCC->APB1LENR) &= ~ (RCC_APB1LENR_CECEN)
#define __HAL_RCC_DAC12_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_DAC12EN)
#define __HAL_RCC_UART7_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_UART7EN)
#define __HAL_RCC_UART8_CLK_DISABLE()          (RCC->APB1LENR) &= ~ (RCC_APB1LENR_UART8EN)
#define __HAL_RCC_CRS_CLK_DISABLE()            (RCC->APB1HENR) &= ~ (RCC_APB1HENR_CRSEN)
#define __HAL_RCC_SWPMI1_CLK_DISABLE()          (RCC->APB1HENR) &= ~ (RCC_APB1HENR_SWPMIEN)
#define __HAL_RCC_OPAMP_CLK_DISABLE()          (RCC->APB1HENR) &= ~ (RCC_APB1HENR_OPAMPEN)
#define __HAL_RCC_MDIOS_CLK_DISABLE()          (RCC->APB1HENR) &= ~ (RCC_APB1HENR_MDIOSEN)
#define __HAL_RCC_FDCAN_CLK_DISABLE()          (RCC->APB1HENR) &= ~ (RCC_APB1HENR_FDCANEN)


/** @brief  Get the enable or disable status of the APB1 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_TIM2_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_TIM2EN)    != 0U)
#define __HAL_RCC_TIM3_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_TIM3EN)    != 0U)
#define __HAL_RCC_TIM4_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_TIM4EN)    != 0U)
#define __HAL_RCC_TIM5_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_TIM5EN)    != 0U)
#define __HAL_RCC_TIM6_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_TIM6EN)    != 0U)
#define __HAL_RCC_TIM7_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_TIM7EN)    != 0U)
#define __HAL_RCC_TIM12_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM12EN)   != 0U)
#define __HAL_RCC_TIM13_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM13EN)   != 0U)
#define __HAL_RCC_TIM14_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM14EN)   != 0U)
#define __HAL_RCC_LPTIM1_IS_CLK_ENABLED()          ((RCC->APB1LENR & RCC_APB1LENR_LPTIM1EN)  != 0U)
#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_WWDG2EN)   != 0U)
#endif /*DUAL_CORE*/
#define __HAL_RCC_SPI2_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_SPI2EN)    != 0U)
#define __HAL_RCC_SPI3_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_SPI3EN)    != 0U)
#define __HAL_RCC_SPDIFRX_IS_CLK_ENABLED()         ((RCC->APB1LENR & RCC_APB1LENR_SPDIFRXEN) != 0U)
#define __HAL_RCC_USART2_IS_CLK_ENABLED()          ((RCC->APB1LENR & RCC_APB1LENR_USART2EN)  != 0U)
#define __HAL_RCC_USART3_IS_CLK_ENABLED()          ((RCC->APB1LENR & RCC_APB1LENR_USART3EN)  != 0U)
#define __HAL_RCC_UART4_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_UART4EN)   != 0U)
#define __HAL_RCC_UART5_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_UART5EN)   != 0U)
#define __HAL_RCC_I2C1_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_I2C1EN)    != 0U)
#define __HAL_RCC_I2C2_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_I2C2EN)    != 0U)
#define __HAL_RCC_I2C3_IS_CLK_ENABLED()            ((RCC->APB1LENR & RCC_APB1LENR_I2C3EN)    != 0U)
#define __HAL_RCC_CEC_IS_CLK_ENABLED()             ((RCC->APB1LENR & RCC_APB1LENR_CECEN)     != 0U)
#define __HAL_RCC_DAC12_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_DAC12EN)   != 0U)
#define __HAL_RCC_UART7_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_UART7EN)   != 0U)
#define __HAL_RCC_UART8_IS_CLK_ENABLED()           ((RCC->APB1LENR & RCC_APB1LENR_UART8EN)   != 0U)
#define __HAL_RCC_CRS_IS_CLK_ENABLED()             ((RCC->APB1HENR & RCC_APB1HENR_CRSEN)     != 0U)
#define __HAL_RCC_SWPMI1_IS_CLK_ENABLED()          ((RCC->APB1HENR & RCC_APB1HENR_SWPMIEN)   != 0U)
#define __HAL_RCC_OPAMP_IS_CLK_ENABLED()           ((RCC->APB1HENR & RCC_APB1HENR_OPAMPEN)   != 0U)
#define __HAL_RCC_MDIOS_IS_CLK_ENABLED()           ((RCC->APB1HENR & RCC_APB1HENR_MDIOSEN)   != 0U)
#define __HAL_RCC_FDCAN_IS_CLK_ENABLED()           ((RCC->APB1HENR & RCC_APB1HENR_FDCANEN)   != 0U)

#define __HAL_RCC_TIM2_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM2EN)    == 0U)
#define __HAL_RCC_TIM3_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM3EN)    == 0U)
#define __HAL_RCC_TIM4_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM4EN)    == 0U)
#define __HAL_RCC_TIM5_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM5EN)    == 0U)
#define __HAL_RCC_TIM6_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM6EN)    == 0U)
#define __HAL_RCC_TIM7_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_TIM7EN)    == 0U)
#define __HAL_RCC_TIM12_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_TIM12EN)   == 0U)
#define __HAL_RCC_TIM13_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_TIM13EN)   == 0U)
#define __HAL_RCC_TIM14_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_TIM14EN)   == 0U)
#define __HAL_RCC_LPTIM1_IS_CLK_DISABLED()         ((RCC->APB1LENR & RCC_APB1LENR_LPTIM1EN)  == 0U)
#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_WWDG2EN)   == 0U)
#endif /*DUAL_CORE*/
#define __HAL_RCC_SPI2_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_SPI2EN)    == 0U)
#define __HAL_RCC_SPI3_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_SPI3EN)    == 0U)
#define __HAL_RCC_SPDIFRX_IS_CLK_DISABLED()        ((RCC->APB1LENR & RCC_APB1LENR_SPDIFRXEN) == 0U)
#define __HAL_RCC_USART2_IS_CLK_DISABLED()         ((RCC->APB1LENR & RCC_APB1LENR_USART2EN)  == 0U)
#define __HAL_RCC_USART3_IS_CLK_DISABLED()         ((RCC->APB1LENR & RCC_APB1LENR_USART3EN)  == 0U)
#define __HAL_RCC_UART4_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_UART4EN)   == 0U)
#define __HAL_RCC_UART5_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_UART5EN)   == 0U)
#define __HAL_RCC_I2C1_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_I2C1EN)    == 0U)
#define __HAL_RCC_I2C2_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_I2C2EN)    == 0U)
#define __HAL_RCC_I2C3_IS_CLK_DISABLED()           ((RCC->APB1LENR & RCC_APB1LENR_I2C3EN)    == 0U)
#define __HAL_RCC_CEC_IS_CLK_DISABLED()            ((RCC->APB1LENR & RCC_APB1LENR_CECEN)     == 0U)
#define __HAL_RCC_DAC12_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_DAC12EN)   == 0U)
#define __HAL_RCC_UART7_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_UART7EN)   == 0U)
#define __HAL_RCC_UART8_IS_CLK_DISABLED()          ((RCC->APB1LENR & RCC_APB1LENR_UART8EN)   == 0U)
#define __HAL_RCC_CRS_IS_CLK_DISABLED()            ((RCC->APB1HENR & RCC_APB1HENR_CRSEN)     == 0U)
#define __HAL_RCC_SWPMI1_IS_CLK_DISABLED()         ((RCC->APB1HENR & RCC_APB1HENR_SWPMIEN)   == 0U)
#define __HAL_RCC_OPAMP_IS_CLK_DISABLED()          ((RCC->APB1HENR & RCC_APB1HENR_OPAMPEN)   == 0U)
#define __HAL_RCC_MDIOS_IS_CLK_DISABLED()          ((RCC->APB1HENR & RCC_APB1HENR_MDIOSEN)   == 0U)
#define __HAL_RCC_FDCAN_IS_CLK_DISABLED()          ((RCC->APB1HENR & RCC_APB1HENR_FDCANEN)   == 0U)


/** @brief  Enable or disable the APB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_TIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_USART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_USART6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(UART9)
#define __HAL_RCC_UART9_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_UART9EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_UART9EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*UART9*/

#if defined(USART10)
#define __HAL_RCC_USART10_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART10EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART10EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*USART10*/

#define __HAL_RCC_SPI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SPI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM15_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM16_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_TIM17_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SPI5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SAI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SAI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(SAI3)
#define __HAL_RCC_SAI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*SAI3*/

#define __HAL_RCC_DFSDM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_HRTIMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_HRTIMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*HRTIM1*/

#define __HAL_RCC_TIM1_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_TIM1EN)
#define __HAL_RCC_TIM8_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_TIM8EN)
#define __HAL_RCC_USART1_CLK_DISABLE()         (RCC->APB2ENR) &= ~ (RCC_APB2ENR_USART1EN)
#define __HAL_RCC_USART6_CLK_DISABLE()         (RCC->APB2ENR) &= ~ (RCC_APB2ENR_USART6EN)
#if defined(UART9)
#define __HAL_RCC_UART9_CLK_DISABLE()          (RCC->APB2ENR) &= ~ (RCC_APB2ENR_UART9EN)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_CLK_DISABLE()        (RCC->APB2ENR) &= ~ (RCC_APB2ENR_USART10EN)
#endif /*USART10*/
#define __HAL_RCC_SPI1_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_SPI1EN)
#define __HAL_RCC_SPI4_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_SPI4EN)
#define __HAL_RCC_TIM15_CLK_DISABLE()          (RCC->APB2ENR) &= ~ (RCC_APB2ENR_TIM15EN)
#define __HAL_RCC_TIM16_CLK_DISABLE()          (RCC->APB2ENR) &= ~ (RCC_APB2ENR_TIM16EN)
#define __HAL_RCC_TIM17_CLK_DISABLE()          (RCC->APB2ENR) &= ~ (RCC_APB2ENR_TIM17EN)
#define __HAL_RCC_SPI5_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_SPI5EN)
#define __HAL_RCC_SAI1_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_SAI1EN)
#define __HAL_RCC_SAI2_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_SAI2EN)
#if defined(SAI3)
#define __HAL_RCC_SAI3_CLK_DISABLE()           (RCC->APB2ENR) &= ~ (RCC_APB2ENR_SAI3EN)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_CLK_DISABLE()         (RCC->APB2ENR) &= ~ (RCC_APB2ENR_DFSDM1EN)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_CLK_DISABLE()         (RCC->APB2ENR) &= ~ (RCC_APB2ENR_HRTIMEN)
#endif /*HRTIM*/

/** @brief  Get the enable or disable status of the APB2 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_TIM1_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_TIM1EN)   != 0U)
#define __HAL_RCC_TIM8_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_TIM8EN)   != 0U)
#define __HAL_RCC_USART1_IS_CLK_ENABLED()          ((RCC->APB2ENR & RCC_APB2ENR_USART1EN) != 0U)
#define __HAL_RCC_USART6_IS_CLK_ENABLED()          ((RCC->APB2ENR & RCC_APB2ENR_USART6EN) != 0U)
#if defined(UART9)
#define __HAL_RCC_UART9_IS_CLK_ENABLED()           ((RCC->APB2ENR & RCC_APB2ENR_UART9EN) != 0U)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_IS_CLK_ENABLED()         ((RCC->APB2ENR & RCC_APB2ENR_USART10EN) != 0U)
#endif /*USART10*/
#define __HAL_RCC_SPI1_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_SPI1EN)   != 0U)
#define __HAL_RCC_SPI4_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_SPI4EN)   != 0U)
#define __HAL_RCC_TIM15_IS_CLK_ENABLED()           ((RCC->APB2ENR & RCC_APB2ENR_TIM15EN)  != 0U)
#define __HAL_RCC_TIM16_IS_CLK_ENABLED()           ((RCC->APB2ENR & RCC_APB2ENR_TIM16EN)  != 0U)
#define __HAL_RCC_TIM17_IS_CLK_ENABLED()           ((RCC->APB2ENR & RCC_APB2ENR_TIM17EN)  != 0U)
#define __HAL_RCC_SPI5_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_SPI5EN)   != 0U)
#define __HAL_RCC_SAI1_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_SAI1EN)   != 0U)
#define __HAL_RCC_SAI2_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_SAI2EN)   != 0U)
#if defined(SAI3)
#define __HAL_RCC_SAI3_IS_CLK_ENABLED()            ((RCC->APB2ENR & RCC_APB2ENR_SAI3EN)   != 0U)
#endif /* SAI3 */
#define __HAL_RCC_DFSDM1_IS_CLK_ENABLED()          ((RCC->APB2ENR & RCC_APB2ENR_DFSDM1EN) != 0U)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_IS_CLK_ENABLED()          ((RCC->APB2ENR & RCC_APB2ENR_HRTIMEN)  != 0U)
#endif /*HRTIM1*/

#define __HAL_RCC_TIM1_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_TIM1EN)   == 0U)
#define __HAL_RCC_TIM8_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_TIM8EN)   == 0U)
#define __HAL_RCC_USART1_IS_CLK_DISABLED()         ((RCC->APB2ENR & RCC_APB2ENR_USART1EN) == 0U)
#define __HAL_RCC_USART6_IS_CLK_DISABLED()         ((RCC->APB2ENR & RCC_APB2ENR_USART6EN) == 0U)
#if defined(UART9)
#define __HAL_RCC_UART9_IS_CLK_DISABLED()         ((RCC->APB2ENR & RCC_APB2ENR_UART9EN) == 0U)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_IS_CLK_DISABLED()         ((RCC->APB2ENR & RCC_APB2ENR_USART10EN) == 0U)
#endif /*USART10*/
#define __HAL_RCC_SPI1_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_SPI1EN)   == 0U)
#define __HAL_RCC_SPI4_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_SPI4EN)   == 0U)
#define __HAL_RCC_TIM15_IS_CLK_DISABLED()          ((RCC->APB2ENR & RCC_APB2ENR_TIM15EN)  == 0U)
#define __HAL_RCC_TIM16_IS_CLK_DISABLED()          ((RCC->APB2ENR & RCC_APB2ENR_TIM16EN)  == 0U)
#define __HAL_RCC_TIM17_IS_CLK_DISABLED()          ((RCC->APB2ENR & RCC_APB2ENR_TIM17EN)  == 0U)
#define __HAL_RCC_SPI5_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_SPI5EN)   == 0U)
#define __HAL_RCC_SAI1_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_SAI1EN)   == 0U)
#define __HAL_RCC_SAI2_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_SAI2EN)   == 0U)
#if defined(SAI3)
#define __HAL_RCC_SAI3_IS_CLK_DISABLED()           ((RCC->APB2ENR & RCC_APB2ENR_SAI3EN)   == 0U)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_IS_CLK_DISABLED()         ((RCC->APB2ENR & RCC_APB2ENR_DFSDM1EN) == 0U)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_IS_CLK_DISABLED()         ((RCC->APB2ENR & RCC_APB2ENR_HRTIMEN)  == 0U)
#endif /*HRTIM1*/

/** @brief  Enable or disable the APB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_SYSCFG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_LPUART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_LPUART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_LPUART1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_SPI6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_SPI6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_SPI6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_I2C4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_I2C4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_I2C4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_LPTIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_LPTIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* LPTIM4 */

#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_LPTIM5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* LPTIM5 */

#if defined(DAC2)
#define __HAL_RCC_DAC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_DAC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_DAC2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*  DAC2 */

#define __HAL_RCC_COMP12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_COMP12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_COMP12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_VREF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_VREFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_VREFEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(SAI4)
#define __HAL_RCC_SAI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_SAI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_SAI4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* SAI4 */

#define __HAL_RCC_RTC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_RTCAPBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_RTCAPBEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(DTS)
#define __HAL_RCC_DTS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_DTSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_DTSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*DTS*/

#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_DFSDM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_DFSDM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /*DFSDM2*/

#define __HAL_RCC_SYSCFG_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_SYSCFGEN)
#define __HAL_RCC_LPUART1_CLK_DISABLE()          (RCC->APB4ENR) &= ~ (RCC_APB4ENR_LPUART1EN)
#define __HAL_RCC_SPI6_CLK_DISABLE()             (RCC->APB4ENR) &= ~ (RCC_APB4ENR_SPI6EN)
#define __HAL_RCC_I2C4_CLK_DISABLE()             (RCC->APB4ENR) &= ~ (RCC_APB4ENR_I2C4EN)
#define __HAL_RCC_LPTIM2_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM2EN)
#define __HAL_RCC_LPTIM3_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM3EN)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM4EN)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM5EN)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_CLK_DISABLE()             (RCC->APB4ENR) &= ~ (RCC_APB4ENR_DAC2EN)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_COMP12EN)
#define __HAL_RCC_VREF_CLK_DISABLE()             (RCC->APB4ENR) &= ~ (RCC_APB4ENR_VREFEN)
#define __HAL_RCC_RTC_CLK_DISABLE()              (RCC->APB4ENR) &= ~ (RCC_APB4ENR_RTCAPBEN)
#if defined(SAI4)
#define __HAL_RCC_SAI4_CLK_DISABLE()             (RCC->APB4ENR) &= ~ (RCC_APB4ENR_SAI4EN)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_CLK_DISABLE()              (RCC->APB4ENR) &= ~ (RCC_APB4ENR_DTSEN)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_CLK_DISABLE()           (RCC->APB4ENR) &= ~ (RCC_APB4ENR_DFSDM2EN)
#endif /*DFSDM2*/

/** @brief  Get the enable or disable status of the APB4 peripheral clock
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_SYSCFG_IS_CLK_ENABLED()            ((RCC->APB4ENR & RCC_APB4ENR_SYSCFGEN)  != 0U)
#define __HAL_RCC_LPUART1_IS_CLK_ENABLED()           ((RCC->APB4ENR & RCC_APB4ENR_LPUART1EN) != 0U)
#define __HAL_RCC_SPI6_IS_CLK_ENABLED()              ((RCC->APB4ENR & RCC_APB4ENR_SPI6EN)    != 0U)
#define __HAL_RCC_I2C4_IS_CLK_ENABLED()              ((RCC->APB4ENR & RCC_APB4ENR_I2C4EN)    != 0U)
#define __HAL_RCC_LPTIM2_IS_CLK_ENABLED()            ((RCC->APB4ENR & RCC_APB4ENR_LPTIM2EN)  != 0U)
#define __HAL_RCC_LPTIM3_IS_CLK_ENABLED()            ((RCC->APB4ENR & RCC_APB4ENR_LPTIM3EN)  != 0U)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_IS_CLK_ENABLED()            ((RCC->APB4ENR & RCC_APB4ENR_LPTIM4EN)  != 0U)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_IS_CLK_ENABLED()            ((RCC->APB4ENR & RCC_APB4ENR_LPTIM5EN)  != 0U)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_IS_CLK_ENABLED()              ((RCC->APB4ENR & RCC_APB4ENR_DAC2EN)  != 0U)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_IS_CLK_ENABLED()            ((RCC->APB4ENR & RCC_APB4ENR_COMP12EN)  != 0U)
#define __HAL_RCC_VREF_IS_CLK_ENABLED()              ((RCC->APB4ENR & RCC_APB4ENR_VREFEN)    != 0U)
#define __HAL_RCC_RTC_IS_CLK_ENABLED()               ((RCC->APB4ENR & RCC_APB4ENR_RTCAPBEN)  != 0U)
#if defined(SAI4)
#define __HAL_RCC_SAI4_IS_CLK_ENABLED()              ((RCC->APB4ENR & RCC_APB4ENR_SAI4EN)    != 0U)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_IS_CLK_ENABLED()               ((RCC->APB4ENR & RCC_APB4ENR_DTSEN)    != 0U)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_IS_CLK_ENABLED()              ((RCC->APB4ENR & RCC_APB4ENR_DFSDM2EN)    != 0U)
#endif /*DFSDM2*/

#define __HAL_RCC_SYSCFG_IS_CLK_DISABLED()           ((RCC->APB4ENR & RCC_APB4ENR_SYSCFGEN)  == 0U)
#define __HAL_RCC_LPUART1_IS_CLK_DISABLED()          ((RCC->APB4ENR & RCC_APB4ENR_LPUART1EN) == 0U)
#define __HAL_RCC_SPI6_IS_CLK_DISABLED()             ((RCC->APB4ENR & RCC_APB4ENR_SPI6EN)    == 0U)
#define __HAL_RCC_I2C4_IS_CLK_DISABLED()             ((RCC->APB4ENR & RCC_APB4ENR_I2C4EN)    == 0U)
#define __HAL_RCC_LPTIM2_IS_CLK_DISABLED()           ((RCC->APB4ENR & RCC_APB4ENR_LPTIM2EN)  == 0U)
#define __HAL_RCC_LPTIM3_IS_CLK_DISABLED()           ((RCC->APB4ENR & RCC_APB4ENR_LPTIM3EN)  == 0U)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_IS_CLK_DISABLED()           ((RCC->APB4ENR & RCC_APB4ENR_LPTIM4EN)  == 0U)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_IS_CLK_DISABLED()           ((RCC->APB4ENR & RCC_APB4ENR_LPTIM5EN)  == 0U)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_IS_CLK_DISABLED()             ((RCC->APB4ENR & RCC_APB4ENR_DAC2EN)  == 0U)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_IS_CLK_DISABLED()           ((RCC->APB4ENR & RCC_APB4ENR_COMP12EN)  == 0U)
#define __HAL_RCC_VREF_IS_CLK_DISABLED()             ((RCC->APB4ENR & RCC_APB4ENR_VREFEN)    == 0U)
#define __HAL_RCC_RTC_IS_CLK_DISABLED()              ((RCC->APB4ENR & RCC_APB4ENR_RTCAPBEN)  == 0U)
#if defined(SAI4)
#define __HAL_RCC_SAI4_IS_CLK_DISABLED()             ((RCC->APB4ENR & RCC_APB4ENR_SAI4EN)    == 0U)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_IS_CLK_DISABLED()              ((RCC->APB4ENR & RCC_APB4ENR_DTSEN)    == 0U)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_IS_CLK_DISABLED()          ((RCC->APB4ENR & RCC_APB4ENR_DFSDM2EN)    == 0U)
#endif /*DFSDM2*/

#if defined(DUAL_CORE)

/* Exported macros for RCC_C1 -------------------------------------------------*/

/** @brief  Enable or disable the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_MDMA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_MDMAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_MDMAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DMA2D_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_DMA2DEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_DMA2DEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_JPGDECEN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_JPGDECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_JPGDECEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C1_FMC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_QSPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SDMMC1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_SDMMC1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB3ENR, RCC_AHB3ENR_SDMMC1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)




#define __HAL_RCC_C1_MDMA_CLK_DISABLE()            (RCC_C1->AHB3ENR &= ~ (RCC_AHB3ENR_MDMAEN))
#define __HAL_RCC_C1_DMA2D_CLK_DISABLE()           (RCC_C1->AHB3ENR &= ~ (RCC_AHB3ENR_DMA2DEN))
#define __HAL_RCC_C1_JPGDECEN_CLK_DISABLE()        (RCC_C1->AHB3ENR &= ~ (RCC_AHB3ENR_JPGDECEN))
#define __HAL_RCC_C1_FMC_CLK_DISABLE()             (RCC_C1->AHB3ENR &= ~ (RCC_AHB3ENR_FMCEN))
#define __HAL_RCC_C1_QSPI_CLK_DISABLE()            (RCC_C1->AHB3ENR &= ~ (RCC_AHB3ENR_QSPIEN))
#define __HAL_RCC_C1_SDMMC1_CLK_DISABLE()          (RCC_C1->AHB3ENR &= ~ (RCC_AHB3ENR_SDMMC1EN))




/** @brief  Enable or disable the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_DMA1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DMA2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_ADC12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ADC12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ADC12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_ART_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ARTEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ARTEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_ETH1MAC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ETH1MACEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ETH1MACEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_ETH1TX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ETH1TXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ETH1TXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_ETH1RX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ETH1RXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_ETH1RXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C1_USB1_OTG_HS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USB1_OTG_HS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USB2_OTG_FS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USB2_OTG_FS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DMA1_CLK_DISABLE()             (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_DMA1EN))
#define __HAL_RCC_C1_DMA2_CLK_DISABLE()             (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_DMA2EN))
#define __HAL_RCC_C1_ADC12_CLK_DISABLE()            (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_ADC12EN))
#define __HAL_RCC_C1_ART_CLK_DISABLE()              (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_ARTEN))
#define __HAL_RCC_C1_ETH1MAC_CLK_DISABLE()          (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1MACEN))
#define __HAL_RCC_C1_ETH1TX_CLK_DISABLE()           (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1TXEN))
#define __HAL_RCC_C1_ETH1RX_CLK_DISABLE()           (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1RXEN))
#define __HAL_RCC_C1_USB1_OTG_HS_CLK_DISABLE()      (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_USB1OTGHSEN))
#define __HAL_RCC_C1_USB1_OTG_HS_ULPI_CLK_DISABLE() (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_USB1OTGHSULPIEN))
#define __HAL_RCC_C1_USB2_OTG_FS_CLK_DISABLE()      (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_USB2OTGHSEN))
#define __HAL_RCC_C1_USB2_OTG_FS_ULPI_CLK_DISABLE() (RCC_C1->AHB1ENR &= ~ (RCC_AHB1ENR_USB2OTGHSULPIEN))

/** @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_DCMI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#if defined(CRYP)
#define __HAL_RCC_C1_CRYP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* CRYP */

#if defined(HASH)
#define __HAL_RCC_C1_HASH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* HASH */

#define __HAL_RCC_C1_RNG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SDMMC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_SDMMC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_SDMMC2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_D2SRAM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_D2SRAM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_D2SRAM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_D2SRAM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_D2SRAM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_D2SRAM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_D2SRAM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_D2SRAM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB2ENR, RCC_AHB2ENR_D2SRAM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DCMI_CLK_DISABLE()             (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_DCMIEN))
#if defined(CRYP)
#define __HAL_RCC_C1_CRYP_CLK_DISABLE()             (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_CRYPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_C1_HASH_CLK_DISABLE()             (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_HASHEN))
#endif /* HASH */
#define __HAL_RCC_C1_RNG_CLK_DISABLE()              (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_RNGEN))
#define __HAL_RCC_C1_SDMMC2_CLK_DISABLE()           (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_SDMMC2EN))
#define __HAL_RCC_C1_D2SRAM1_CLK_DISABLE()          (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM1EN))
#define __HAL_RCC_C1_D2SRAM2_CLK_DISABLE()          (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM2EN))
#define __HAL_RCC_C1_D2SRAM3_CLK_DISABLE()          (RCC_C1->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM3EN))

/** @brief  Enable or disable the AHB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_GPIOA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOB_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOBEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOD_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIODEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIODEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOE_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOEEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOEEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOFEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOJ_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOJEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOJEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_GPIOK_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOKEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_GPIOKEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_CRC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_CRCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_CRCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_BDMA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_BDMAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_BDMAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_ADC3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_ADC3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_ADC3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_HSEM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_BKPRAM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_BKPRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->AHB4ENR, RCC_AHB4ENR_BKPRAMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C1_GPIOA_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOAEN)
#define __HAL_RCC_C1_GPIOB_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOBEN)
#define __HAL_RCC_C1_GPIOC_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOCEN)
#define __HAL_RCC_C1_GPIOD_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIODEN)
#define __HAL_RCC_C1_GPIOE_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOEEN)
#define __HAL_RCC_C1_GPIOF_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOFEN)
#define __HAL_RCC_C1_GPIOG_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOGEN)
#define __HAL_RCC_C1_GPIOH_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOHEN)
#define __HAL_RCC_C1_GPIOI_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOIEN)
#define __HAL_RCC_C1_GPIOJ_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOJEN)
#define __HAL_RCC_C1_GPIOK_CLK_DISABLE()           (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOKEN)
#define __HAL_RCC_C1_CRC_CLK_DISABLE()             (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_CRCEN)
#define __HAL_RCC_C1_BDMA_CLK_DISABLE()            (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_BDMAEN)
#define __HAL_RCC_C1_ADC3_CLK_DISABLE()            (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_ADC3EN)
#define __HAL_RCC_C1_HSEM_CLK_DISABLE()            (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_HSEMEN)
#define __HAL_RCC_C1_BKPRAM_CLK_DISABLE()          (RCC_C1->AHB4ENR) &= ~ (RCC_AHB4ENR_BKPRAMEN)


/** @brief  Enable or disable the APB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_LTDC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB3ENR, RCC_APB3ENR_LTDCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB3ENR, RCC_APB3ENR_LTDCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DSI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB3ENR, RCC_APB3ENR_DSIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB3ENR, RCC_APB3ENR_DSIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_WWDG1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB3ENR, RCC_APB3ENR_WWDG1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB3ENR, RCC_APB3ENR_WWDG1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LTDC_CLK_DISABLE()           (RCC_C1->APB3ENR) &= ~ (RCC_APB3ENR_LTDCEN)
#define __HAL_RCC_C1_DSI_CLK_DISABLE()            (RCC_C1->APB3ENR) &= ~ (RCC_APB3ENR_DSIEN)
#define __HAL_RCC_C1_WWDG1_CLK_DISABLE()          (RCC_C1->APB3ENR) &= ~ (RCC_APB3ENR_WWDG1EN)

/** @brief  Enable or disable the APB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_TIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM7EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM13_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM13EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM13EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM14_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM14EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_TIM14EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LPTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_LPTIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_LPTIM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_WWDG2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_WWDG2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_WWDG2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_SPI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_SPI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_SPI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_SPI3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPDIFRX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_SPDIFRXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_SPDIFRXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USART2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_USART2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_USART2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USART3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_USART3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_USART3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_UART4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_UART5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_I2C1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_I2C1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_I2C1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_I2C2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_I2C2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_I2C2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_I2C3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_I2C3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_I2C3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_CEC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_CECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_CECEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DAC12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_DAC12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_DAC12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_UART7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART7EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_UART8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1LENR, RCC_APB1LENR_UART8EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_CRS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_CRSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_CRSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SWPMI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_SWPMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_SWPMIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_OPAMP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_OPAMPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_OPAMPEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_MDIOS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_MDIOSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_MDIOSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_FDCAN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_FDCANEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB1HENR, RCC_APB1HENR_FDCANEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C1_TIM2_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM2EN)
#define __HAL_RCC_C1_TIM3_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM3EN)
#define __HAL_RCC_C1_TIM4_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM4EN)
#define __HAL_RCC_C1_TIM5_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM5EN)
#define __HAL_RCC_C1_TIM6_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM6EN)
#define __HAL_RCC_C1_TIM7_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM7EN)
#define __HAL_RCC_C1_TIM12_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM12EN)
#define __HAL_RCC_C1_TIM13_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM13EN)
#define __HAL_RCC_C1_TIM14_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_TIM14EN)
#define __HAL_RCC_C1_LPTIM1_CLK_DISABLE()         (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_LPTIM1EN)
#define __HAL_RCC_C1_WWDG2_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_WWDG2EN)
#define __HAL_RCC_C1_SPI2_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_SPI2EN)
#define __HAL_RCC_C1_SPI3_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_SPI3EN)
#define __HAL_RCC_C1_SPDIFRX_CLK_DISABLE()        (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_SPDIFRXEN)
#define __HAL_RCC_C1_USART2_CLK_DISABLE()         (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_USART2EN)
#define __HAL_RCC_C1_USART3_CLK_DISABLE()         (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_USART3EN)
#define __HAL_RCC_C1_UART4_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_UART4EN)
#define __HAL_RCC_C1_UART5_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_UART5EN)
#define __HAL_RCC_C1_I2C1_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_I2C1EN)
#define __HAL_RCC_C1_I2C2_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_I2C2EN)
#define __HAL_RCC_C1_I2C3_CLK_DISABLE()           (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_I2C3EN)
#define __HAL_RCC_C1_CEC_CLK_DISABLE()            (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_CECEN)
#define __HAL_RCC_C1_DAC12_CLK_DISABLE()          (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_DAC12EN)
#define __HAL_RCC_C1_UART7_CLK_DISABLE()         (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_UART7EN)
#define __HAL_RCC_C1_UART8_CLK_DISABLE()         (RCC_C1->APB1LENR) &= ~ (RCC_APB1LENR_UART8EN)
#define __HAL_RCC_C1_CRS_CLK_DISABLE()            (RCC_C1->APB1HENR) &= ~ (RCC_APB1HENR_CRSEN)
#define __HAL_RCC_C1_SWPMI_CLK_DISABLE()          (RCC_C1->APB1HENR) &= ~ (RCC_APB1HENR_SWPMIEN)
#define __HAL_RCC_C1_OPAMP_CLK_DISABLE()          (RCC_C1->APB1HENR) &= ~ (RCC_APB1HENR_OPAMPEN)
#define __HAL_RCC_C1_MDIOS_CLK_DISABLE()          (RCC_C1->APB1HENR) &= ~ (RCC_APB1HENR_MDIOSEN)
#define __HAL_RCC_C1_FDCAN_CLK_DISABLE()          (RCC_C1->APB1HENR) &= ~ (RCC_APB1HENR_FDCANEN)

/** @brief  Enable or disable the APB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_TIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_USART6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM15_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM15EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM15EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM16_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM16EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM16EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM17_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM17EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_TIM17EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPI5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SAI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SAI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SAI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SAI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_SAI3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_DFSDM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_HRTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_HRTIMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB2ENR, RCC_APB2ENR_HRTIMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_TIM1_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_TIM1EN)
#define __HAL_RCC_C1_TIM8_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_TIM8EN)
#define __HAL_RCC_C1_USART1_CLK_DISABLE()         (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_USART1EN)
#define __HAL_RCC_C1_USART6_CLK_DISABLE()         (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_USART6EN)
#define __HAL_RCC_C1_SPI1_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_SPI1EN)
#define __HAL_RCC_C1_SPI4_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_SPI4EN)
#define __HAL_RCC_C1_TIM15_CLK_DISABLE()          (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_TIM15EN)
#define __HAL_RCC_C1_TIM16_CLK_DISABLE()          (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_TIM16EN)
#define __HAL_RCC_C1_TIM17_CLK_DISABLE()          (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_TIM17EN)
#define __HAL_RCC_C1_SPI5_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_SPI5EN)
#define __HAL_RCC_C1_SAI1_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_SAI1EN)
#define __HAL_RCC_C1_SAI2_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_SAI2EN)
#define __HAL_RCC_C1_SAI3_CLK_DISABLE()           (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_SAI3EN)
#define __HAL_RCC_C1_DFSDM1_CLK_DISABLE()         (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_DFSDM1EN)
#define __HAL_RCC_C1_HRTIM1_CLK_DISABLE()         (RCC_C1->APB2ENR) &= ~ (RCC_APB2ENR_HRTIMEN)

/** @brief  Enable or disable the APB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C1_SYSCFG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_SYSCFGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_SYSCFGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LPUART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPUART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPUART1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SPI6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_SPI6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_SPI6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_I2C4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_I2C4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_I2C4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LPTIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LPTIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LPTIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_LPTIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_LPTIM5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_COMP12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_COMP12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_COMP12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C1_VREF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_VREFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_VREFEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_RTC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_RTCAPBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_RTCAPBEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C1_SAI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_SAI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C1->APB4ENR, RCC_APB4ENR_SAI4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C1_SYSCFG_CLK_DISABLE()           (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_SYSCFGEN)
#define __HAL_RCC_C1_LPUART1_CLK_DISABLE()          (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_LPUART1EN)
#define __HAL_RCC_C1_SPI6_CLK_DISABLE()             (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_SPI6EN)
#define __HAL_RCC_C1_I2C4_CLK_DISABLE()             (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_I2C4EN)
#define __HAL_RCC_C1_LPTIM2_CLK_DISABLE()           (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM2EN)
#define __HAL_RCC_C1_LPTIM3_CLK_DISABLE()           (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM3EN)
#define __HAL_RCC_C1_LPTIM4_CLK_DISABLE()           (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM4EN)
#define __HAL_RCC_C1_LPTIM5_CLK_DISABLE()           (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM5EN)
#define __HAL_RCC_C1_COMP12_CLK_DISABLE()           (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_COMP12EN)
#define __HAL_RCC_C1_VREF_CLK_DISABLE()             (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_VREFEN)
#define __HAL_RCC_C1_RTC_CLK_DISABLE()              (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_RTCAPBEN)
#define __HAL_RCC_C1_SAI4_CLK_DISABLE()             (RCC_C1->APB4ENR) &= ~ (RCC_APB4ENR_SAI4EN)

/* Exported macros for RCC_C2 -------------------------------------------------*/

/** @brief  Enable or disable the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */


#define __HAL_RCC_C2_MDMA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_MDMAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_MDMAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_DMA2D_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_DMA2DEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_DMA2DEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_JPGDECEN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_JPGDECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_JPGDECEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_FLASH_C2_ALLOCATE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_FLASHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_FLASHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_DTCM1_C2_ALLOCATE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_DTCM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_DTCM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_DTCM2_C2_ALLOCATE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_DTCM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_DTCM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_ITCM_C2_ALLOCATE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_ITCMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_ITCMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_D1SRAM1_C2_ALLOCATE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_AXISRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_AXISRAMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_FMC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_QSPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SDMMC1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_SDMMC1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB3ENR, RCC_AHB3ENR_SDMMC1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)




#define __HAL_RCC_C2_MDMA_CLK_DISABLE()            (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_MDMAEN))
#define __HAL_RCC_C2_DMA2D_CLK_DISABLE()           (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_DMA2DEN))
#define __HAL_RCC_C2_JPGDECEN_CLK_DISABLE()        (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_JPGDECEN))
#define __HAL_RCC_C2_FMC_CLK_DISABLE()             (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_FMCEN))
#define __HAL_RCC_C2_QSPI_CLK_DISABLE()            (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_QSPIEN))
#define __HAL_RCC_C2_SDMMC1_CLK_DISABLE()          (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_SDMMC1EN))
#define __HAL_RCC_FLASH_C2_DEALLOCATE()            (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_FLASHEN))
#define __HAL_RCC_DTCM1_C2_DEALLOCATE()            (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_DTCM1EN))
#define __HAL_RCC_DTCM2_C2_DEALLOCATE()            (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_DTCM2EN))
#define __HAL_RCC_ITCM_C2_DEALLOCATE()             (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_ITCMEN))
#define __HAL_RCC_D1SRAM1_C2_DEALLOCATE()          (RCC_C2->AHB3ENR &= ~ (RCC_AHB3ENR_AXISRAMEN))

/** @brief  Enable or disable the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_DMA1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_DMA2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_ADC12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ADC12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ADC12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_ART_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ARTEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ARTEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_ETH1MAC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ETH1MACEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ETH1MACEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_ETH1TX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ETH1TXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ETH1TXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_ETH1RX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ETH1RXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_ETH1RXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USB1_OTG_HS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USB1_OTG_HS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USB2_OTG_FS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USB2_OTG_FS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C2_DMA1_CLK_DISABLE()             (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_DMA1EN))
#define __HAL_RCC_C2_DMA2_CLK_DISABLE()             (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_DMA2EN))
#define __HAL_RCC_C2_ADC12_CLK_DISABLE()            (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_ADC12EN))
#define __HAL_RCC_C2_ART_CLK_DISABLE()              (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_ARTEN))
#define __HAL_RCC_C2_ETH1MAC_CLK_DISABLE()          (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1MACEN))
#define __HAL_RCC_C2_ETH1TX_CLK_DISABLE()           (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1TXEN))
#define __HAL_RCC_C2_ETH1RX_CLK_DISABLE()           (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_ETH1RXEN))
#define __HAL_RCC_C2_USB1_OTG_HS_CLK_DISABLE()      (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_USB1OTGHSEN))
#define __HAL_RCC_C2_USB1_OTG_HS_ULPI_CLK_DISABLE() (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_USB1OTGHSULPIEN))
#define __HAL_RCC_C2_USB2_OTG_FS_CLK_DISABLE()      (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_USB2OTGHSEN))
#define __HAL_RCC_C2_USB2_OTG_FS_ULPI_CLK_DISABLE() (RCC_C2->AHB1ENR &= ~ (RCC_AHB1ENR_USB2OTGHSULPIEN))

/** @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_DCMI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#if defined(CRYP)
#define __HAL_RCC_C2_CRYP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* CRYP */

#if defined(HASH)
#define __HAL_RCC_C2_HASH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)
#endif /* HASH */

#define __HAL_RCC_C2_RNG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SDMMC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_SDMMC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_SDMMC2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_D2SRAM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_D2SRAM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_D2SRAM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_D2SRAM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_D2SRAM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_D2SRAM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_D2SRAM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_D2SRAM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB2ENR, RCC_AHB2ENR_D2SRAM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_DCMI_CLK_DISABLE()             (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_DCMIEN))
#if defined(CRYP)
#define __HAL_RCC_C2_CRYP_CLK_DISABLE()             (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_CRYPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_C2_HASH_CLK_DISABLE()             (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_HASHEN))
#endif /* HASH */
#define __HAL_RCC_C2_RNG_CLK_DISABLE()              (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_RNGEN))
#define __HAL_RCC_C2_SDMMC2_CLK_DISABLE()           (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_SDMMC2EN))
#define __HAL_RCC_C2_D2SRAM1_CLK_DISABLE()          (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM1EN))
#define __HAL_RCC_C2_D2SRAM2_CLK_DISABLE()          (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM2EN))
#define __HAL_RCC_C2_D2SRAM3_CLK_DISABLE()          (RCC_C2->AHB2ENR &= ~ (RCC_AHB2ENR_D2SRAM3EN))

/** @brief  Enable or disable the AHB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_GPIOA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOB_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOBEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOD_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIODEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIODEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOE_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOEEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOEEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOFEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOHEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOJ_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOJEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOJEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_GPIOK_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOKEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_GPIOKEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_CRC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_CRCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_CRCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_BDMA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_BDMAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_BDMAEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_ADC3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_ADC3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_ADC3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_HSEM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_BKPRAM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_BKPRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->AHB4ENR, RCC_AHB4ENR_BKPRAMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C2_GPIOA_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOAEN)
#define __HAL_RCC_C2_GPIOB_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOBEN)
#define __HAL_RCC_C2_GPIOC_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOCEN)
#define __HAL_RCC_C2_GPIOD_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIODEN)
#define __HAL_RCC_C2_GPIOE_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOEEN)
#define __HAL_RCC_C2_GPIOF_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOFEN)
#define __HAL_RCC_C2_GPIOG_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOGEN)
#define __HAL_RCC_C2_GPIOH_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOHEN)
#define __HAL_RCC_C2_GPIOI_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOIEN)
#define __HAL_RCC_C2_GPIOJ_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOJEN)
#define __HAL_RCC_C2_GPIOK_CLK_DISABLE()           (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_GPIOKEN)
#define __HAL_RCC_C2_CRC_CLK_DISABLE()             (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_CRCEN)
#define __HAL_RCC_C2_BDMA_CLK_DISABLE()            (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_BDMAEN)
#define __HAL_RCC_C2_ADC3_CLK_DISABLE()            (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_ADC3EN)
#define __HAL_RCC_C2_HSEM_CLK_DISABLE()            (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_HSEMEN)
#define __HAL_RCC_C2_BKPRAM_CLK_DISABLE()          (RCC_C2->AHB4ENR) &= ~ (RCC_AHB4ENR_BKPRAMEN)


/** @brief  Enable or disable the APB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_LTDC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB3ENR, RCC_APB3ENR_LTDCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB3ENR, RCC_APB3ENR_LTDCEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_DSI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB3ENR, RCC_APB3ENR_DSIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB3ENR, RCC_APB3ENR_DSIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_WWDG1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB3ENR, RCC_APB3ENR_WWDG1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB3ENR, RCC_APB3ENR_WWDG1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LTDC_CLK_DISABLE()           (RCC_C2->APB3ENR) &= ~ (RCC_APB3ENR_LTDCEN)
#define __HAL_RCC_C2_DSI_CLK_DISABLE()            (RCC_C2->APB3ENR) &= ~ (RCC_APB3ENR_DSIEN)
#define __HAL_RCC_C2_WWDG1_CLK_DISABLE()          (RCC_C2->APB3ENR) &= ~ (RCC_APB3ENR_WWDG1EN)

/** @brief  Enable or disable the APB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_TIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM7EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM13_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM13EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM13EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM14_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM14EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_TIM14EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LPTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_LPTIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_LPTIM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_WWDG2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_WWDG2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_WWDG2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_SPI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_SPI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_SPI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_SPI3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPDIFRX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_SPDIFRXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_SPDIFRXEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USART2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_USART2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_USART2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USART3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_USART3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_USART3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_UART4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_UART5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_I2C1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_I2C1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_I2C1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_I2C2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_I2C2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_I2C2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_I2C3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_I2C3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_I2C3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_CEC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_CECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_CECEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_DAC12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_DAC12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_DAC12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_UART7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART7EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_UART8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1LENR, RCC_APB1LENR_UART8EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_CRS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_CRSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_CRSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SWPMI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_SWPMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_SWPMIEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_OPAMP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_OPAMPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_OPAMPEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_MDIOS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_MDIOSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_MDIOSEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_FDCAN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_FDCANEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB1HENR, RCC_APB1HENR_FDCANEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)


#define __HAL_RCC_C2_TIM2_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM2EN)
#define __HAL_RCC_C2_TIM3_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM3EN)
#define __HAL_RCC_C2_TIM4_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM4EN)
#define __HAL_RCC_C2_TIM5_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM5EN)
#define __HAL_RCC_C2_TIM6_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM6EN)
#define __HAL_RCC_C2_TIM7_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM7EN)
#define __HAL_RCC_C2_TIM12_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM12EN)
#define __HAL_RCC_C2_TIM13_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM13EN)
#define __HAL_RCC_C2_TIM14_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_TIM14EN)
#define __HAL_RCC_C2_LPTIM1_CLK_DISABLE()         (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_LPTIM1EN)
#define __HAL_RCC_C2_WWDG2_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_WWDG2EN)
#define __HAL_RCC_C2_SPI2_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_SPI2EN)
#define __HAL_RCC_C2_SPI3_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_SPI3EN)
#define __HAL_RCC_C2_SPDIFRX_CLK_DISABLE()        (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_SPDIFRXEN)
#define __HAL_RCC_C2_USART2_CLK_DISABLE()         (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_USART2EN)
#define __HAL_RCC_C2_USART3_CLK_DISABLE()         (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_USART3EN)
#define __HAL_RCC_C2_UART4_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_UART4EN)
#define __HAL_RCC_C2_UART5_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_UART5EN)
#define __HAL_RCC_C2_I2C1_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_I2C1EN)
#define __HAL_RCC_C2_I2C2_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_I2C2EN)
#define __HAL_RCC_C2_I2C3_CLK_DISABLE()           (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_I2C3EN)
#define __HAL_RCC_C2_CEC_CLK_DISABLE()            (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_CECEN)
#define __HAL_RCC_C2_DAC12_CLK_DISABLE()          (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_DAC12EN)
#define __HAL_RCC_C2_UART7_CLK_DISABLE()         (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_UART7EN)
#define __HAL_RCC_C2_UART8_CLK_DISABLE()         (RCC_C2->APB1LENR) &= ~ (RCC_APB1LENR_UART8EN)
#define __HAL_RCC_C2_CRS_CLK_DISABLE()            (RCC_C2->APB1HENR) &= ~ (RCC_APB1HENR_CRSEN)
#define __HAL_RCC_C2_SWPMI_CLK_DISABLE()          (RCC_C2->APB1HENR) &= ~ (RCC_APB1HENR_SWPMIEN)
#define __HAL_RCC_C2_OPAMP_CLK_DISABLE()          (RCC_C2->APB1HENR) &= ~ (RCC_APB1HENR_OPAMPEN)
#define __HAL_RCC_C2_MDIOS_CLK_DISABLE()          (RCC_C2->APB1HENR) &= ~ (RCC_APB1HENR_MDIOSEN)
#define __HAL_RCC_C2_FDCAN_CLK_DISABLE()          (RCC_C2->APB1HENR) &= ~ (RCC_APB1HENR_FDCANEN)

/** @brief  Enable or disable the APB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_TIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_USART6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM15_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM15EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM15EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM16_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM16EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM16EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM17_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM17EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_TIM17EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPI5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SAI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SAI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SAI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SAI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_SAI3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_DFSDM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_HRTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_HRTIMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB2ENR, RCC_APB2ENR_HRTIMEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_TIM1_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_TIM1EN)
#define __HAL_RCC_C2_TIM8_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_TIM8EN)
#define __HAL_RCC_C2_USART1_CLK_DISABLE()         (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_USART1EN)
#define __HAL_RCC_C2_USART6_CLK_DISABLE()         (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_USART6EN)
#define __HAL_RCC_C2_SPI1_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_SPI1EN)
#define __HAL_RCC_C2_SPI4_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_SPI4EN)
#define __HAL_RCC_C2_TIM15_CLK_DISABLE()          (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_TIM15EN)
#define __HAL_RCC_C2_TIM16_CLK_DISABLE()          (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_TIM16EN)
#define __HAL_RCC_C2_TIM17_CLK_DISABLE()          (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_TIM17EN)
#define __HAL_RCC_C2_SPI5_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_SPI5EN)
#define __HAL_RCC_C2_SAI1_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_SAI1EN)
#define __HAL_RCC_C2_SAI2_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_SAI2EN)
#define __HAL_RCC_C2_SAI3_CLK_DISABLE()           (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_SAI3EN)
#define __HAL_RCC_C2_DFSDM1_CLK_DISABLE()         (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_DFSDM1EN)
#define __HAL_RCC_C2_HRTIM1_CLK_DISABLE()         (RCC_C2->APB2ENR) &= ~ (RCC_APB2ENR_HRTIMEN)

/** @brief  Enable or disable the APB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */

#define __HAL_RCC_C2_SYSCFG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_SYSCFGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_SYSCFGEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LPUART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPUART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPUART1EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SPI6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_SPI6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_SPI6EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_I2C4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_I2C4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_I2C4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LPTIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM2EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LPTIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM3EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LPTIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_LPTIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_LPTIM5EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_COMP12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_COMP12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_COMP12EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_VREF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_VREFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_VREFEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_RTC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_RTCAPBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_RTCAPBEN);\
                                        UNUSED(tmpreg); \
                                       } while(0)

#define __HAL_RCC_C2_SAI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_SAI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC_C2->APB4ENR, RCC_APB4ENR_SAI4EN);\
                                        UNUSED(tmpreg); \
                                       } while(0)



#define __HAL_RCC_C2_SYSCFG_CLK_DISABLE()           (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_SYSCFGEN)
#define __HAL_RCC_C2_LPUART1_CLK_DISABLE()          (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_LPUART1EN)
#define __HAL_RCC_C2_SPI6_CLK_DISABLE()             (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_SPI6EN)
#define __HAL_RCC_C2_I2C4_CLK_DISABLE()             (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_I2C4EN)
#define __HAL_RCC_C2_LPTIM2_CLK_DISABLE()           (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM2EN)
#define __HAL_RCC_C2_LPTIM3_CLK_DISABLE()           (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM3EN)
#define __HAL_RCC_C2_LPTIM4_CLK_DISABLE()           (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM4EN)
#define __HAL_RCC_C2_LPTIM5_CLK_DISABLE()           (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_LPTIM5EN)
#define __HAL_RCC_C2_COMP12_CLK_DISABLE()           (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_COMP12EN)
#define __HAL_RCC_C2_VREF_CLK_DISABLE()             (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_VREFEN)
#define __HAL_RCC_C2_RTC_CLK_DISABLE()              (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_RTCAPBEN)
#define __HAL_RCC_C2_SAI4_CLK_DISABLE()             (RCC_C2->APB4ENR) &= ~ (RCC_APB4ENR_SAI4EN)

#endif /*DUAL_CORE*/

/** @brief  Enable or disable the AHB3 peripheral reset.
  */

#define __HAL_RCC_AHB3_FORCE_RESET()          (RCC->AHB3RSTR = 0x7FFFFFFFU)
#define __HAL_RCC_MDMA_FORCE_RESET()          (RCC->AHB3RSTR |= (RCC_AHB3RSTR_MDMARST))
#define __HAL_RCC_DMA2D_FORCE_RESET()         (RCC->AHB3RSTR |= (RCC_AHB3RSTR_DMA2DRST))
#if defined(JPEG)
#define __HAL_RCC_JPGDECRST_FORCE_RESET()     (RCC->AHB3RSTR |= (RCC_AHB3RSTR_JPGDECRST))
#endif /* JPEG */
#define __HAL_RCC_FMC_FORCE_RESET()           (RCC->AHB3RSTR |= (RCC_AHB3RSTR_FMCRST))
#if defined(QUADSPI)
#define __HAL_RCC_QSPI_FORCE_RESET()          (RCC->AHB3RSTR |= (RCC_AHB3RSTR_QSPIRST))
#endif /*QUADSPI*/
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_FORCE_RESET()         (RCC->AHB3RSTR |= (RCC_AHB3RSTR_OSPI1RST))
#endif /*OCTOSPI1*/
#define __HAL_RCC_SDMMC1_FORCE_RESET()        (RCC->AHB3RSTR |= (RCC_AHB3RSTR_SDMMC1RST))
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_FORCE_RESET()         (RCC->AHB3RSTR |= (RCC_AHB3RSTR_OSPI2RST))
#endif /*OCTOSPI2*/
#if defined(OCTOSPIM)
#define __HAL_RCC_IOMNGR_FORCE_RESET()      (RCC->AHB3RSTR |= (RCC_AHB3RSTR_IOMNGRRST))
#endif /*OCTOSPIM*/
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_FORCE_RESET()         (RCC->AHB3RSTR |= (RCC_AHB3RSTR_OTFDEC1RST))
#endif /*OTFDEC1*/
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_FORCE_RESET()         (RCC->AHB3RSTR |= (RCC_AHB3RSTR_OTFDEC2RST))
#endif /*OTFDEC2*/
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_FORCE_RESET()        (RCC->AHB3RSTR |= (RCC_AHB3RSTR_GFXMMURST))
#endif /*GFXMMU*/

#define __HAL_RCC_AHB3_RELEASE_RESET()        (RCC->AHB3RSTR = 0x00)
#define __HAL_RCC_MDMA_RELEASE_RESET()        (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_MDMARST))
#define __HAL_RCC_DMA2D_RELEASE_RESET()       (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_DMA2DRST))
#if defined(JPEG)
#define __HAL_RCC_JPGDECRST_RELEASE_RESET()   (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_JPGDECRST))
#endif /* JPEG */
#define __HAL_RCC_FMC_RELEASE_RESET()         (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_FMCRST))
#if defined(QUADSPI)
#define __HAL_RCC_QSPI_RELEASE_RESET()        (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_QSPIRST))
#endif /*QUADSPI*/
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_RELEASE_RESET()       (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_OSPI1RST))
#endif /*OCTOSPI1*/
#define __HAL_RCC_SDMMC1_RELEASE_RESET()      (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_SDMMC1RST))
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_RELEASE_RESET()       (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_OSPI2RST))
#endif /*OCTOSPI2*/
#if defined(OCTOSPIM)
#define __HAL_RCC_IOMNGR_RELEASE_RESET()      (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_IOMNGRRST))
#endif /*OCTOSPIM*/
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_RELEASE_RESET()       (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_OTFDEC1RST))
#endif /*OTFDEC1*/
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_RELEASE_RESET()       (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_OTFDEC2RST))
#endif /*OTFDEC2*/
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_RELEASE_RESET()      (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_GFXMMURST))
#endif /*GFXMMU*/



/** @brief  Force or release the AHB1 peripheral reset.
  */
#define __HAL_RCC_AHB1_FORCE_RESET()             (RCC->AHB1RSTR = 0xFFFFFFFFU)
#define __HAL_RCC_DMA1_FORCE_RESET()             (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA1RST))
#define __HAL_RCC_DMA2_FORCE_RESET()             (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA2RST))
#define __HAL_RCC_ADC12_FORCE_RESET()            (RCC->AHB1RSTR |= (RCC_AHB1RSTR_ADC12RST))
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_FORCE_RESET()              (RCC->AHB1RSTR |= (RCC_AHB1RSTR_ARTRST))
#endif /*DUAL_CORE*/
#if defined(RCC_AHB1RSTR_CRCRST)
#define __HAL_RCC_CRC_FORCE_RESET()            (RCC->AHB1RSTR |= (RCC_AHB1RSTR_CRCRST))
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_FORCE_RESET()          (RCC->AHB1RSTR |= (RCC_AHB1RSTR_ETH1MACRST))
#endif /*ETH*/
#define __HAL_RCC_USB1_OTG_HS_FORCE_RESET()      (RCC->AHB1RSTR |= (RCC_AHB1RSTR_USB1OTGHSRST))
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_FORCE_RESET()      (RCC->AHB1RSTR |= (RCC_AHB1RSTR_USB2OTGHSRST))
#endif /*USB2_OTG_FS*/

#define __HAL_RCC_AHB1_RELEASE_RESET()           (RCC->AHB1RSTR = 0x00U)
#define __HAL_RCC_DMA1_RELEASE_RESET()             (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_DMA1RST))
#define __HAL_RCC_DMA2_RELEASE_RESET()             (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_DMA2RST))
#define __HAL_RCC_ADC12_RELEASE_RESET()            (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_ADC12RST))
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_RELEASE_RESET()              (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_ARTRST))
#endif /*DUAL_CORE*/
#if defined(RCC_AHB1RSTR_CRCRST)
#define __HAL_RCC_CRC_RELEASE_RESET()                (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_CRCRST))
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_RELEASE_RESET()          (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_ETH1MACRST))
#endif /*ETH*/
#define __HAL_RCC_USB1_OTG_HS_RELEASE_RESET()      (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_USB1OTGHSRST))
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_RELEASE_RESET()      (RCC->AHB1RSTR &= ~ (RCC_AHB1RSTR_USB2OTGHSRST))
#endif /*USB2_OTG_FS*/

/** @brief  Force or release the AHB2 peripheral reset.
  */
#define __HAL_RCC_AHB2_FORCE_RESET()           (RCC->AHB2RSTR = 0xFFFFFFFFU)
#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_FORCE_RESET()        (RCC->AHB2RSTR |= (RCC_AHB2RSTR_DCMI_PSSIRST))
#define __HAL_RCC_DCMI_FORCE_RESET()             __HAL_RCC_DCMI_PSSI_FORCE_RESET()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_FORCE_RESET()             (RCC->AHB2RSTR |= (RCC_AHB2RSTR_DCMIRST))
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_FORCE_RESET()             (RCC->AHB2RSTR |= (RCC_AHB2RSTR_CRYPRST))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_FORCE_RESET()             (RCC->AHB2RSTR |= (RCC_AHB2RSTR_HASHRST))
#endif /* HASH */
#define __HAL_RCC_RNG_FORCE_RESET()              (RCC->AHB2RSTR |= (RCC_AHB2RSTR_RNGRST))
#define __HAL_RCC_SDMMC2_FORCE_RESET()           (RCC->AHB2RSTR |= (RCC_AHB2RSTR_SDMMC2RST))
#if defined(RCC_AHB2RSTR_HSEMRST)
#define __HAL_RCC_HSEM_FORCE_RESET()             (RCC->AHB2RSTR |= (RCC_AHB2RSTR_HSEMRST))
#endif
#if defined(BDMA1)
#define __HAL_RCC_BDMA1_FORCE_RESET()           (RCC->AHB2RSTR |= (RCC_AHB2RSTR_BDMA1RST))
#endif /*BDMA1*/

#define __HAL_RCC_AHB2_RELEASE_RESET()           (RCC->AHB2RSTR = 0x00U)
#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_RELEASE_RESET()        (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_DCMI_PSSIRST))
#define __HAL_RCC_DCMI_RELEASE_RESET()             __HAL_RCC_DCMI_PSSI_RELEASE_RESET()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_RELEASE_RESET()             (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_DCMIRST))
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_RELEASE_RESET()             (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_CRYPRST))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_RELEASE_RESET()             (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_HASHRST))
#endif /* HASH */
#define __HAL_RCC_RNG_RELEASE_RESET()              (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_RNGRST))
#define __HAL_RCC_SDMMC2_RELEASE_RESET()           (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_SDMMC2RST))
#if defined(RCC_AHB2RSTR_HSEMRST)
#define __HAL_RCC_HSEM_RELEASE_RESET()             (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_HSEMRST))
#endif
#if defined(BDMA1)
#define __HAL_RCC_BDMA1_RELEASE_RESET()           (RCC->AHB2RSTR &= ~ (RCC_AHB2RSTR_BDMA1RST))
#endif /*BDMA1*/


/** @brief  Force or release the AHB4 peripheral reset.
  */

#define __HAL_RCC_AHB4_FORCE_RESET()            (RCC->AHB4RSTR = 0xFFFFFFFFU)
#define __HAL_RCC_GPIOA_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOARST)
#define __HAL_RCC_GPIOB_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOBRST)
#define __HAL_RCC_GPIOC_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOCRST)
#define __HAL_RCC_GPIOD_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIODRST)
#define __HAL_RCC_GPIOE_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOERST)
#define __HAL_RCC_GPIOF_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOFRST)
#define __HAL_RCC_GPIOG_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOGRST)
#define __HAL_RCC_GPIOH_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOHRST)
#define __HAL_RCC_GPIOI_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOIRST)
#define __HAL_RCC_GPIOJ_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOJRST)
#define __HAL_RCC_GPIOK_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_GPIOKRST)
#if defined(RCC_AHB4RSTR_CRCRST)
#define __HAL_RCC_CRC_FORCE_RESET()             (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_CRCRST)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_FORCE_RESET()           (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_BDMA2RST)
#define __HAL_RCC_BDMA_FORCE_RESET()            __HAL_RCC_BDMA2_FORCE_RESET()         /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_FORCE_RESET()            (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_BDMARST)
#endif /*BDMA2*/
#if defined(ADC3)
#define __HAL_RCC_ADC3_FORCE_RESET()            (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_ADC3RST)
#endif /*ADC3*/
#if defined(RCC_AHB4RSTR_HSEMRST)
#define __HAL_RCC_HSEM_FORCE_RESET()            (RCC->AHB4RSTR) |= (RCC_AHB4RSTR_HSEMRST)
#endif

#define __HAL_RCC_AHB4_RELEASE_RESET()          (RCC->AHB4RSTR = 0x00U)
#define __HAL_RCC_GPIOA_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOARST)
#define __HAL_RCC_GPIOB_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOBRST)
#define __HAL_RCC_GPIOC_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOCRST)
#define __HAL_RCC_GPIOD_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIODRST)
#define __HAL_RCC_GPIOE_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOERST)
#define __HAL_RCC_GPIOF_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOFRST)
#define __HAL_RCC_GPIOG_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOGRST)
#define __HAL_RCC_GPIOH_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOHRST)
#define __HAL_RCC_GPIOI_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOIRST)
#define __HAL_RCC_GPIOJ_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOJRST)
#define __HAL_RCC_GPIOK_RELEASE_RESET()           (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_GPIOKRST)
#if defined(RCC_AHB4RSTR_CRCRST)
#define __HAL_RCC_CRC_RELEASE_RESET()             (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_CRCRST)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_RELEASE_RESET()            (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_BDMA2RST)
#define __HAL_RCC_BDMA_RELEASE_RESET()   __HAL_RCC_BDMA2_RELEASE_RESET()      /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_RELEASE_RESET()            (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_BDMARST)
#endif /*BDMA2*/
#if defined(ADC3)
#define __HAL_RCC_ADC3_RELEASE_RESET()            (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_ADC3RST)
#endif /*ADC3*/
#if defined(RCC_AHB4RSTR_HSEMRST)
#define __HAL_RCC_HSEM_RELEASE_RESET()            (RCC->AHB4RSTR) &= ~ (RCC_AHB4RSTR_HSEMRST)
#endif

/** @brief  Force or release the APB3 peripheral reset.
  */
#define __HAL_RCC_APB3_FORCE_RESET()         (RCC->APB3RSTR = 0xFFFFFFFFU)
#if defined(LTDC)
#define __HAL_RCC_LTDC_FORCE_RESET()           (RCC->APB3RSTR) |= (RCC_APB3RSTR_LTDCRST)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_FORCE_RESET()            (RCC->APB3RSTR) |= (RCC_APB3RSTR_DSIRST)
#endif /*DSI*/

#define __HAL_RCC_APB3_RELEASE_RESET()         (RCC->APB3RSTR = 0x00U)
#if defined(LTDC)
#define __HAL_RCC_LTDC_RELEASE_RESET()           (RCC->APB3RSTR) &= ~ (RCC_APB3RSTR_LTDCRST)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_RELEASE_RESET()            (RCC->APB3RSTR) &= ~ (RCC_APB3RSTR_DSIRST)
#endif /*DSI*/

/** @brief  Force or release the APB1 peripheral reset.
  */
#define __HAL_RCC_APB1L_FORCE_RESET()        (RCC->APB1LRSTR = 0xFFFFFFFFU)
#define __HAL_RCC_APB1H_FORCE_RESET()        (RCC->APB1HRSTR = 0xFFFFFFFFU)
#define __HAL_RCC_TIM2_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM2RST)
#define __HAL_RCC_TIM3_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM3RST)
#define __HAL_RCC_TIM4_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM4RST)
#define __HAL_RCC_TIM5_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM5RST)
#define __HAL_RCC_TIM6_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM6RST)
#define __HAL_RCC_TIM7_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM7RST)
#define __HAL_RCC_TIM12_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM12RST)
#define __HAL_RCC_TIM13_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM13RST)
#define __HAL_RCC_TIM14_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_TIM14RST)
#define __HAL_RCC_LPTIM1_FORCE_RESET()         (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_LPTIM1RST)
#define __HAL_RCC_SPI2_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_SPI2RST)
#define __HAL_RCC_SPI3_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_SPI3RST)
#define __HAL_RCC_SPDIFRX_FORCE_RESET()        (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_SPDIFRXRST)
#define __HAL_RCC_USART2_FORCE_RESET()         (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_USART2RST)
#define __HAL_RCC_USART3_FORCE_RESET()         (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_USART3RST)
#define __HAL_RCC_UART4_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_UART4RST)
#define __HAL_RCC_UART5_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_UART5RST)
#define __HAL_RCC_I2C1_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_I2C1RST)
#define __HAL_RCC_I2C2_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_I2C2RST)
#define __HAL_RCC_I2C3_FORCE_RESET()           (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_I2C3RST)
#define __HAL_RCC_CEC_FORCE_RESET()            (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_CECRST)
#define __HAL_RCC_DAC12_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_DAC12RST)
#define __HAL_RCC_UART7_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_UART7RST)
#define __HAL_RCC_UART8_FORCE_RESET()          (RCC->APB1LRSTR) |= (RCC_APB1LRSTR_UART8RST)
#define __HAL_RCC_CRS_FORCE_RESET()            (RCC->APB1HRSTR) |= (RCC_APB1HRSTR_CRSRST)
#define __HAL_RCC_SWPMI1_FORCE_RESET()          (RCC->APB1HRSTR) |= (RCC_APB1HRSTR_SWPMIRST)
#define __HAL_RCC_OPAMP_FORCE_RESET()          (RCC->APB1HRSTR) |= (RCC_APB1HRSTR_OPAMPRST)
#define __HAL_RCC_MDIOS_FORCE_RESET()          (RCC->APB1HRSTR) |= (RCC_APB1HRSTR_MDIOSRST)
#define __HAL_RCC_FDCAN_FORCE_RESET()          (RCC->APB1HRSTR) |= (RCC_APB1HRSTR_FDCANRST)

#define __HAL_RCC_APB1L_RELEASE_RESET()       (RCC->APB1LRSTR = 0x00U)
#define __HAL_RCC_APB1H_RELEASE_RESET()       (RCC->APB1HRSTR = 0x00U)
#define __HAL_RCC_TIM2_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM2RST)
#define __HAL_RCC_TIM3_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM3RST)
#define __HAL_RCC_TIM4_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM4RST)
#define __HAL_RCC_TIM5_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM5RST)
#define __HAL_RCC_TIM6_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM6RST)
#define __HAL_RCC_TIM7_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM7RST)
#define __HAL_RCC_TIM12_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM12RST)
#define __HAL_RCC_TIM13_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM13RST)
#define __HAL_RCC_TIM14_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_TIM14RST)
#define __HAL_RCC_LPTIM1_RELEASE_RESET()         (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_LPTIM1RST)
#define __HAL_RCC_SPI2_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_SPI2RST)
#define __HAL_RCC_SPI3_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_SPI3RST)
#define __HAL_RCC_SPDIFRX_RELEASE_RESET()        (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_SPDIFRXRST)
#define __HAL_RCC_USART2_RELEASE_RESET()         (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_USART2RST)
#define __HAL_RCC_USART3_RELEASE_RESET()         (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_USART3RST)
#define __HAL_RCC_UART4_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_UART4RST)
#define __HAL_RCC_UART5_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_UART5RST)
#define __HAL_RCC_I2C1_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_I2C1RST)
#define __HAL_RCC_I2C2_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_I2C2RST)
#define __HAL_RCC_I2C3_RELEASE_RESET()           (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_I2C3RST)
#define __HAL_RCC_CEC_RELEASE_RESET()            (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_CECRST)
#define __HAL_RCC_DAC12_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_DAC12RST)
#define __HAL_RCC_UART7_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_UART7RST)
#define __HAL_RCC_UART8_RELEASE_RESET()          (RCC->APB1LRSTR) &= ~ (RCC_APB1LRSTR_UART8RST)
#define __HAL_RCC_CRS_RELEASE_RESET()            (RCC->APB1HRSTR) &= ~ (RCC_APB1HRSTR_CRSRST)
#define __HAL_RCC_SWPMI1_RELEASE_RESET()          (RCC->APB1HRSTR) &= ~ (RCC_APB1HRSTR_SWPMIRST)
#define __HAL_RCC_OPAMP_RELEASE_RESET()          (RCC->APB1HRSTR) &= ~ (RCC_APB1HRSTR_OPAMPRST)
#define __HAL_RCC_MDIOS_RELEASE_RESET()          (RCC->APB1HRSTR) &= ~ (RCC_APB1HRSTR_MDIOSRST)
#define __HAL_RCC_FDCAN_RELEASE_RESET()          (RCC->APB1HRSTR) &= ~ (RCC_APB1HRSTR_FDCANRST)

/** @brief  Force or release the APB2 peripheral reset.
  */
#define __HAL_RCC_APB2_FORCE_RESET()         (RCC->APB2RSTR = 0xFFFFFFFFU)
#define __HAL_RCC_TIM1_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_TIM1RST)
#define __HAL_RCC_TIM8_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_TIM8RST)
#define __HAL_RCC_USART1_FORCE_RESET()         (RCC->APB2RSTR) |= (RCC_APB2RSTR_USART1RST)
#define __HAL_RCC_USART6_FORCE_RESET()         (RCC->APB2RSTR) |= (RCC_APB2RSTR_USART6RST)
#if defined(UART9)
#define __HAL_RCC_UART9_FORCE_RESET()         (RCC->APB2RSTR) |= (RCC_APB2RSTR_UART9RST)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_FORCE_RESET()         (RCC->APB2RSTR) |= (RCC_APB2RSTR_USART10RST)
#endif /*USART10*/
#define __HAL_RCC_SPI1_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_SPI1RST)
#define __HAL_RCC_SPI4_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_SPI4RST)
#define __HAL_RCC_TIM15_FORCE_RESET()          (RCC->APB2RSTR) |= (RCC_APB2RSTR_TIM15RST)
#define __HAL_RCC_TIM16_FORCE_RESET()          (RCC->APB2RSTR) |= (RCC_APB2RSTR_TIM16RST)
#define __HAL_RCC_TIM17_FORCE_RESET()          (RCC->APB2RSTR) |= (RCC_APB2RSTR_TIM17RST)
#define __HAL_RCC_SPI5_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_SPI5RST)
#define __HAL_RCC_SAI1_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_SAI1RST)
#define __HAL_RCC_SAI2_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_SAI2RST)
#if defined(SAI3)
#define __HAL_RCC_SAI3_FORCE_RESET()           (RCC->APB2RSTR) |= (RCC_APB2RSTR_SAI3RST)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_FORCE_RESET()         (RCC->APB2RSTR) |= (RCC_APB2RSTR_DFSDM1RST)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_FORCE_RESET()         (RCC->APB2RSTR) |= (RCC_APB2RSTR_HRTIMRST)
#endif /*HRTIM1*/

#define __HAL_RCC_APB2_RELEASE_RESET()         (RCC->APB2RSTR = 0x00U)
#define __HAL_RCC_TIM1_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_TIM1RST)
#define __HAL_RCC_TIM8_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_TIM8RST)
#define __HAL_RCC_USART1_RELEASE_RESET()         (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_USART1RST)
#define __HAL_RCC_USART6_RELEASE_RESET()         (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_USART6RST)
#if defined(UART9)
#define __HAL_RCC_UART9_RELEASE_RESET()         (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_UART9RST)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_RELEASE_RESET()         (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_USART10RST)
#endif /*USART10*/
#define __HAL_RCC_SPI1_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_SPI1RST)
#define __HAL_RCC_SPI4_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_SPI4RST)
#define __HAL_RCC_TIM15_RELEASE_RESET()          (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_TIM15RST)
#define __HAL_RCC_TIM16_RELEASE_RESET()          (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_TIM16RST)
#define __HAL_RCC_TIM17_RELEASE_RESET()          (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_TIM17RST)
#define __HAL_RCC_SPI5_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_SPI5RST)
#define __HAL_RCC_SAI1_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_SAI1RST)
#define __HAL_RCC_SAI2_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_SAI2RST)
#if defined(SAI3)
#define __HAL_RCC_SAI3_RELEASE_RESET()           (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_SAI3RST)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_RELEASE_RESET()         (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_DFSDM1RST)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_RELEASE_RESET()         (RCC->APB2RSTR) &= ~ (RCC_APB2RSTR_HRTIMRST)
#endif /*HRTIM1*/

/** @brief  Force or release the APB4 peripheral reset.
  */

#define __HAL_RCC_APB4_FORCE_RESET()           (RCC->APB4RSTR = 0xFFFFFFFFU)
#define __HAL_RCC_SYSCFG_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_SYSCFGRST)
#define __HAL_RCC_LPUART1_FORCE_RESET()          (RCC->APB4RSTR) |= (RCC_APB4RSTR_LPUART1RST)
#define __HAL_RCC_SPI6_FORCE_RESET()             (RCC->APB4RSTR) |= (RCC_APB4RSTR_SPI6RST)
#define __HAL_RCC_I2C4_FORCE_RESET()             (RCC->APB4RSTR) |= (RCC_APB4RSTR_I2C4RST)
#define __HAL_RCC_LPTIM2_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_LPTIM2RST)
#define __HAL_RCC_LPTIM3_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_LPTIM3RST)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_LPTIM4RST)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_LPTIM5RST)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_FORCE_RESET()             (RCC->APB4RSTR) |= (RCC_APB4RSTR_DAC2RST)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_COMP12RST)
#define __HAL_RCC_VREF_FORCE_RESET()             (RCC->APB4RSTR) |= (RCC_APB4RSTR_VREFRST)
#if defined(SAI4)
#define __HAL_RCC_SAI4_FORCE_RESET()             (RCC->APB4RSTR) |= (RCC_APB4RSTR_SAI4RST)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_FORCE_RESET()              (RCC->APB4RSTR) |= (RCC_APB4RSTR_DTSRST)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_FORCE_RESET()           (RCC->APB4RSTR) |= (RCC_APB4RSTR_DFSDM2RST)
#endif /*DFSDM2*/

#define __HAL_RCC_APB4_RELEASE_RESET()           (RCC->APB4RSTR = 0x00U)
#define __HAL_RCC_SYSCFG_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_SYSCFGRST)
#define __HAL_RCC_LPUART1_RELEASE_RESET()          (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_LPUART1RST)
#define __HAL_RCC_SPI6_RELEASE_RESET()             (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_SPI6RST)
#define __HAL_RCC_I2C4_RELEASE_RESET()             (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_I2C4RST)
#define __HAL_RCC_LPTIM2_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_LPTIM2RST)
#define __HAL_RCC_LPTIM3_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_LPTIM3RST)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_LPTIM4RST)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_LPTIM5RST)
#endif /*LPTIM5*/
#if defined(RCC_APB4RSTR_DAC2RST)
#define __HAL_RCC_DAC2_RELEASE_RESET()             (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_DAC2RST)
#endif
#define __HAL_RCC_COMP12_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_COMP12RST)
#define __HAL_RCC_VREF_RELEASE_RESET()             (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_VREFRST)
#if defined(SAI4)
#define __HAL_RCC_SAI4_RELEASE_RESET()             (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_SAI4RST)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_RELEASE_RESET()              (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_DTSRST)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_RELEASE_RESET()           (RCC->APB4RSTR) &= ~ (RCC_APB4RSTR_DFSDM2RST)
#endif /*DFSDM2*/

/** @brief  Enable or disable the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */


#define __HAL_RCC_MDMA_CLK_SLEEP_ENABLE()            (RCC->AHB3LPENR |= (RCC_AHB3LPENR_MDMALPEN))
#define __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE()           (RCC->AHB3LPENR |= (RCC_AHB3LPENR_DMA2DLPEN))
#if defined(JPEG)
#define __HAL_RCC_JPGDEC_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_JPGDECLPEN))
#endif /* JPEG */
#define __HAL_RCC_FLASH_CLK_SLEEP_ENABLE()           (RCC->AHB3LPENR |= (RCC_AHB3LPENR_FLASHLPEN))
#define __HAL_RCC_FMC_CLK_SLEEP_ENABLE()             (RCC->AHB3LPENR |= (RCC_AHB3LPENR_FMCLPEN))
#if defined(QUADSPI)
#define __HAL_RCC_QSPI_CLK_SLEEP_ENABLE()            (RCC->AHB3LPENR |= (RCC_AHB3LPENR_QSPILPEN))
#endif /*QUADSPI*/
#define __HAL_RCC_SDMMC1_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_SDMMC1LPEN))
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_OSPI1LPEN))
#endif /*OCTOSPI1*/
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_OSPI2LPEN))
#endif /*OCTOSPI2*/
#if defined(OCTOSPIM)
#define __HAL_RCC_IOMNGR_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_IOMNGRLPEN))
#endif /*OCTOSPIM*/
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_OTFDEC1LPEN))
#endif /*OTFDEC1*/
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_OTFDEC2LPEN))
#endif /*OTFDEC2*/
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_GFXMMULPEN))
#endif /*GFXMMU*/
#if defined(CD_AXISRAM2_BASE)
#define __HAL_RCC_AXISRAM2_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_AXISRAM2LPEN))
#endif
#if defined(CD_AXISRAM3_BASE)
#define __HAL_RCC_AXISRAM3_CLK_SLEEP_ENABLE()          (RCC->AHB3LPENR |= (RCC_AHB3LPENR_AXISRAM3LPEN))
#endif
#define __HAL_RCC_DTCM1_CLK_SLEEP_ENABLE()           (RCC->AHB3LPENR |= (RCC_AHB3LPENR_DTCM1LPEN))
#define __HAL_RCC_DTCM2_CLK_SLEEP_ENABLE()           (RCC->AHB3LPENR |= (RCC_AHB3LPENR_DTCM2LPEN))
#define __HAL_RCC_ITCM_CLK_SLEEP_ENABLE()            (RCC->AHB3LPENR |= (RCC_AHB3LPENR_ITCMLPEN))
#if defined(RCC_AHB3LPENR_AXISRAMLPEN)
#define __HAL_RCC_D1SRAM1_CLK_SLEEP_ENABLE()         (RCC->AHB3LPENR |= (RCC_AHB3LPENR_AXISRAMLPEN))
#define __HAL_RCC_AXISRAM_CLK_SLEEP_ENABLE           __HAL_RCC_D1SRAM1_CLK_SLEEP_ENABLE
#else
#define __HAL_RCC_AXISRAM1_CLK_SLEEP_ENABLE()        (RCC->AHB3LPENR |= (RCC_AHB3LPENR_AXISRAM1LPEN))
#define __HAL_RCC_D1SRAM1_CLK_SLEEP_ENABLE           __HAL_RCC_AXISRAM1_CLK_SLEEP_ENABLE  /* For backward compatibility */
#endif /* RCC_AHB3LPENR_AXISRAMLPEN */

#define __HAL_RCC_MDMA_CLK_SLEEP_DISABLE()            (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_MDMALPEN))
#define __HAL_RCC_DMA2D_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_DMA2DLPEN))
#if defined(JPEG)
#define __HAL_RCC_JPGDEC_CLK_SLEEP_DISABLE()          (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_JPGDECLPEN))
#endif /* JPEG */
#define __HAL_RCC_FLASH_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_FLASHLPEN))
#define __HAL_RCC_FMC_CLK_SLEEP_DISABLE()             (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_FMCLPEN))
#if defined(QUADSPI)
#define __HAL_RCC_QSPI_CLK_SLEEP_DISABLE()            (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_QSPILPEN))
#endif /*QUADSPI*/
#define __HAL_RCC_SDMMC1_CLK_SLEEP_DISABLE()          (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_SDMMC1LPEN))
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_OSPI1LPEN))
#endif /*OCTOSPI1*/
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_OSPI2LPEN))
#endif /*OCTOSPI2*/
#if defined(OCTOSPIM)
#define __HAL_RCC_IOMNGR_CLK_SLEEP_DISABLE()          (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_IOMNGRLPEN))
#endif /*OCTOSPIM*/
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_OTFDEC1LPEN))
#endif /*OTFDEC1*/
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_OTFDEC2LPEN))
#endif /*OTFDEC2*/
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_GFXMMULPEN))
#endif /*GFXMMU*/
#if defined(CD_AXISRAM2_BASE)
#define __HAL_RCC_AXISRAM2_CLK_SLEEP_DISABLE()         (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_AXISRAM2LPEN))
#endif
#if defined(CD_AXISRAM3_BASE)
#define __HAL_RCC_AXISRAM3_CLK_SLEEP_DISABLE()         (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_AXISRAM3LPEN))
#endif
#define __HAL_RCC_DTCM1_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_DTCM1LPEN))
#define __HAL_RCC_DTCM2_CLK_SLEEP_DISABLE()           (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_DTCM2LPEN))
#define __HAL_RCC_ITCM_CLK_SLEEP_DISABLE()            (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_ITCMLPEN))
#if defined(RCC_AHB3LPENR_AXISRAMLPEN)
#define __HAL_RCC_D1SRAM1_CLK_SLEEP_DISABLE()         (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_AXISRAMLPEN))
#define __HAL_RCC_AXISRAM_CLK_SLEEP_DISABLE           __HAL_RCC_D1SRAM1_CLK_SLEEP_DISABLE
#else
#define __HAL_RCC_AXISRAM1_CLK_SLEEP_DISABLE()        (RCC->AHB3LPENR &= ~ (RCC_AHB3LPENR_AXISRAM1LPEN))
#define __HAL_RCC_D1SRAM1_CLK_SLEEP_DISABLE          __HAL_RCC_AXISRAM1_CLK_SLEEP_DISABLE      /* For backward compatibility */
#endif  /* RCC_AHB3LPENR_AXISRAMLPEN */

/** @brief  Get the enable or disable status of the AHB3 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#define __HAL_RCC_MDMA_IS_CLK_SLEEP_ENABLED()             ((RCC->AHB3LPENR & RCC_AHB3LPENR_MDMALPEN)    != 0U)
#define __HAL_RCC_DMA2D_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_DMA2DLPEN)   != 0U)
#if defined(JPEG)
#define __HAL_RCC_JPGDEC_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_JPGDECLPEN)  != 0U)
#endif /* JPEG */
#define __HAL_RCC_FLASH_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_FLASHLPEN)   != 0U)
#define __HAL_RCC_FMC_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB3LPENR & RCC_AHB3LPENR_FMCLPEN)     != 0U)
#if defined(QUADSPI)
#define __HAL_RCC_QSPI_IS_CLK_SLEEP_ENABLED()             ((RCC->AHB3LPENR & RCC_AHB3LPENR_QSPILPEN)    != 0U)
#endif /*QUADSPI*/
#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_SDMMC1LPEN)  != 0U)
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OSPI1LPEN)    != 0U)
#endif /*OCTOSPI1*/
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OSPI2LPEN)    != 0U)
#endif /*OCTOSPI2*/
#if defined(OCTOSPIM)
#define __HAL_RCC_IOMNGR_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_IOMNGRLPEN)   != 0U)
#endif /*OCTOSPIM*/
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OTFDEC1LPEN) != 0U)
#endif /*OTFDEC1*/
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OTFDEC2LPEN) != 0U)
#endif /*OTFDEC2*/
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_GFXMMULPEN)    != 0U)
#endif /*GFXMMU*/
#if defined(CD_AXISRAM2_BASE)
#define __HAL_RCC_AXISRAM2_IS_CLK_SLEEP_ENABLED()         ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAM2LPEN)   != 0U)
#endif
#if defined(CD_AXISRAM3_BASE)
#define __HAL_RCC_AXISRAM3_IS_CLK_SLEEP_ENABLED()         ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAM3LPEN)   != 0U)
#endif
#define __HAL_RCC_DTCM1_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_DTCM1LPEN)   != 0U)
#define __HAL_RCC_DTCM2_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_DTCM2LPEN)   != 0U)
#define __HAL_RCC_ITCM_IS_CLK_SLEEP_ENABLED()             ((RCC->AHB3LPENR & RCC_AHB3LPENR_ITCMLPEN)    != 0U)
#if defined(RCC_AHB3LPENR_AXISRAMLPEN)
#define __HAL_RCC_D1SRAM1_IS_CLK_SLEEP_ENABLED()          ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAMLPEN) != 0U)
#else
#define __HAL_RCC_AXISRAM1_IS_CLK_SLEEP_ENABLED()         ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAM1LPEN) != 0U)
#endif

#define __HAL_RCC_MDMA_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_MDMALPEN)    == 0U)
#define __HAL_RCC_DMA2D_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_DMA2DLPEN)   == 0U)
#if defined(JPEG)
#define __HAL_RCC_JPGDEC_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB3LPENR & RCC_AHB3LPENR_JPGDECLPEN)  == 0U)
#endif /* JPEG */
#define __HAL_RCC_FLASH_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_FLASHLPEN)   == 0U)
#define __HAL_RCC_FMC_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB3LPENR & RCC_AHB3LPENR_FMCLPEN)     == 0U)
#if defined(QUADSPI)
#define __HAL_RCC_QSPI_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_QSPILPEN)    == 0U)
#endif /*QUADSPI*/
#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB3LPENR & RCC_AHB3LPENR_SDMMC1LPEN)  == 0U)
#if defined(OCTOSPI1)
#define __HAL_RCC_OSPI1_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OSPI1LPEN)  == 0U)
#endif /*OCTOSPI1*/
#if defined(OCTOSPI2)
#define __HAL_RCC_OSPI2_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OSPI2LPEN)  == 0U)
#endif /*OCTOSPI2*/
#if defined(OCTOSPIM)
#define __HAL_RCC_IOMNGR_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_IOMNGRLPEN) == 0U)
#endif /*OCTOSPIM*/
#if defined(OTFDEC1)
#define __HAL_RCC_OTFDEC1_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OTFDEC1LPEN)  == 0U)
#endif /*OTFDEC1*/
#if defined(OTFDEC2)
#define __HAL_RCC_OTFDEC2_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_OTFDEC2LPEN)  == 0U)
#endif /*OTFDEC2*/
#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_GFXMMULPEN)  == 0U)
#endif /*GFXMMU*/
#if defined(CD_AXISRAM2_BASE)
#define __HAL_RCC_AXISRAM2_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAM2LPEN)  == 0U)
#endif
#if defined(CD_AXISRAM3_BASE)
#define __HAL_RCC_AXISRAM3_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAM3LPEN)  == 0U)
#endif
#define __HAL_RCC_DTCM1_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_DTCM1LPEN)   == 0U)
#define __HAL_RCC_DTCM2_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB3LPENR & RCC_AHB3LPENR_DTCM2LPEN)   == 0U)
#define __HAL_RCC_ITCM_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB3LPENR & RCC_AHB3LPENR_ITCMLPEN)    == 0U)
#if defined(RCC_AHB3LPENR_AXISRAMLPEN)
#define __HAL_RCC_D1SRAM1_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAMLPEN) == 0U)
#else
#define __HAL_RCC_AXISRAM1_IS_CLK_SLEEP_DISABLED()        ((RCC->AHB3LPENR & RCC_AHB3LPENR_AXISRAML1PEN) == 0U)
#endif /* RCC_AHB3LPENR_AXISRAMLPEN */

/** @brief  ENABLE or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_DMA1_CLK_SLEEP_ENABLE()             (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DMA1LPEN))
#define __HAL_RCC_DMA2_CLK_SLEEP_ENABLE()             (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_ADC12_CLK_SLEEP_ENABLE()            (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ADC12LPEN))
#if defined(RCC_AHB1LPENR_CRCLPEN)
#define __HAL_RCC_CRC_CLK_SLEEP_ENABLE()              (RCC->AHB1LPENR |= (RCC_AHB1LPENR_CRCLPEN))
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_CLK_SLEEP_ENABLE()          (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETH1MACLPEN))
#endif /*ETH*/
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_CLK_SLEEP_ENABLE()              (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ARTLPEN))
#endif /*DUAL_CORE*/
#if defined(ETH)
#define __HAL_RCC_ETH1TX_CLK_SLEEP_ENABLE()           (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETH1TXLPEN))
#define __HAL_RCC_ETH1RX_CLK_SLEEP_ENABLE()           (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETH1RXLPEN))
#endif /*ETH*/
#define __HAL_RCC_USB1_OTG_HS_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_USB1OTGHSLPEN))
#define __HAL_RCC_USB1_OTG_HS_ULPI_CLK_SLEEP_ENABLE() (RCC->AHB1LPENR |= (RCC_AHB1LPENR_USB1OTGHSULPILPEN))
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_USB2OTGHSLPEN))
#define __HAL_RCC_USB2_OTG_FS_ULPI_CLK_SLEEP_ENABLE() (RCC->AHB1LPENR |= (RCC_AHB1LPENR_USB2OTGHSULPILPEN))
#endif /* USB2_OTG_FS */

#define __HAL_RCC_DMA1_CLK_SLEEP_DISABLE()             (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_DMA1LPEN))
#define __HAL_RCC_DMA2_CLK_SLEEP_DISABLE()             (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_ADC12_CLK_SLEEP_DISABLE()            (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_ADC12LPEN))
#if defined(RCC_AHB1LPENR_CRCLPEN)
#define __HAL_RCC_CRC_CLK_SLEEP_DISABLE()              (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_CRCLPEN))
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_CLK_SLEEP_DISABLE()          (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1MACLPEN))
#endif  /*ETH*/
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_CLK_SLEEP_DISABLE()              (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_ARTLPEN))
#endif /*DUAL_CORE*/
#if defined(ETH)
#define __HAL_RCC_ETH1TX_CLK_SLEEP_DISABLE()           (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1TXLPEN))
#define __HAL_RCC_ETH1RX_CLK_SLEEP_DISABLE()           (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1RXLPEN))
#endif  /*ETH*/
#define __HAL_RCC_USB1_OTG_HS_CLK_SLEEP_DISABLE()      (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB1OTGHSLPEN))
#define __HAL_RCC_USB1_OTG_HS_ULPI_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB1OTGHSULPILPEN))
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_CLK_SLEEP_DISABLE()      (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB2OTGHSLPEN))
#define __HAL_RCC_USB2_OTG_FS_ULPI_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB2OTGHSULPILPEN))
#endif /* USB2_OTG_FS */

/** @brief  Get the enable or disable status of the AHB1 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#define __HAL_RCC_DMA1_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA1LPEN))          != 0U)
#define __HAL_RCC_DMA2_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA2LPEN))          != 0U)
#define __HAL_RCC_ADC12_IS_CLK_SLEEP_ENABLED()             ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ADC12LPEN))         != 0U)
#if defined(RCC_AHB1LPENR_CRCLPEN)
#define __HAL_RCC_CRC_IS_CLK_SLEEP_ENABLED()                ((RCC->AHB1LPENR & (RCC_AHB1LPENR_CRCLPEN))         != 0U)
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETH1MACLPEN))       != 0U)
#endif  /*ETH*/
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_IS_CLK_SLEEP_ENABLED()               ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ARTLPEN))       != 0U)
#endif /*DUAL_CORE*/
#if defined(ETH)
#define __HAL_RCC_ETH1TX_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETH1TXLPEN))        != 0U)
#define __HAL_RCC_ETH1RX_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETH1RXLPEN))        != 0U)
#endif  /*ETH*/
#define __HAL_RCC_USB1_OTG_HS_IS_CLK_SLEEP_ENABLED()       ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB1OTGHSLPEN))     != 0U)
#define __HAL_RCC_USB1_OTG_HS_ULPI_IS_CLK_SLEEP_ENABLED()  ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB1OTGHSULPILPEN)) != 0U)
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_IS_CLK_SLEEP_ENABLED()       ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB2OTGHSLPEN))     != 0U)
#define __HAL_RCC_USB2_OTG_FS_ULPI_IS_CLK_SLEEP_ENABLED()  ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB2OTGHSULPILPEN)) != 0U)
#endif /* USB2_OTG_FS */

#define __HAL_RCC_DMA1_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA1LPEN))          == 0U)
#define __HAL_RCC_DMA2_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA2LPEN))          == 0U)
#define __HAL_RCC_ADC12_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ADC12LPEN))         == 0U)
#if defined(RCC_AHB1LPENR_CRCLPEN)
#define __HAL_RCC_CRC_IS_CLK_SLEEP_DISABLED()              ((RCC->AHB1LPENR & (RCC_AHB1LPENR_CRCLPEN))           == 0U)
#endif
#if defined(ETH)
#define __HAL_RCC_ETH1MAC_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETH1MACLPEN))       == 0U)
#endif /* ETH */
#if defined(DUAL_CORE)
#define __HAL_RCC_ART_IS_CLK_SLEEP_DISABLED()              ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ARTLPEN))           == 0U)
#endif /*DUAL_CORE*/
#if defined(ETH)
#define __HAL_RCC_ETH1TX_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETH1TXLPEN))        == 0U)
#define __HAL_RCC_ETH1RX_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETH1RXLPEN))        == 0U)
#endif /* ETH */
#define __HAL_RCC_USB1_OTG_HS_IS_CLK_SLEEP_DISABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB1OTGHSLPEN))     == 0U)
#define __HAL_RCC_USB1_OTG_HS_ULPI_IS_CLK_SLEEP_DISABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB1OTGHSULPILPEN)) == 0U)
#if defined(USB2_OTG_FS)
#define __HAL_RCC_USB2_OTG_FS_IS_CLK_SLEEP_DISABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB2OTGHSLPEN))     == 0U)
#define __HAL_RCC_USB2_OTG_FS_ULPI_IS_CLK_SLEEP_DISABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_USB2OTGHSULPILPEN)) == 0U)
#endif /* USB2_OTG_FS */


/** @brief  ENABLE or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_CLK_SLEEP_ENABLE()        (RCC->AHB2LPENR |= (RCC_AHB2LPENR_DCMI_PSSILPEN))
#define __HAL_RCC_DCMI_CLK_SLEEP_ENABLE()             __HAL_RCC_DCMI_PSSI_CLK_SLEEP_ENABLE()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_CLK_SLEEP_ENABLE()             (RCC->AHB2LPENR |= (RCC_AHB2LPENR_DCMILPEN))
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_CLK_SLEEP_ENABLE()             (RCC->AHB2LPENR |= (RCC_AHB2LPENR_CRYPLPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_CLK_SLEEP_ENABLE()             (RCC->AHB2LPENR |= (RCC_AHB2LPENR_HASHLPEN))
#endif /* HASH */
#define __HAL_RCC_RNG_CLK_SLEEP_ENABLE()              (RCC->AHB2LPENR |= (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_SDMMC2_CLK_SLEEP_ENABLE()           (RCC->AHB2LPENR |= (RCC_AHB2LPENR_SDMMC2LPEN))
#if defined(RCC_AHB2LPENR_DFSDMDMALPEN)
#define __HAL_RCC_DFSDMDMA_CLK_SLEEP_ENABLE()         (RCC->AHB2LPENR |= (RCC_AHB2LPENR_DFSDMDMALPEN))
#endif
#if defined(RCC_AHB2LPENR_D2SRAM1LPEN)
#define __HAL_RCC_D2SRAM1_CLK_SLEEP_ENABLE()          (RCC->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM1LPEN))
#else
#define __HAL_RCC_AHBSRAM1_CLK_SLEEP_ENABLE()         (RCC->AHB2LPENR |= (RCC_AHB2LPENR_AHBSRAM1LPEN))
#endif /* RCC_AHB2LPENR_D2SRAM1LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM2LPEN)
#define __HAL_RCC_D2SRAM2_CLK_SLEEP_ENABLE()          (RCC->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM2LPEN))
#else
#define __HAL_RCC_AHBSRAM2_CLK_SLEEP_ENABLE()         (RCC->AHB2LPENR |= (RCC_AHB2LPENR_AHBSRAM2LPEN))
#endif /* RCC_AHB2LPENR_D2SRAM2LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM3LPEN)
#define __HAL_RCC_D2SRAM3_CLK_SLEEP_ENABLE()          (RCC->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM3LPEN))
#endif

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_CLK_SLEEP_DISABLE()        (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_DCMI_PSSILPEN))
#define __HAL_RCC_DCMI_CLK_SLEEP_DISABLE()             __HAL_RCC_DCMI_PSSI_CLK_SLEEP_DISABLE()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_CLK_SLEEP_DISABLE()             (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_DCMILPEN))
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_CLK_SLEEP_DISABLE()             (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_CRYPLPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_CLK_SLEEP_DISABLE()             (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_HASHLPEN))
#endif /* HASH */
#define __HAL_RCC_RNG_CLK_SLEEP_DISABLE()              (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_SDMMC2_CLK_SLEEP_DISABLE()           (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_SDMMC2LPEN))
#if defined(RCC_AHB2LPENR_DFSDMDMALPEN)
#define __HAL_RCC_DFSDMDMA_CLK_SLEEP_DISABLE()         (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_DFSDMDMALPEN))
#endif
#if defined(RCC_AHB2LPENR_D2SRAM1LPEN)
#define __HAL_RCC_D2SRAM1_CLK_SLEEP_DISABLE()          (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM1LPEN))
#else
#define __HAL_RCC_AHBSRAM1_CLK_SLEEP_DISABLE()         (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_AHBSRAM1LPEN))
#endif /* RCC_AHB2LPENR_D2SRAM1LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM2LPEN)
#define __HAL_RCC_D2SRAM2_CLK_SLEEP_DISABLE()          (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM2LPEN))
#else
#define __HAL_RCC_AHBSRAM2_CLK_SLEEP_DISABLE()          (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_AHBSRAM2LPEN))
#endif /* RCC_AHB2LPENR_D2SRAM2LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM3LPEN)
#define __HAL_RCC_D2SRAM3_CLK_SLEEP_DISABLE()          (RCC->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM3LPEN))
#endif

/** @brief  Get the enable or disable status of the AHB2 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_IS_CLK_SLEEP_ENABLED()         ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DCMI_PSSILPEN))    != 0U)
#define __HAL_RCC_DCMI_IS_CLK_SLEEP_ENABLED()              __HAL_RCC_DCMI_PSSI_IS_CLK_SLEEP_ENABLED()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DCMILPEN))    != 0U)
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB2LPENR & (RCC_AHB2LPENR_CRYPLPEN))    != 0U)
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB2LPENR & (RCC_AHB2LPENR_HASHLPEN))    != 0U)
#endif /* HASH */
#define __HAL_RCC_RNG_IS_CLK_SLEEP_ENABLED()               ((RCC->AHB2LPENR & (RCC_AHB2LPENR_RNGLPEN))     != 0U)
#define __HAL_RCC_SDMMC2_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB2LPENR & (RCC_AHB2LPENR_SDMMC2LPEN))  != 0U)
#if defined(RCC_AHB2LPENR_DFSDMDMALPEN)
#define __HAL_RCC_DFSDMDMA_IS_CLK_SLEEP_ENABLED()          ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DFSDMDMALPEN)) != 0U)
#endif
#if defined(RCC_AHB2LPENR_D2SRAM1LPEN)
#define __HAL_RCC_D2SRAM1_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB2LPENR & (RCC_AHB2LPENR_D2SRAM1LPEN)) != 0U)
#else
#define __HAL_RCC_AHBSRAM1_IS_CLK_SLEEP_ENABLED()          ((RCC->AHB2LPENR & (RCC_AHB2LPENR_AHBSRAM1LPEN)) != 0U)
#endif /* RCC_AHB2LPENR_D2SRAM1LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM2LPEN)
#define __HAL_RCC_D2SRAM2_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB2LPENR & (RCC_AHB2LPENR_D2SRAM2LPEN)) != 0U)
#else
#define __HAL_RCC_AHBSRAM2_IS_CLK_SLEEP_ENABLED()          ((RCC->AHB2LPENR & (RCC_AHB2LPENR_AHBSRAM2LPEN)) != 0U)
#endif /* RCC_AHB2LPENR_D2SRAM2LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM3LPEN)
#define __HAL_RCC_D2SRAM3_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB2LPENR & (RCC_AHB2LPENR_D2SRAM3LPEN)) != 0U)
#endif /* RCC_AHB2LPENR_D2SRAM3LPEN */

#if defined(DCMI) && defined(PSSI)
#define __HAL_RCC_DCMI_PSSI_IS_CLK_SLEEP_DISABLED()        ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DCMI_PSSILPEN))    == 0U)
#define __HAL_RCC_DCMI_IS_CLK_SLEEP_DISABLED()             __HAL_RCC_DCMI_PSSI_IS_CLK_SLEEP_DISABLED()  /* for API backward compatibility*/
#else
#define __HAL_RCC_DCMI_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DCMILPEN))    == 0U)
#endif /* DCMI && PSSI */
#if defined(CRYP)
#define __HAL_RCC_CRYP_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB2LPENR & (RCC_AHB2LPENR_CRYPLPEN))    == 0U)
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_HASH_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB2LPENR & (RCC_AHB2LPENR_HASHLPEN))    == 0U)
#endif /* HASH */
#define __HAL_RCC_RNG_IS_CLK_SLEEP_DISABLED()              ((RCC->AHB2LPENR & (RCC_AHB2LPENR_RNGLPEN))     == 0U)
#if defined(RCC_AHB2LPENR_DFSDMDMALPEN)
#define __HAL_RCC_DFSDMDMA_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DFSDMDMALPEN)) == 0U)
#endif
#define __HAL_RCC_SDMMC2_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB2LPENR & (RCC_AHB2LPENR_SDMMC2LPEN))  == 0U)
#if defined(RCC_AHB2LPENR_D2SRAM1LPEN)
#define __HAL_RCC_D2SRAM1_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB2LPENR & (RCC_AHB2LPENR_D2SRAM1LPEN)) == 0U)
#else
#define __HAL_RCC_AHBSRAM1_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB2LPENR & (RCC_AHB2LPENR_AHBSRAM1LPEN)) == 0U)
#endif /* RCC_AHB2LPENR_D2SRAM1LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM2LPEN)
#define __HAL_RCC_D2SRAM2_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB2LPENR & (RCC_AHB2LPENR_D2SRAM2LPEN)) == 0U)
#else
#define __HAL_RCC_AHBSRAM2_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB2LPENR & (RCC_AHB2LPENR_AHBSRAM2LPEN)) == 0U)
#endif /* RCC_AHB2LPENR_D2SRAM2LPEN */
#if defined(RCC_AHB2LPENR_D2SRAM3LPEN)
#define __HAL_RCC_D2SRAM3_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB2LPENR & (RCC_AHB2LPENR_D2SRAM3LPEN)) == 0U)
#endif /* RCC_AHB2LPENR_D2SRAM1LPEN*/


/** @brief  ENABLE or disable the AHB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_GPIOA_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOALPEN)
#define __HAL_RCC_GPIOB_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOBLPEN)
#define __HAL_RCC_GPIOC_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOCLPEN)
#define __HAL_RCC_GPIOD_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIODLPEN)
#define __HAL_RCC_GPIOE_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOELPEN)
#define __HAL_RCC_GPIOF_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOFLPEN)
#define __HAL_RCC_GPIOG_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOGLPEN)
#define __HAL_RCC_GPIOH_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOHLPEN)
#define __HAL_RCC_GPIOI_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOILPEN)
#define __HAL_RCC_GPIOJ_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOJLPEN)
#define __HAL_RCC_GPIOK_CLK_SLEEP_ENABLE()           (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOKLPEN)
#if defined(RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_CRC_CLK_SLEEP_ENABLE()             (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_CRCLPEN)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_CLK_SLEEP_ENABLE()            (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_BDMA2LPEN)
#define __HAL_RCC_BDMA_CLK_SLEEP_ENABLE  __HAL_RCC_BDMA2_CLK_SLEEP_ENABLE /* for API backward compatibility*/
#else
#define __HAL_RCC_BDMA_CLK_SLEEP_ENABLE()            (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_BDMALPEN)
#endif /* BDMA2 */
#if defined(ADC3)
#define __HAL_RCC_ADC3_CLK_SLEEP_ENABLE()            (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_ADC3LPEN)
#endif /* ADC3 */
#define __HAL_RCC_BKPRAM_CLK_SLEEP_ENABLE()          (RCC->AHB4LPENR) |= (RCC_AHB4LPENR_BKPRAMLPEN)
#if defined(RCC_AHB4LPENR_SRDSRAMLPEN)
#define __HAL_RCC_SRDSRAM_CLK_SLEEP_ENABLE()         (RCC->AHB4LPENR  |= (RCC_AHB4LPENR_SRDSRAMLPEN))
#define __HAL_RCC_D3SRAM1_CLK_SLEEP_ENABLE  __HAL_RCC_SRDSRAM_CLK_SLEEP_ENABLE /* for API backward compatibility*/
#else
#define __HAL_RCC_D3SRAM1_CLK_SLEEP_ENABLE()         (RCC->AHB4LPENR  |= (RCC_AHB4LPENR_D3SRAM1LPEN))
#endif /* RCC_AHB4LPENR_SRDSRAMLPEN */

#define __HAL_RCC_GPIOA_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOALPEN)
#define __HAL_RCC_GPIOB_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOBLPEN)
#define __HAL_RCC_GPIOC_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOCLPEN)
#define __HAL_RCC_GPIOD_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIODLPEN)
#define __HAL_RCC_GPIOE_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOELPEN)
#define __HAL_RCC_GPIOF_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOFLPEN)
#define __HAL_RCC_GPIOG_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOGLPEN)
#define __HAL_RCC_GPIOH_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOHLPEN)
#define __HAL_RCC_GPIOI_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOILPEN)
#define __HAL_RCC_GPIOJ_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOJLPEN)
#define __HAL_RCC_GPIOK_CLK_SLEEP_DISABLE()           (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOKLPEN)
#if defined(RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_CRC_CLK_SLEEP_DISABLE()             (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_CRCLPEN)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_CLK_SLEEP_DISABLE()            (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BDMA2LPEN)
#define __HAL_RCC_BDMA_CLK_SLEEP_DISABLE  __HAL_RCC_BDMA2_CLK_SLEEP_DISABLE  /* For API backward compatibility*/
#else
#define __HAL_RCC_BDMA_CLK_SLEEP_DISABLE()            (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BDMALPEN)
#endif /*BDMA2*/
#if defined(ADC3)
#define __HAL_RCC_ADC3_CLK_SLEEP_DISABLE()            (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_ADC3LPEN)
#endif /*ADC3*/
#define __HAL_RCC_BKPRAM_CLK_SLEEP_DISABLE()          (RCC->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BKPRAMLPEN)
#if defined(RCC_AHB4LPENR_SRDSRAMLPEN)
#define __HAL_RCC_SRDSRAM_CLK_SLEEP_DISABLE()         (RCC->AHB4LPENR  &= ~ (RCC_AHB4LPENR_SRDSRAMLPEN))
#define __HAL_RCC_D3SRAM1_CLK_SLEEP_DISABLE  __HAL_RCC_SRDSRAM_CLK_SLEEP_DISABLE
#else
#define __HAL_RCC_D3SRAM1_CLK_SLEEP_DISABLE()         (RCC->AHB4LPENR  &= ~ (RCC_AHB4LPENR_D3SRAM1LPEN))
#endif


/** @brief  Get the enable or disable status of the AHB4 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOALPEN))   != 0U)
#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOBLPEN))   != 0U)
#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOCLPEN))   != 0U)
#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIODLPEN))   != 0U)
#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOELPEN))   != 0U)
#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOFLPEN))   != 0U)
#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOGLPEN))   != 0U)
#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOHLPEN))   != 0U)
#define __HAL_RCC_GPIOI_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOILPEN))   != 0U)
#define __HAL_RCC_GPIOJ_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOJLPEN))   != 0U)
#define __HAL_RCC_GPIOK_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOKLPEN))   != 0U)
#if defined(RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_CRC_IS_CLK_SLEEP_ENABLED()              ((RCC->AHB4LPENR & (RCC_AHB4LPENR_CRCLPEN))     != 0U)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_IS_CLK_SLEEP_ENABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_BDMA2LPEN))    != 0U)
#define __HAL_RCC_BDMA_IS_CLK_SLEEP_ENABLED __HAL_RCC_BDMA2_IS_CLK_SLEEP_ENABLED  /* For API backward compatibility*/
#else
#define __HAL_RCC_BDMA_IS_CLK_SLEEP_ENABLED()             ((RCC->AHB4LPENR & (RCC_AHB4LPENR_BDMALPEN))    != 0U)
#endif /*BDMA2*/
#if defined(ADC3)
#define __HAL_RCC_ADC3_IS_CLK_SLEEP_ENABLED()             ((RCC->AHB4LPENR & (RCC_AHB4LPENR_ADC3LPEN))    != 0U)
#endif /*ADC3*/
#define __HAL_RCC_BKPRAM_IS_CLK_SLEEP_ENABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_BKPRAMLPEN))  != 0U)
#if defined(RCC_AHB4LPENR_SRDSRAMLPEN)
#define __HAL_RCC_SRDSRAM_IS_CLK_SLEEP_ENABLED()          ((RCC->AHB4LPENR & (RCC_AHB4LPENR_SRDSRAMLPEN)) != 0U)
#define __HAL_RCC_D3SRAM1_IS_CLK_SLEEP_ENABLED  __HAL_RCC_SRDSRAM_IS_CLK_SLEEP_ENABLED  /* For API backward compatibility*/
#else
#define __HAL_RCC_D3SRAM1_IS_CLK_SLEEP_ENABLED()          ((RCC->AHB4LPENR & (RCC_AHB4LPENR_D3SRAM1LPEN)) != 0U)
#endif

#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOALPEN))   == 0U)
#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOBLPEN))   == 0U)
#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOCLPEN))   == 0U)
#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIODLPEN))   == 0U)
#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOELPEN))   == 0U)
#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOFLPEN))   == 0U)
#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOGLPEN))   == 0U)
#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOHLPEN))   == 0U)
#define __HAL_RCC_GPIOI_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOILPEN))   == 0U)
#define __HAL_RCC_GPIOJ_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOJLPEN))   == 0U)
#define __HAL_RCC_GPIOK_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_GPIOKLPEN))   == 0U)
#if defined(RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_CRC_IS_CLK_SLEEP_DISABLED()             ((RCC->AHB4LPENR & (RCC_AHB4LPENR_CRCLPEN))     == 0U)
#endif
#if defined(BDMA2)
#define __HAL_RCC_BDMA2_IS_CLK_SLEEP_DISABLED()           ((RCC->AHB4LPENR & (RCC_AHB4LPENR_BDMA2LPEN))  == 0U)
#define __HAL_RCC_BDMA_IS_CLK_SLEEP_DISABLED __HAL_RCC_BDMA2_IS_CLK_SLEEP_DISABLED  /* For API backward compatibility*/
#else
#define __HAL_RCC_BDMA_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_BDMALPEN))    == 0U)
#endif /*BDMA2*/
#if defined(ADC3)
#define __HAL_RCC_ADC3_IS_CLK_SLEEP_DISABLED()            ((RCC->AHB4LPENR & (RCC_AHB4LPENR_ADC3LPEN))    == 0U)
#endif /*ADC3*/
#define __HAL_RCC_BKPRAM_IS_CLK_SLEEP_DISABLED()          ((RCC->AHB4LPENR & (RCC_AHB4LPENR_BKPRAMLPEN))  == 0U)
#if defined(RCC_AHB4LPENR_SRDSRAMLPEN)
#define __HAL_RCC_SRDSRAM_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB4LPENR & (RCC_AHB4LPENR_SRDSRAMLPEN)) == 0U)
#define __HAL_RCC_D3SRAM1_IS_CLK_SLEEP_DISABLED  __HAL_RCC_SRDSRAM_IS_CLK_SLEEP_DISABLED  /* For API backward compatibility*/
#else
#define __HAL_RCC_D3SRAM1_IS_CLK_SLEEP_DISABLED()         ((RCC->AHB4LPENR & (RCC_AHB4LPENR_D3SRAM1LPEN)) == 0U)
#endif


/** @brief  ENABLE or disable the APB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#if defined(LTDC)
#define __HAL_RCC_LTDC_CLK_SLEEP_ENABLE()           (RCC->APB3LPENR) |= (RCC_APB3LPENR_LTDCLPEN)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_CLK_SLEEP_ENABLE()            (RCC->APB3LPENR) |= (RCC_APB3LPENR_DSILPEN)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_CLK_SLEEP_ENABLE()          (RCC->APB3LPENR) |= (RCC_APB3LPENR_WWDG1LPEN)

#if defined(LTDC)
#define __HAL_RCC_LTDC_CLK_SLEEP_DISABLE()           (RCC->APB3LPENR) &= ~ (RCC_APB3LPENR_LTDCLPEN)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_CLK_SLEEP_DISABLE()            (RCC->APB3LPENR) &= ~ (RCC_APB3LPENR_DSILPEN)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_CLK_SLEEP_DISABLE()          (RCC->APB3LPENR) &= ~ (RCC_APB3LPENR_WWDG1LPEN)


/** @brief  Get the enable or disable status of the APB3 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#if defined(LTDC)
#define __HAL_RCC_LTDC_IS_CLK_SLEEP_ENABLED()            ((RCC->APB3LPENR & (RCC_APB3LPENR_LTDCLPEN))  != 0U)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_IS_CLK_SLEEP_ENABLED()             ((RCC->APB3LPENR & (RCC_APB3LPENR_DSILPEN))   != 0U)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_IS_CLK_SLEEP_ENABLED()           ((RCC->APB3LPENR & (RCC_APB3LPENR_WWDG1LPEN)) != 0U)

#if defined(LTDC)
#define __HAL_RCC_LTDC_IS_CLK_SLEEP_DISABLED()           ((RCC->APB3LPENR & (RCC_APB3LPENR_LTDCLPEN))  == 0U)
#endif /* LTDC */
#if defined(DSI)
#define __HAL_RCC_DSI_IS_CLK_SLEEP_DISABLED()            ((RCC->APB3LPENR & (RCC_APB3LPENR_DSILPEN))   == 0U)
#endif /*DSI*/
#define __HAL_RCC_WWDG1_IS_CLK_SLEEP_DISABLED()          ((RCC->APB3LPENR & (RCC_APB3LPENR_WWDG1LPEN)) == 0U)


/** @brief  ENABLE or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_TIM2_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM2LPEN)
#define __HAL_RCC_TIM3_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM3LPEN)
#define __HAL_RCC_TIM4_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM4LPEN)
#define __HAL_RCC_TIM5_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM5LPEN)
#define __HAL_RCC_TIM6_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM6LPEN)
#define __HAL_RCC_TIM7_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM7LPEN)
#define __HAL_RCC_TIM12_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM12LPEN)
#define __HAL_RCC_TIM13_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM13LPEN)
#define __HAL_RCC_TIM14_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_TIM14LPEN)
#define __HAL_RCC_LPTIM1_CLK_SLEEP_ENABLE()         (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_LPTIM1LPEN)

#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_WWDG2LPEN)
#endif /*DUAL_CORE*/

#define __HAL_RCC_SPI2_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_SPI2LPEN)
#define __HAL_RCC_SPI3_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_SPI3LPEN)
#define __HAL_RCC_SPDIFRX_CLK_SLEEP_ENABLE()        (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_SPDIFRXLPEN)
#define __HAL_RCC_USART2_CLK_SLEEP_ENABLE()         (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_USART2LPEN)
#define __HAL_RCC_USART3_CLK_SLEEP_ENABLE()         (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_USART3LPEN)
#define __HAL_RCC_UART4_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_UART4LPEN)
#define __HAL_RCC_UART5_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_UART5LPEN)
#define __HAL_RCC_I2C1_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_I2C1LPEN)
#define __HAL_RCC_I2C2_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_I2C2LPEN)
#define __HAL_RCC_I2C3_CLK_SLEEP_ENABLE()           (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_I2C3LPEN)
#define __HAL_RCC_CEC_CLK_SLEEP_ENABLE()            (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_CECLPEN)
#define __HAL_RCC_DAC12_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_DAC12LPEN)
#define __HAL_RCC_UART7_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_UART7LPEN)
#define __HAL_RCC_UART8_CLK_SLEEP_ENABLE()          (RCC->APB1LLPENR) |= (RCC_APB1LLPENR_UART8LPEN)
#define __HAL_RCC_CRS_CLK_SLEEP_ENABLE()            (RCC->APB1HLPENR) |= (RCC_APB1HLPENR_CRSLPEN)
#define __HAL_RCC_SWPMI1_CLK_SLEEP_ENABLE()          (RCC->APB1HLPENR) |= (RCC_APB1HLPENR_SWPMILPEN)
#define __HAL_RCC_OPAMP_CLK_SLEEP_ENABLE()          (RCC->APB1HLPENR) |= (RCC_APB1HLPENR_OPAMPLPEN)
#define __HAL_RCC_MDIOS_CLK_SLEEP_ENABLE()          (RCC->APB1HLPENR) |= (RCC_APB1HLPENR_MDIOSLPEN)
#define __HAL_RCC_FDCAN_CLK_SLEEP_ENABLE()          (RCC->APB1HLPENR) |= (RCC_APB1HLPENR_FDCANLPEN)


#define __HAL_RCC_TIM2_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM2LPEN)
#define __HAL_RCC_TIM3_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM3LPEN)
#define __HAL_RCC_TIM4_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM4LPEN)
#define __HAL_RCC_TIM5_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM5LPEN)
#define __HAL_RCC_TIM6_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM6LPEN)
#define __HAL_RCC_TIM7_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM7LPEN)
#define __HAL_RCC_TIM12_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM12LPEN)
#define __HAL_RCC_TIM13_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM13LPEN)
#define __HAL_RCC_TIM14_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM14LPEN)
#define __HAL_RCC_LPTIM1_CLK_SLEEP_DISABLE()         (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_LPTIM1LPEN)

#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_WWDG2LPEN)
#endif /*DUAL_CORE*/

#define __HAL_RCC_SPI2_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPI2LPEN)
#define __HAL_RCC_SPI3_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPI3LPEN)
#define __HAL_RCC_SPDIFRX_CLK_SLEEP_DISABLE()        (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPDIFRXLPEN)
#define __HAL_RCC_USART2_CLK_SLEEP_DISABLE()         (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_USART2LPEN)
#define __HAL_RCC_USART3_CLK_SLEEP_DISABLE()         (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_USART3LPEN)
#define __HAL_RCC_UART4_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART4LPEN)
#define __HAL_RCC_UART5_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART5LPEN)
#define __HAL_RCC_I2C1_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C1LPEN)
#define __HAL_RCC_I2C2_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C2LPEN)
#define __HAL_RCC_I2C3_CLK_SLEEP_DISABLE()           (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C3LPEN)
#define __HAL_RCC_CEC_CLK_SLEEP_DISABLE()            (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_CECLPEN)
#define __HAL_RCC_DAC12_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_DAC12LPEN)
#define __HAL_RCC_UART7_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART7LPEN)
#define __HAL_RCC_UART8_CLK_SLEEP_DISABLE()          (RCC->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART8LPEN)
#define __HAL_RCC_CRS_CLK_SLEEP_DISABLE()            (RCC->APB1HLPENR) &= ~ (RCC_APB1HLPENR_CRSLPEN)
#define __HAL_RCC_SWPMI1_CLK_SLEEP_DISABLE()          (RCC->APB1HLPENR) &= ~ (RCC_APB1HLPENR_SWPMILPEN)
#define __HAL_RCC_OPAMP_CLK_SLEEP_DISABLE()          (RCC->APB1HLPENR) &= ~ (RCC_APB1HLPENR_OPAMPLPEN)
#define __HAL_RCC_MDIOS_CLK_SLEEP_DISABLE()          (RCC->APB1HLPENR) &= ~ (RCC_APB1HLPENR_MDIOSLPEN)
#define __HAL_RCC_FDCAN_CLK_SLEEP_DISABLE()          (RCC->APB1HLPENR) &= ~ (RCC_APB1HLPENR_FDCANLPEN)


/** @brief  Get the enable or disable status of the APB1 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#define __HAL_RCC_TIM2_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM2LPEN))    != 0U)
#define __HAL_RCC_TIM3_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM3LPEN))    != 0U)
#define __HAL_RCC_TIM4_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM4LPEN))    != 0U)
#define __HAL_RCC_TIM5_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM5LPEN))    != 0U)
#define __HAL_RCC_TIM6_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM6LPEN))    != 0U)
#define __HAL_RCC_TIM7_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM7LPEN))    != 0U)
#define __HAL_RCC_TIM12_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM12LPEN))   != 0U)
#define __HAL_RCC_TIM13_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM13LPEN))   != 0U)
#define __HAL_RCC_TIM14_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM14LPEN))   != 0U)
#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_ENABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_LPTIM1LPEN))  != 0U)
#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_WWDG2LPEN))   != 0U)
#endif /*DUAL_CORE*/
#define __HAL_RCC_SPI2_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_SPI2LPEN))    != 0U)
#define __HAL_RCC_SPI3_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_SPI3LPEN))    != 0U)
#define __HAL_RCC_SPDIFRX_IS_CLK_SLEEP_ENABLED()         ((RCC->APB1LLPENR & (RCC_APB1LLPENR_SPDIFRXLPEN)) != 0U)
#define __HAL_RCC_USART2_IS_CLK_SLEEP_ENABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_USART2LPEN))  != 0U)
#define __HAL_RCC_USART3_IS_CLK_SLEEP_ENABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_USART3LPEN))  != 0U)
#define __HAL_RCC_UART4_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART4LPEN))   != 0U)
#define __HAL_RCC_UART5_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART5LPEN))   != 0U)
#define __HAL_RCC_I2C1_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_I2C1LPEN))    != 0U)
#define __HAL_RCC_I2C2_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_I2C2LPEN))    != 0U)
#define __HAL_RCC_I2C3_IS_CLK_SLEEP_ENABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_I2C3LPEN))    != 0U)
#define __HAL_RCC_CEC_IS_CLK_SLEEP_ENABLED()             ((RCC->APB1LLPENR & (RCC_APB1LLPENR_CECLPEN))     != 0U)
#define __HAL_RCC_DAC12_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_DAC12LPEN))   != 0U)
#define __HAL_RCC_UART7_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART7LPEN))   != 0U)
#define __HAL_RCC_UART8_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART8LPEN))   != 0U)
#define __HAL_RCC_CRS_IS_CLK_SLEEP_ENABLED()             ((RCC->APB1HLPENR & (RCC_APB1HLPENR_CRSLPEN))     != 0U)
#define __HAL_RCC_SWPMI1_IS_CLK_SLEEP_ENABLED()          ((RCC->APB1HLPENR & (RCC_APB1HLPENR_SWPMILPEN))   != 0U)
#define __HAL_RCC_OPAMP_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1HLPENR & (RCC_APB1HLPENR_OPAMPLPEN))   != 0U)
#define __HAL_RCC_MDIOS_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1HLPENR & (RCC_APB1HLPENR_MDIOSLPEN))   != 0U)
#define __HAL_RCC_FDCAN_IS_CLK_SLEEP_ENABLED()           ((RCC->APB1HLPENR & (RCC_APB1HLPENR_FDCANLPEN))   != 0U)

#define __HAL_RCC_TIM2_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM2LPEN))    == 0U)
#define __HAL_RCC_TIM3_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM3LPEN))    == 0U)
#define __HAL_RCC_TIM4_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM4LPEN))    == 0U)
#define __HAL_RCC_TIM5_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM5LPEN))    == 0U)
#define __HAL_RCC_TIM6_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM6LPEN))    == 0U)
#define __HAL_RCC_TIM7_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM7LPEN))    == 0U)
#define __HAL_RCC_TIM12_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM12LPEN))   == 0U)
#define __HAL_RCC_TIM13_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM13LPEN))   == 0U)
#define __HAL_RCC_TIM14_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_TIM14LPEN))   == 0U)
#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_DISABLED()         ((RCC->APB1LLPENR & (RCC_APB1LLPENR_LPTIM1LPEN))  == 0U)
#if defined(DUAL_CORE)
#define __HAL_RCC_WWDG2_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_WWDG2LPEN))   == 0U)
#endif /*DUAL_CORE*/
#define __HAL_RCC_SPI2_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_SPI2LPEN))    == 0U)
#define __HAL_RCC_SPI3_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_SPI3LPEN))    == 0U)
#define __HAL_RCC_SPDIFRX_IS_CLK_SLEEP_DISABLED()        ((RCC->APB1LLPENR & (RCC_APB1LLPENR_SPDIFRXLPEN)) == 0U)
#define __HAL_RCC_USART2_IS_CLK_SLEEP_DISABLED()         ((RCC->APB1LLPENR & (RCC_APB1LLPENR_USART2LPEN))  == 0U)
#define __HAL_RCC_USART3_IS_CLK_SLEEP_DISABLED()         ((RCC->APB1LLPENR & (RCC_APB1LLPENR_USART3LPEN))  == 0U)
#define __HAL_RCC_UART4_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART4LPEN))   == 0U)
#define __HAL_RCC_UART5_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART5LPEN))   == 0U)
#define __HAL_RCC_I2C1_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_I2C1LPEN))    == 0U)
#define __HAL_RCC_I2C2_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_I2C2LPEN))    == 0U)
#define __HAL_RCC_I2C3_IS_CLK_SLEEP_DISABLED()           ((RCC->APB1LLPENR & (RCC_APB1LLPENR_I2C3LPEN))    == 0U)
#define __HAL_RCC_CEC_IS_CLK_SLEEP_DISABLED()            ((RCC->APB1LLPENR & (RCC_APB1LLPENR_CECLPEN))     == 0U)
#define __HAL_RCC_DAC12_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_DAC12LPEN))   == 0U)
#define __HAL_RCC_UART7_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART7LPEN))   == 0U)
#define __HAL_RCC_UART8_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1LLPENR & (RCC_APB1LLPENR_UART8LPEN))   == 0U)
#define __HAL_RCC_CRS_IS_CLK_SLEEP_DISABLED()            ((RCC->APB1HLPENR & (RCC_APB1HLPENR_CRSLPEN))     == 0U)
#define __HAL_RCC_SWPMI1_IS_CLK_SLEEP_DISABLED()         ((RCC->APB1HLPENR & (RCC_APB1HLPENR_SWPMILPEN))   == 0U)
#define __HAL_RCC_OPAMP_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1HLPENR & (RCC_APB1HLPENR_OPAMPLPEN))   == 0U)
#define __HAL_RCC_MDIOS_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1HLPENR & (RCC_APB1HLPENR_MDIOSLPEN))   == 0U)
#define __HAL_RCC_FDCAN_IS_CLK_SLEEP_DISABLED()          ((RCC->APB1HLPENR & (RCC_APB1HLPENR_FDCANLPEN))   == 0U)


/** @brief  ENABLE or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_TIM1_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_TIM1LPEN)
#define __HAL_RCC_TIM8_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_TIM8LPEN)
#define __HAL_RCC_USART1_CLK_SLEEP_ENABLE()         (RCC->APB2LPENR) |= (RCC_APB2LPENR_USART1LPEN)
#define __HAL_RCC_USART6_CLK_SLEEP_ENABLE()         (RCC->APB2LPENR) |= (RCC_APB2LPENR_USART6LPEN)
#if defined(UART9)
#define __HAL_RCC_UART9_CLK_SLEEP_ENABLE()          (RCC->APB2LPENR) |= (RCC_APB2LPENR_UART9LPEN)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_CLK_SLEEP_ENABLE()        (RCC->APB2LPENR) |= (RCC_APB2LPENR_USART10LPEN)
#endif /*USART10*/
#define __HAL_RCC_SPI1_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_SPI1LPEN)
#define __HAL_RCC_SPI4_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_SPI4LPEN)
#define __HAL_RCC_TIM15_CLK_SLEEP_ENABLE()          (RCC->APB2LPENR) |= (RCC_APB2LPENR_TIM15LPEN)
#define __HAL_RCC_TIM16_CLK_SLEEP_ENABLE()          (RCC->APB2LPENR) |= (RCC_APB2LPENR_TIM16LPEN)
#define __HAL_RCC_TIM17_CLK_SLEEP_ENABLE()          (RCC->APB2LPENR) |= (RCC_APB2LPENR_TIM17LPEN)
#define __HAL_RCC_SPI5_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_SPI5LPEN)
#define __HAL_RCC_SAI1_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_SAI1LPEN)
#define __HAL_RCC_SAI2_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_SAI2LPEN)
#if defined(SAI3)
#define __HAL_RCC_SAI3_CLK_SLEEP_ENABLE()           (RCC->APB2LPENR) |= (RCC_APB2LPENR_SAI3LPEN)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_CLK_SLEEP_ENABLE()         (RCC->APB2LPENR) |= (RCC_APB2LPENR_DFSDM1LPEN)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_CLK_SLEEP_ENABLE()         (RCC->APB2LPENR) |= (RCC_APB2LPENR_HRTIMLPEN)
#endif /*HRTIM1*/

#define __HAL_RCC_TIM1_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM1LPEN)
#define __HAL_RCC_TIM8_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM8LPEN)
#define __HAL_RCC_USART1_CLK_SLEEP_DISABLE()         (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_USART1LPEN)
#define __HAL_RCC_USART6_CLK_SLEEP_DISABLE()         (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_USART6LPEN)
#if defined(UART9)
#define __HAL_RCC_UART9_CLK_SLEEP_DISABLE()          (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_UART9LPEN)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_CLK_SLEEP_DISABLE()         (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_USART10LPEN)
#endif /*USART10*/
#define __HAL_RCC_SPI1_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI1LPEN)
#define __HAL_RCC_SPI4_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI4LPEN)
#define __HAL_RCC_TIM15_CLK_SLEEP_DISABLE()          (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM15LPEN)
#define __HAL_RCC_TIM16_CLK_SLEEP_DISABLE()          (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM16LPEN)
#define __HAL_RCC_TIM17_CLK_SLEEP_DISABLE()          (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM17LPEN)
#define __HAL_RCC_SPI5_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI5LPEN)
#define __HAL_RCC_SAI1_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI1LPEN)
#define __HAL_RCC_SAI2_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI2LPEN)
#if defined(SAI3)
#define __HAL_RCC_SAI3_CLK_SLEEP_DISABLE()           (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI3LPEN)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_CLK_SLEEP_DISABLE()         (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_DFSDM1LPEN)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_CLK_SLEEP_DISABLE()         (RCC->APB2LPENR) &= ~ (RCC_APB2LPENR_HRTIMLPEN)
#endif /*HRTIM1*/


/** @brief  Get the enable or disable status of the APB2 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#define __HAL_RCC_TIM1_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM1LPEN))   != 0U)
#define __HAL_RCC_TIM8_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM8LPEN))   != 0U)
#define __HAL_RCC_USART1_IS_CLK_SLEEP_ENABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_USART1LPEN)) != 0U)
#define __HAL_RCC_USART6_IS_CLK_SLEEP_ENABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_USART6LPEN)) != 0U)
#if defined(UART9)
#define __HAL_RCC_UART9_IS_CLK_SLEEP_ENABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_UART9LPEN)) != 0U)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_IS_CLK_SLEEP_ENABLED()         ((RCC->APB2LPENR & (RCC_APB2LPENR_USART10LPEN)) != 0U)
#endif /*USART10*/
#define __HAL_RCC_SPI1_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI1LPEN))   != 0U)
#define __HAL_RCC_SPI4_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI4LPEN))   != 0U)
#define __HAL_RCC_TIM15_IS_CLK_SLEEP_ENABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM15LPEN))  != 0U)
#define __HAL_RCC_TIM16_IS_CLK_SLEEP_ENABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM16LPEN))  != 0U)
#define __HAL_RCC_TIM17_IS_CLK_SLEEP_ENABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM17LPEN))  != 0U)
#define __HAL_RCC_SPI5_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI5LPEN))   != 0U)
#define __HAL_RCC_SAI1_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI1LPEN))   != 0U)
#define __HAL_RCC_SAI2_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI2LPEN))   != 0U)
#if defined(SAI3)
#define __HAL_RCC_SAI3_IS_CLK_SLEEP_ENABLED()            ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI3LPEN))   != 0U)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_IS_CLK_SLEEP_ENABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_DFSDM1LPEN)) != 0U)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_IS_CLK_SLEEP_ENABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_HRTIMLPEN))  != 0U)
#endif /*HRTIM1*/

#define __HAL_RCC_TIM1_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM1LPEN))   == 0U)
#define __HAL_RCC_TIM8_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM8LPEN))   == 0U)
#define __HAL_RCC_USART1_IS_CLK_SLEEP_DISABLED()         ((RCC->APB2LPENR & (RCC_APB2LPENR_USART1LPEN)) == 0U)
#define __HAL_RCC_USART6_IS_CLK_SLEEP_DISABLED()         ((RCC->APB2LPENR & (RCC_APB2LPENR_USART6LPEN)) == 0U)
#if defined(UART9)
#define __HAL_RCC_USART9_IS_CLK_SLEEP_DISABLED()         ((RCC->APB2LPENR & (RCC_APB2LPENR_USART9LPEN)) == 0U)
#endif /*UART9*/
#if defined(USART10)
#define __HAL_RCC_USART10_IS_CLK_SLEEP_DISABLED()        ((RCC->APB2LPENR & (RCC_APB2LPENR_USART10LPEN)) == 0U)
#endif /*USART10*/
#define __HAL_RCC_SPI1_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI1LPEN))   == 0U)
#define __HAL_RCC_SPI4_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI4LPEN))   == 0U)
#define __HAL_RCC_TIM15_IS_CLK_SLEEP_DISABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM15LPEN))  == 0U)
#define __HAL_RCC_TIM16_IS_CLK_SLEEP_DISABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM16LPEN))  == 0U)
#define __HAL_RCC_TIM17_IS_CLK_SLEEP_DISABLED()          ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM17LPEN))  == 0U)
#define __HAL_RCC_SPI5_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI5LPEN))   == 0U)
#define __HAL_RCC_SAI1_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI1LPEN))   == 0U)
#define __HAL_RCC_SAI2_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI2LPEN))   == 0U)
#if defined(SAI3)
#define __HAL_RCC_SAI3_IS_CLK_SLEEP_DISABLED()           ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI3LPEN))   == 0U)
#endif /*SAI3*/
#define __HAL_RCC_DFSDM1_IS_CLK_SLEEP_DISABLED()         ((RCC->APB2LPENR & (RCC_APB2LPENR_DFSDM1LPEN)) == 0U)
#if defined(HRTIM1)
#define __HAL_RCC_HRTIM1_IS_CLK_SLEEP_DISABLED()         ((RCC->APB2LPENR & (RCC_APB2LPENR_HRTIMLPEN))  == 0U)
#endif /*HRTIM1*/

/** @brief  ENABLE or disable the APB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_SYSCFG_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_SYSCFGLPEN)
#define __HAL_RCC_LPUART1_CLK_SLEEP_ENABLE()          (RCC->APB4LPENR) |= (RCC_APB4LPENR_LPUART1LPEN)
#define __HAL_RCC_SPI6_CLK_SLEEP_ENABLE()             (RCC->APB4LPENR) |= (RCC_APB4LPENR_SPI6LPEN)
#define __HAL_RCC_I2C4_CLK_SLEEP_ENABLE()             (RCC->APB4LPENR) |= (RCC_APB4LPENR_I2C4LPEN)
#define __HAL_RCC_LPTIM2_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_LPTIM2LPEN)
#define __HAL_RCC_LPTIM3_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_LPTIM3LPEN)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_LPTIM4LPEN)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_LPTIM5LPEN)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_DAC2LPEN)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_COMP12LPEN)
#define __HAL_RCC_VREF_CLK_SLEEP_ENABLE()             (RCC->APB4LPENR) |= (RCC_APB4LPENR_VREFLPEN)
#define __HAL_RCC_RTC_CLK_SLEEP_ENABLE()              (RCC->APB4LPENR) |= (RCC_APB4LPENR_RTCAPBLPEN)
#if defined(SAI4)
#define __HAL_RCC_SAI4_CLK_SLEEP_ENABLE()             (RCC->APB4LPENR) |= (RCC_APB4LPENR_SAI4LPEN)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_CLK_SLEEP_ENABLE()              (RCC->APB4LPENR) |= (RCC_APB4LPENR_DTSLPEN)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_CLK_SLEEP_ENABLE()           (RCC->APB4LPENR) |= (RCC_APB4LPENR_DFSDM2LPEN)
#endif /*DFSDM2*/

#define __HAL_RCC_SYSCFG_CLK_SLEEP_DISABLE()           (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_SYSCFGLPEN)
#define __HAL_RCC_LPUART1_CLK_SLEEP_DISABLE()          (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_LPUART1LPEN)
#define __HAL_RCC_SPI6_CLK_SLEEP_DISABLE()             (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_SPI6LPEN)
#define __HAL_RCC_I2C4_CLK_SLEEP_DISABLE()             (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_I2C4LPEN)
#define __HAL_RCC_LPTIM2_CLK_SLEEP_DISABLE()           (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM2LPEN)
#define __HAL_RCC_LPTIM3_CLK_SLEEP_DISABLE()           (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM3LPEN)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_CLK_SLEEP_DISABLE()           (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM4LPEN)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_CLK_SLEEP_DISABLE()           (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM5LPEN)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_CLK_SLEEP_DISABLE()             (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_DAC2LPEN)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_CLK_SLEEP_DISABLE()           (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_COMP12LPEN)
#define __HAL_RCC_VREF_CLK_SLEEP_DISABLE()             (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_VREFLPEN)
#define __HAL_RCC_RTC_CLK_SLEEP_DISABLE()              (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_RTCAPBLPEN)
#if defined(SAI4)
#define __HAL_RCC_SAI4_CLK_SLEEP_DISABLE()             (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_SAI4LPEN)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_CLK_SLEEP_DISABLE()              (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_DTSLPEN)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_CLK_SLEEP_DISABLE()          (RCC->APB4LPENR) &= ~ (RCC_APB4LPENR_DFSDM2LPEN)
#endif /*DFSDM2*/


/** @brief  Get the enable or disable status of the APB4 peripheral clock during Low Poser (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#define __HAL_RCC_SYSCFG_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_SYSCFGLPEN))  != 0U)
#define __HAL_RCC_LPUART1_IS_CLK_SLEEP_ENABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_LPUART1LPEN)) != 0U)
#define __HAL_RCC_SPI6_IS_CLK_SLEEP_ENABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_SPI6LPEN))    != 0U)
#define __HAL_RCC_I2C4_IS_CLK_SLEEP_ENABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_I2C4LPEN))    != 0U)
#define __HAL_RCC_LPTIM2_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM2LPEN))  != 0U)
#define __HAL_RCC_LPTIM3_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM3LPEN))  != 0U)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM4LPEN))  != 0U)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM5LPEN))  != 0U)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_IS_CLK_SLEEP_ENABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_DAC2LPEN))  != 0U)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_COMP12LPEN))  != 0U)
#define __HAL_RCC_VREF_IS_CLK_SLEEP_ENABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_VREFLPEN))    != 0U)
#define __HAL_RCC_RTC_IS_CLK_SLEEP_ENABLED()               ((RCC->APB4LPENR & (RCC_APB4LPENR_RTCAPBLPEN))  != 0U)
#if defined(SAI4)
#define __HAL_RCC_SAI4_IS_CLK_SLEEP_ENABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_SAI4LPEN))    != 0U)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_IS_CLK_SLEEP_ENABLED()               ((RCC->APB4LPENR & (RCC_APB4LPENR_DTSLPEN))    != 0U)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_IS_CLK_SLEEP_ENABLED()            ((RCC->APB4LPENR & (RCC_APB4LPENR_DFSDM2LPEN)) != 0U)
#endif /*DFSDM2*/

#define __HAL_RCC_SYSCFG_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_SYSCFGLPEN))  == 0U)
#define __HAL_RCC_LPUART1_IS_CLK_SLEEP_DISABLED()          ((RCC->APB4LPENR & (RCC_APB4LPENR_LPUART1LPEN)) == 0U)
#define __HAL_RCC_SPI6_IS_CLK_SLEEP_DISABLED()             ((RCC->APB4LPENR & (RCC_APB4LPENR_SPI6LPEN))    == 0U)
#define __HAL_RCC_I2C4_IS_CLK_SLEEP_DISABLED()             ((RCC->APB4LPENR & (RCC_APB4LPENR_I2C4LPEN))    == 0U)
#define __HAL_RCC_LPTIM2_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM2LPEN))  == 0U)
#define __HAL_RCC_LPTIM3_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM3LPEN))  == 0U)
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM4LPEN))  == 0U)
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_LPTIM5LPEN))  == 0U)
#endif /*LPTIM5*/
#if defined(DAC2)
#define __HAL_RCC_DAC2_IS_CLK_SLEEP_DISABLED()             ((RCC->APB4LPENR & (RCC_APB4LPENR_DAC2LPEN))  == 0U)
#endif /*DAC2*/
#define __HAL_RCC_COMP12_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_COMP12LPEN))  == 0U)
#define __HAL_RCC_VREF_IS_CLK_SLEEP_DISABLED()             ((RCC->APB4LPENR & (RCC_APB4LPENR_VREFLPEN))    == 0U)
#define __HAL_RCC_RTC_IS_CLK_SLEEP_DISABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_RTCAPBLPEN))  == 0U)
#if defined(SAI4)
#define __HAL_RCC_SAI4_IS_CLK_SLEEP_DISABLED()             ((RCC->APB4LPENR & (RCC_APB4LPENR_SAI4LPEN))    == 0U)
#endif /*SAI4*/
#if defined(DTS)
#define __HAL_RCC_DTS_IS_CLK_SLEEP_DISABLED()              ((RCC->APB4LPENR & (RCC_APB4LPENR_DTSLPEN))    == 0U)
#endif /*DTS*/
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_IS_CLK_SLEEP_DISABLED()           ((RCC->APB4LPENR & (RCC_APB4LPENR_DFSDM2LPEN)) == 0U)
#endif /*DFSDM2*/


#if defined(DUAL_CORE)

/** @brief  Enable or disable the RCC_C1 AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_C1_MDMA_CLK_SLEEP_ENABLE()            (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_MDMALPEN))
#define __HAL_RCC_C1_DMA2D_CLK_SLEEP_ENABLE()           (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_DMA2DLPEN))
#define __HAL_RCC_C1_JPGDEC_CLK_SLEEP_ENABLE()          (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_JPGDECLPEN))
#define __HAL_RCC_C1_FLASH_CLK_SLEEP_ENABLE()           (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_FLASHLPEN))
#define __HAL_RCC_C1_FMC_CLK_SLEEP_ENABLE()             (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_FMCLPEN))
#define __HAL_RCC_C1_QSPI_CLK_SLEEP_ENABLE()            (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_QSPILPEN))
#define __HAL_RCC_C1_SDMMC1_CLK_SLEEP_ENABLE()          (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_SDMMC1LPEN))
#define __HAL_RCC_C1_DTCM1_CLK_SLEEP_ENABLE()           (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_DTCM1LPEN))
#define __HAL_RCC_C1_DTCM2_CLK_SLEEP_ENABLE()           (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_DTCM2LPEN))
#define __HAL_RCC_C1_ITCM_CLK_SLEEP_ENABLE()            (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_ITCMLPEN))
#define __HAL_RCC_C1_D1SRAM1_CLK_SLEEP_ENABLE()         (RCC_C1->AHB3LPENR |= (RCC_AHB3LPENR_AXISRAMLPEN))


#define __HAL_RCC_C1_MDMA_CLK_SLEEP_DISABLE()            (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_MDMALPEN))
#define __HAL_RCC_C1_DMA2D_CLK_SLEEP_DISABLE()           (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_DMA2DLPEN))
#define __HAL_RCC_C1_JPGDEC_CLK_SLEEP_DISABLE()          (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_JPGDECLPEN))
#define __HAL_RCC_C1_FLASH_CLK_SLEEP_DISABLE()           (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_FLASHLPEN))
#define __HAL_RCC_C1_FMC_CLK_SLEEP_DISABLE()             (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_FMCLPEN))
#define __HAL_RCC_C1_QSPI_CLK_SLEEP_DISABLE()            (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_QSPILPEN))
#define __HAL_RCC_C1_SDMMC1_CLK_SLEEP_DISABLE()          (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_SDMMC1LPEN))
#define __HAL_RCC_C1_DTCM1_CLK_SLEEP_DISABLE()           (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_DTCM1LPEN))
#define __HAL_RCC_C1_DTCM2_CLK_SLEEP_DISABLE()           (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_DTCM2LPEN))
#define __HAL_RCC_C1_ITCM_CLK_SLEEP_DISABLE()            (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_ITCMLPEN))
#define __HAL_RCC_C1_D1SRAM1_CLK_SLEEP_DISABLE()         (RCC_C1->AHB3LPENR &= ~ (RCC_AHB3LPENR_AXISRAMLPEN))



/** @brief  ENABLE or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_DMA1_CLK_SLEEP_ENABLE()             (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_DMA1LPEN))
#define __HAL_RCC_C1_DMA2_CLK_SLEEP_ENABLE()             (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_C1_ADC12_CLK_SLEEP_ENABLE()            (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_ADC12LPEN))
#define __HAL_RCC_C1_ETH1MAC_CLK_SLEEP_ENABLE()          (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_ETH1MACLPEN))
#define __HAL_RCC_C1_ETH1TX_CLK_SLEEP_ENABLE()           (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_ETH1TXLPEN))
#define __HAL_RCC_C1_ETH1RX_CLK_SLEEP_ENABLE()           (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_ETH1RXLPEN))
#define __HAL_RCC_C1_USB1_OTG_HS_CLK_SLEEP_ENABLE()      (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_USB1OTGHSLPEN))
#define __HAL_RCC_C1_USB1_OTG_HS_ULPI_CLK_SLEEP_ENABLE() (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_USB1OTGHSULPILPEN))
#define __HAL_RCC_C1_USB2_OTG_FS_CLK_SLEEP_ENABLE()      (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_USB2OTGHSLPEN))
#define __HAL_RCC_C1_USB2_OTG_FS_ULPI_CLK_SLEEP_ENABLE() (RCC_C1->AHB1LPENR |= (RCC_AHB1LPENR_USB2OTGHSULPILPEN))

#define __HAL_RCC_C1_DMA1_CLK_SLEEP_DISABLE()             (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_DMA1LPEN))
#define __HAL_RCC_C1_DMA2_CLK_SLEEP_DISABLE()             (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_C1_ADC12_CLK_SLEEP_DISABLE()            (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_ADC12LPEN))
#define __HAL_RCC_C1_ETH1MAC_CLK_SLEEP_DISABLE()          (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1MACLPEN))
#define __HAL_RCC_C1_ETH1TX_CLK_SLEEP_DISABLE()           (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1TXLPEN))
#define __HAL_RCC_C1_ETH1RX_CLK_SLEEP_DISABLE()           (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1RXLPEN))
#define __HAL_RCC_C1_USB1_OTG_HS_CLK_SLEEP_DISABLE()      (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB1OTGHSLPEN))
#define __HAL_RCC_C1_USB1_OTG_HS_ULPI_CLK_SLEEP_DISABLE() (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB1OTGHSULPILPEN))
#define __HAL_RCC_C1_USB2_OTG_FS_CLK_SLEEP_DISABLE()      (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB2OTGHSLPEN))
#define __HAL_RCC_C1_USB2_OTG_FS_ULPI_CLK_SLEEP_DISABLE() (RCC_C1->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB2OTGHSULPILPEN))

/** @brief  ENABLE or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_DCMI_CLK_SLEEP_ENABLE()             (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_DCMILPEN))
#if defined(CRYP)
#define __HAL_RCC_C1_CRYP_CLK_SLEEP_ENABLE()             (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_CRYPLPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_C1_HASH_CLK_SLEEP_ENABLE()             (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_HASHLPEN))
#endif /* HASH */
#define __HAL_RCC_C1_RNG_CLK_SLEEP_ENABLE()              (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_C1_SDMMC2_CLK_SLEEP_ENABLE()           (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_SDMMC2LPEN))
#define __HAL_RCC_C1_D2SRAM1_CLK_SLEEP_ENABLE()          (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM1LPEN))
#define __HAL_RCC_C1_D2SRAM2_CLK_SLEEP_ENABLE()          (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM2LPEN))
#define __HAL_RCC_C1_D2SRAM3_CLK_SLEEP_ENABLE()          (RCC_C1->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM3LPEN))

#define __HAL_RCC_C1_DCMI_CLK_SLEEP_DISABLE()             (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_DCMILPEN))
#if defined(CRYP)
#define __HAL_RCC_C1_CRYP_CLK_SLEEP_DISABLE()             (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_CRYPLPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_C1_HASH_CLK_SLEEP_DISABLE()             (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_HASHLPEN))
#endif /* HASH */
#define __HAL_RCC_C1_RNG_CLK_SLEEP_DISABLE()              (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_C1_SDMMC2_CLK_SLEEP_DISABLE()           (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_SDMMC2LPEN))
#define __HAL_RCC_C1_D2SRAM1_CLK_SLEEP_DISABLE()          (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM1LPEN))
#define __HAL_RCC_C1_D2SRAM2_CLK_SLEEP_DISABLE()          (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM2LPEN))
#define __HAL_RCC_C1_D2SRAM3_CLK_SLEEP_DISABLE()          (RCC_C1->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM3LPEN))

/** @brief  ENABLE or disable the AHB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_GPIOA_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOALPEN)
#define __HAL_RCC_C1_GPIOB_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOBLPEN)
#define __HAL_RCC_C1_GPIOC_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOCLPEN)
#define __HAL_RCC_C1_GPIOD_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIODLPEN)
#define __HAL_RCC_C1_GPIOE_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOELPEN)
#define __HAL_RCC_C1_GPIOF_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOFLPEN)
#define __HAL_RCC_C1_GPIOG_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOGLPEN)
#define __HAL_RCC_C1_GPIOH_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOHLPEN)
#define __HAL_RCC_C1_GPIOI_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOILPEN)
#define __HAL_RCC_C1_GPIOJ_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOJLPEN)
#define __HAL_RCC_C1_GPIOK_CLK_SLEEP_ENABLE()           (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOKLPEN)
#define __HAL_RCC_C1_CRC_CLK_SLEEP_ENABLE()             (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_C1_BDMA_CLK_SLEEP_ENABLE()            (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_BDMALPEN)
#define __HAL_RCC_C1_ADC3_CLK_SLEEP_ENABLE()            (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_ADC3LPEN)
#define __HAL_RCC_C1_BKPRAM_CLK_SLEEP_ENABLE()          (RCC_C1->AHB4LPENR) |= (RCC_AHB4LPENR_BKPRAMLPEN)
#define __HAL_RCC_C1_D3SRAM1_CLK_SLEEP_ENABLE()         (RCC_C1->AHB4LPENR  |= (RCC_AHB4LPENR_D3SRAM1LPEN))

#define __HAL_RCC_C1_GPIOA_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOALPEN)
#define __HAL_RCC_C1_GPIOB_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOBLPEN)
#define __HAL_RCC_C1_GPIOC_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOCLPEN)
#define __HAL_RCC_C1_GPIOD_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIODLPEN)
#define __HAL_RCC_C1_GPIOE_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOELPEN)
#define __HAL_RCC_C1_GPIOF_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOFLPEN)
#define __HAL_RCC_C1_GPIOG_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOGLPEN)
#define __HAL_RCC_C1_GPIOH_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOHLPEN)
#define __HAL_RCC_C1_GPIOI_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOILPEN)
#define __HAL_RCC_C1_GPIOJ_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOJLPEN)
#define __HAL_RCC_C1_GPIOK_CLK_SLEEP_DISABLE()           (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOKLPEN)
#define __HAL_RCC_C1_CRC_CLK_SLEEP_DISABLE()             (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_C1_BDMA_CLK_SLEEP_DISABLE()            (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BDMALPEN)
#define __HAL_RCC_C1_ADC3_CLK_SLEEP_DISABLE()            (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_ADC3LPEN)
#define __HAL_RCC_C1_BKPRAM_CLK_SLEEP_DISABLE()          (RCC_C1->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BKPRAMLPEN)
#define __HAL_RCC_C1_D3SRAM1_CLK_SLEEP_DISABLE()         (RCC_C1->AHB4LPENR  &= ~ (RCC_AHB4LPENR_D3SRAM1LPEN))

/** @brief  ENABLE or disable the APB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_LTDC_CLK_SLEEP_ENABLE()           (RCC_C1->APB3LPENR) |= (RCC_APB3LPENR_LTDCLPEN)
#define __HAL_RCC_C1_DSI_CLK_SLEEP_ENABLE()            (RCC_C1->APB3LPENR) |= (RCC_APB3LPENR_DSILPEN)
#define __HAL_RCC_C1_WWDG1_CLK_SLEEP_ENABLE()          (RCC_C1->APB3LPENR) |= (RCC_APB3LPENR_WWDG1LPEN)

#define __HAL_RCC_C1_LTDC_CLK_SLEEP_DISABLE()           (RCC_C1->APB3LPENR) &= ~ (RCC_APB3LPENR_LTDCLPEN)
#define __HAL_RCC_C1_DSI_CLK_SLEEP_DISABLE()            (RCC_C1->APB3LPENR) &= ~ (RCC_APB3LPENR_DSILPEN)
#define __HAL_RCC_C1_WWDG1_CLK_SLEEP_DISABLE()          (RCC_C1->APB3LPENR) &= ~ (RCC_APB3LPENR_WWDG1LPEN)

/** @brief  ENABLE or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_TIM2_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM2LPEN)
#define __HAL_RCC_C1_TIM3_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM3LPEN)
#define __HAL_RCC_C1_TIM4_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM4LPEN)
#define __HAL_RCC_C1_TIM5_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM5LPEN)
#define __HAL_RCC_C1_TIM6_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM6LPEN)
#define __HAL_RCC_C1_TIM7_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM7LPEN)
#define __HAL_RCC_C1_TIM12_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM12LPEN)
#define __HAL_RCC_C1_TIM13_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM13LPEN)
#define __HAL_RCC_C1_TIM14_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_TIM14LPEN)
#define __HAL_RCC_C1_LPTIM1_CLK_SLEEP_ENABLE()         (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_LPTIM1LPEN)
#define __HAL_RCC_C1_WWDG2_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_WWDG2LPEN)
#define __HAL_RCC_C1_SPI2_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_SPI2LPEN)
#define __HAL_RCC_C1_SPI3_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_SPI3LPEN)
#define __HAL_RCC_C1_SPDIFRX_CLK_SLEEP_ENABLE()        (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_SPDIFRXLPEN)
#define __HAL_RCC_C1_USART2_CLK_SLEEP_ENABLE()         (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_USART2LPEN)
#define __HAL_RCC_C1_USART3_CLK_SLEEP_ENABLE()         (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_USART3LPEN)
#define __HAL_RCC_C1_UART4_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_UART4LPEN)
#define __HAL_RCC_C1_UART5_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_UART5LPEN)
#define __HAL_RCC_C1_I2C1_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_I2C1LPEN)
#define __HAL_RCC_C1_I2C2_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_I2C2LPEN)
#define __HAL_RCC_C1_I2C3_CLK_SLEEP_ENABLE()           (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_I2C3LPEN)
#define __HAL_RCC_C1_CEC_CLK_SLEEP_ENABLE()            (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_CECLPEN)
#define __HAL_RCC_C1_DAC12_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_DAC12LPEN)
#define __HAL_RCC_C1_UART7_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_UART7LPEN)
#define __HAL_RCC_C1_UART8_CLK_SLEEP_ENABLE()          (RCC_C1->APB1LLPENR) |= (RCC_APB1LLPENR_UART8LPEN)
#define __HAL_RCC_C1_CRS_CLK_SLEEP_ENABLE()            (RCC_C1->APB1HLPENR) |= (RCC_APB1HLPENR_CRSLPEN)
#define __HAL_RCC_C1_SWPMI_CLK_SLEEP_ENABLE()          (RCC_C1->APB1HLPENR) |= (RCC_APB1HLPENR_SWPMILPEN)
#define __HAL_RCC_C1_OPAMP_CLK_SLEEP_ENABLE()          (RCC_C1->APB1HLPENR) |= (RCC_APB1HLPENR_OPAMPLPEN)
#define __HAL_RCC_C1_MDIOS_CLK_SLEEP_ENABLE()          (RCC_C1->APB1HLPENR) |= (RCC_APB1HLPENR_MDIOSLPEN)
#define __HAL_RCC_C1_FDCAN_CLK_SLEEP_ENABLE()          (RCC_C1->APB1HLPENR) |= (RCC_APB1HLPENR_FDCANLPEN)


#define __HAL_RCC_C1_TIM2_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM2LPEN)
#define __HAL_RCC_C1_TIM3_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM3LPEN)
#define __HAL_RCC_C1_TIM4_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM4LPEN)
#define __HAL_RCC_C1_TIM5_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM5LPEN)
#define __HAL_RCC_C1_TIM6_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM6LPEN)
#define __HAL_RCC_C1_TIM7_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM7LPEN)
#define __HAL_RCC_C1_TIM12_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM12LPEN)
#define __HAL_RCC_C1_TIM13_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM13LPEN)
#define __HAL_RCC_C1_TIM14_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM14LPEN)
#define __HAL_RCC_C1_LPTIM1_CLK_SLEEP_DISABLE()         (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_LPTIM1LPEN)
#define __HAL_RCC_C1_WWDG2_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_WWDG2LPEN)
#define __HAL_RCC_C1_SPI2_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPI2LPEN)
#define __HAL_RCC_C1_SPI3_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPI3LPEN)
#define __HAL_RCC_C1_SPDIFRX_CLK_SLEEP_DISABLE()        (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPDIFRXLPEN)
#define __HAL_RCC_C1_USART2_CLK_SLEEP_DISABLE()         (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_USART2LPEN)
#define __HAL_RCC_C1_USART3_CLK_SLEEP_DISABLE()         (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_USART3LPEN)
#define __HAL_RCC_C1_UART4_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART4LPEN)
#define __HAL_RCC_C1_UART5_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART5LPEN)
#define __HAL_RCC_C1_I2C1_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C1LPEN)
#define __HAL_RCC_C1_I2C2_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C2LPEN)
#define __HAL_RCC_C1_I2C3_CLK_SLEEP_DISABLE()           (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C3LPEN)
#define __HAL_RCC_C1_CEC_CLK_SLEEP_DISABLE()            (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_CECLPEN)
#define __HAL_RCC_C1_DAC12_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_DAC12LPEN)
#define __HAL_RCC_C1_UART7_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART7LPEN)
#define __HAL_RCC_C1_UART8_CLK_SLEEP_DISABLE()          (RCC_C1->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART8LPEN)
#define __HAL_RCC_C1_CRS_CLK_SLEEP_DISABLE()            (RCC_C1->APB1HLPENR) &= ~ (RCC_APB1HLPENR_CRSLPEN)
#define __HAL_RCC_C1_SWPMI_CLK_SLEEP_DISABLE()          (RCC_C1->APB1HLPENR) &= ~ (RCC_APB1HLPENR_SWPMILPEN)
#define __HAL_RCC_C1_OPAMP_CLK_SLEEP_DISABLE()          (RCC_C1->APB1HLPENR) &= ~ (RCC_APB1HLPENR_OPAMPLPEN)
#define __HAL_RCC_C1_MDIOS_CLK_SLEEP_DISABLE()          (RCC_C1->APB1HLPENR) &= ~ (RCC_APB1HLPENR_MDIOSLPEN)
#define __HAL_RCC_C1_FDCAN_CLK_SLEEP_DISABLE()          (RCC_C1->APB1HLPENR) &= ~ (RCC_APB1HLPENR_FDCANLPEN)

/** @brief  ENABLE or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_TIM1_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_TIM1LPEN)
#define __HAL_RCC_C1_TIM8_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_TIM8LPEN)
#define __HAL_RCC_C1_USART1_CLK_SLEEP_ENABLE()         (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_USART1LPEN)
#define __HAL_RCC_C1_USART6_CLK_SLEEP_ENABLE()         (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_USART6LPEN)
#define __HAL_RCC_C1_SPI1_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_SPI1LPEN)
#define __HAL_RCC_C1_SPI4_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_SPI4LPEN)
#define __HAL_RCC_C1_TIM15_CLK_SLEEP_ENABLE()          (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_TIM15LPEN)
#define __HAL_RCC_C1_TIM16_CLK_SLEEP_ENABLE()          (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_TIM16LPEN)
#define __HAL_RCC_C1_TIM17_CLK_SLEEP_ENABLE()          (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_TIM17LPEN)
#define __HAL_RCC_C1_SPI5_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_SPI5LPEN)
#define __HAL_RCC_C1_SAI1_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_SAI1LPEN)
#define __HAL_RCC_C1_SAI2_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_SAI2LPEN)
#define __HAL_RCC_C1_SAI3_CLK_SLEEP_ENABLE()           (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_SAI3LPEN)
#define __HAL_RCC_C1_DFSDM1_CLK_SLEEP_ENABLE()         (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_DFSDM1LPEN)
#define __HAL_RCC_C1_HRTIM1_CLK_SLEEP_ENABLE()         (RCC_C1->APB2LPENR) |= (RCC_APB2LPENR_HRTIMLPEN)

#define __HAL_RCC_C1_TIM1_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM1LPEN)
#define __HAL_RCC_C1_TIM8_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM8LPEN)
#define __HAL_RCC_C1_USART1_CLK_SLEEP_DISABLE()         (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_USART1LPEN)
#define __HAL_RCC_C1_USART6_CLK_SLEEP_DISABLE()         (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_USART6LPEN)
#define __HAL_RCC_C1_SPI1_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI1LPEN)
#define __HAL_RCC_C1_SPI4_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI4LPEN)
#define __HAL_RCC_C1_TIM15_CLK_SLEEP_DISABLE()          (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM15LPEN)
#define __HAL_RCC_C1_TIM16_CLK_SLEEP_DISABLE()          (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM16LPEN)
#define __HAL_RCC_C1_TIM17_CLK_SLEEP_DISABLE()          (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM17LPEN)
#define __HAL_RCC_C1_SPI5_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI5LPEN)
#define __HAL_RCC_C1_SAI1_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI1LPEN)
#define __HAL_RCC_C1_SAI2_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI2LPEN)
#define __HAL_RCC_C1_SAI3_CLK_SLEEP_DISABLE()           (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI3LPEN)
#define __HAL_RCC_C1_DFSDM1_CLK_SLEEP_DISABLE()         (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_DFSDM1LPEN)
#define __HAL_RCC_C1_HRTIM1_CLK_SLEEP_DISABLE()         (RCC_C1->APB2LPENR) &= ~ (RCC_APB2LPENR_HRTIMLPEN)

/** @brief  ENABLE or disable the APB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C1_SYSCFG_CLK_SLEEP_ENABLE()           (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_SYSCFGLPEN)
#define __HAL_RCC_C1_LPUART1_CLK_SLEEP_ENABLE()          (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_LPUART1LPEN)
#define __HAL_RCC_C1_SPI6_CLK_SLEEP_ENABLE()             (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_SPI6LPEN)
#define __HAL_RCC_C1_I2C4_CLK_SLEEP_ENABLE()             (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_I2C4LPEN)
#define __HAL_RCC_C1_LPTIM2_CLK_SLEEP_ENABLE()           (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_LPTIM2LPEN)
#define __HAL_RCC_C1_LPTIM3_CLK_SLEEP_ENABLE()           (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_LPTIM3LPEN)
#define __HAL_RCC_C1_LPTIM4_CLK_SLEEP_ENABLE()           (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_LPTIM4LPEN)
#define __HAL_RCC_C1_LPTIM5_CLK_SLEEP_ENABLE()           (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_LPTIM5LPEN)
#define __HAL_RCC_C1_COMP12_CLK_SLEEP_ENABLE()           (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_COMP12LPEN)
#define __HAL_RCC_C1_VREF_CLK_SLEEP_ENABLE()             (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_VREFLPEN)
#define __HAL_RCC_C1_SAI4_CLK_SLEEP_ENABLE()             (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_SAI4LPEN)
#define __HAL_RCC_C1_RTC_CLK_SLEEP_ENABLE()              (RCC_C1->APB4LPENR) |= (RCC_APB4LPENR_RTCAPBLPEN)


#define __HAL_RCC_C1_SYSCFG_CLK_SLEEP_DISABLE()           (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_SYSCFGLPEN)
#define __HAL_RCC_C1_LPUART1_CLK_SLEEP_DISABLE()          (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_LPUART1LPEN)
#define __HAL_RCC_C1_SPI6_CLK_SLEEP_DISABLE()             (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_SPI6LPEN)
#define __HAL_RCC_C1_I2C4_CLK_SLEEP_DISABLE()             (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_I2C4LPEN)
#define __HAL_RCC_C1_LPTIM2_CLK_SLEEP_DISABLE()           (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM2LPEN)
#define __HAL_RCC_C1_LPTIM3_CLK_SLEEP_DISABLE()           (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM3LPEN)
#define __HAL_RCC_C1_LPTIM4_CLK_SLEEP_DISABLE()           (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM4LPEN)
#define __HAL_RCC_C1_LPTIM5_CLK_SLEEP_DISABLE()           (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM5LPEN)
#define __HAL_RCC_C1_COMP12_CLK_SLEEP_DISABLE()           (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_COMP12LPEN)
#define __HAL_RCC_C1_VREF_CLK_SLEEP_DISABLE()             (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_VREFLPEN)
#define __HAL_RCC_C1_SAI4_CLK_SLEEP_DISABLE()             (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_SAI4LPEN)
#define __HAL_RCC_C1_RTC_CLK_SLEEP_DISABLE()              (RCC_C1->APB4LPENR) &= ~ (RCC_APB4LPENR_RTCAPBLPEN)

/** @brief  Enable or disable the RCC_C2 AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */


#define __HAL_RCC_C2_MDMA_CLK_SLEEP_ENABLE()            (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_MDMALPEN))
#define __HAL_RCC_C2_DMA2D_CLK_SLEEP_ENABLE()           (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_DMA2DLPEN))
#define __HAL_RCC_C2_JPGDEC_CLK_SLEEP_ENABLE()          (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_JPGDECLPEN))
#define __HAL_RCC_C2_FLASH_CLK_SLEEP_ENABLE()           (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_FLASHLPEN))
#define __HAL_RCC_C2_FMC_CLK_SLEEP_ENABLE()             (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_FMCLPEN))
#define __HAL_RCC_C2_QSPI_CLK_SLEEP_ENABLE()            (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_QSPILPEN))
#define __HAL_RCC_C2_SDMMC1_CLK_SLEEP_ENABLE()          (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_SDMMC1LPEN))
#define __HAL_RCC_C2_DTCM1_CLK_SLEEP_ENABLE()           (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_DTCM1LPEN))
#define __HAL_RCC_C2_DTCM2_CLK_SLEEP_ENABLE()           (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_DTCM2LPEN))
#define __HAL_RCC_C2_ITCM_CLK_SLEEP_ENABLE()            (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_ITCMLPEN))
#define __HAL_RCC_C2_D1SRAM1_CLK_SLEEP_ENABLE()         (RCC_C2->AHB3LPENR |= (RCC_AHB3LPENR_AXISRAMLPEN))


#define __HAL_RCC_C2_MDMA_CLK_SLEEP_DISABLE()            (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_MDMALPEN))
#define __HAL_RCC_C2_DMA2D_CLK_SLEEP_DISABLE()           (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_DMA2DLPEN))
#define __HAL_RCC_C2_JPGDEC_CLK_SLEEP_DISABLE()          (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_JPGDECLPEN))
#define __HAL_RCC_C2_FLASH_CLK_SLEEP_DISABLE()           (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_FLASHLPEN))
#define __HAL_RCC_C2_FMC_CLK_SLEEP_DISABLE()             (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_FMCLPEN))
#define __HAL_RCC_C2_QSPI_CLK_SLEEP_DISABLE()            (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_QSPILPEN))
#define __HAL_RCC_C2_SDMMC1_CLK_SLEEP_DISABLE()          (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_SDMMC1LPEN))
#define __HAL_RCC_C2_DTCM1_CLK_SLEEP_DISABLE()           (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_DTCM1LPEN))
#define __HAL_RCC_C2_DTCM2_CLK_SLEEP_DISABLE()           (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_DTCM2LPEN))
#define __HAL_RCC_C2_ITCM_CLK_SLEEP_DISABLE()            (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_ITCMLPEN))
#define __HAL_RCC_C2_D1SRAM1_CLK_SLEEP_DISABLE()         (RCC_C2->AHB3LPENR &= ~ (RCC_AHB3LPENR_AXISRAMLPEN))



/** @brief  ENABLE or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_DMA1_CLK_SLEEP_ENABLE()             (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_DMA1LPEN))
#define __HAL_RCC_C2_DMA2_CLK_SLEEP_ENABLE()             (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_C2_ADC12_CLK_SLEEP_ENABLE()            (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_ADC12LPEN))
#define __HAL_RCC_C2_ETH1MAC_CLK_SLEEP_ENABLE()          (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_ETH1MACLPEN))
#define __HAL_RCC_C2_ETH1TX_CLK_SLEEP_ENABLE()           (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_ETH1TXLPEN))
#define __HAL_RCC_C2_ETH1RX_CLK_SLEEP_ENABLE()           (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_ETH1RXLPEN))
#define __HAL_RCC_C2_USB1_OTG_HS_CLK_SLEEP_ENABLE()      (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_USB1OTGHSLPEN))
#define __HAL_RCC_C2_USB1_OTG_HS_ULPI_CLK_SLEEP_ENABLE() (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_USB1OTGHSULPILPEN))
#define __HAL_RCC_C2_USB2_OTG_FS_CLK_SLEEP_ENABLE()      (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_USB2OTGHSLPEN))
#define __HAL_RCC_C2_USB2_OTG_FS_ULPI_CLK_SLEEP_ENABLE() (RCC_C2->AHB1LPENR |= (RCC_AHB1LPENR_USB2OTGHSULPILPEN))

#define __HAL_RCC_C2_DMA1_CLK_SLEEP_DISABLE()             (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_DMA1LPEN))
#define __HAL_RCC_C2_DMA2_CLK_SLEEP_DISABLE()             (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_C2_ADC12_CLK_SLEEP_DISABLE()            (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_ADC12LPEN))
#define __HAL_RCC_C2_ETH1MAC_CLK_SLEEP_DISABLE()          (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1MACLPEN))
#define __HAL_RCC_C2_ETH1TX_CLK_SLEEP_DISABLE()           (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1TXLPEN))
#define __HAL_RCC_C2_ETH1RX_CLK_SLEEP_DISABLE()           (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_ETH1RXLPEN))
#define __HAL_RCC_C2_USB1_OTG_HS_CLK_SLEEP_DISABLE()      (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB1OTGHSLPEN))
#define __HAL_RCC_C2_USB1_OTG_HS_ULPI_CLK_SLEEP_DISABLE() (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB1OTGHSULPILPEN))
#define __HAL_RCC_C2_USB2_OTG_FS_CLK_SLEEP_DISABLE()      (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB2OTGHSLPEN))
#define __HAL_RCC_C2_USB2_OTG_FS_ULPI_CLK_SLEEP_DISABLE() (RCC_C2->AHB1LPENR &= ~ (RCC_AHB1LPENR_USB2OTGHSULPILPEN))

/** @brief  ENABLE or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_DCMI_CLK_SLEEP_ENABLE()             (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_DCMILPEN))
#if defined(CRYP)
#define __HAL_RCC_C2_CRYP_CLK_SLEEP_ENABLE()             (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_CRYPLPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_C2_HASH_CLK_SLEEP_ENABLE()             (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_HASHLPEN))
#endif /* HASH */
#define __HAL_RCC_C2_RNG_CLK_SLEEP_ENABLE()              (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_C2_SDMMC2_CLK_SLEEP_ENABLE()           (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_SDMMC2LPEN))
#define __HAL_RCC_C2_D2SRAM1_CLK_SLEEP_ENABLE()          (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM1LPEN))
#define __HAL_RCC_C2_D2SRAM2_CLK_SLEEP_ENABLE()          (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM2LPEN))
#define __HAL_RCC_C2_D2SRAM3_CLK_SLEEP_ENABLE()          (RCC_C2->AHB2LPENR |= (RCC_AHB2LPENR_D2SRAM3LPEN))

#define __HAL_RCC_C2_DCMI_CLK_SLEEP_DISABLE()             (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_DCMILPEN))
#if defined(CRYP)
#define __HAL_RCC_C2_CRYP_CLK_SLEEP_DISABLE()             (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_CRYPLPEN))
#endif /* CRYP */
#if defined(HASH)
#define __HAL_RCC_C2_HASH_CLK_SLEEP_DISABLE()             (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_HASHLPEN))
#endif /* HASH */
#define __HAL_RCC_C2_RNG_CLK_SLEEP_DISABLE()              (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_C2_SDMMC2_CLK_SLEEP_DISABLE()           (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_SDMMC2LPEN))
#define __HAL_RCC_C2_D2SRAM1_CLK_SLEEP_DISABLE()          (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM1LPEN))
#define __HAL_RCC_C2_D2SRAM2_CLK_SLEEP_DISABLE()          (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM2LPEN))
#define __HAL_RCC_C2_D2SRAM3_CLK_SLEEP_DISABLE()          (RCC_C2->AHB2LPENR &= ~ (RCC_AHB2LPENR_D2SRAM3LPEN))

/** @brief  ENABLE or disable the AHB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_GPIOA_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOALPEN)
#define __HAL_RCC_C2_GPIOB_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOBLPEN)
#define __HAL_RCC_C2_GPIOC_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOCLPEN)
#define __HAL_RCC_C2_GPIOD_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIODLPEN)
#define __HAL_RCC_C2_GPIOE_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOELPEN)
#define __HAL_RCC_C2_GPIOF_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOFLPEN)
#define __HAL_RCC_C2_GPIOG_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOGLPEN)
#define __HAL_RCC_C2_GPIOH_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOHLPEN)
#define __HAL_RCC_C2_GPIOI_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOILPEN)
#define __HAL_RCC_C2_GPIOJ_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOJLPEN)
#define __HAL_RCC_C2_GPIOK_CLK_SLEEP_ENABLE()           (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_GPIOKLPEN)
#define __HAL_RCC_C2_CRC_CLK_SLEEP_ENABLE()             (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_C2_BDMA_CLK_SLEEP_ENABLE()            (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_BDMALPEN)
#define __HAL_RCC_C2_ADC3_CLK_SLEEP_ENABLE()            (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_ADC3LPEN)
#define __HAL_RCC_C2_BKPRAM_CLK_SLEEP_ENABLE()          (RCC_C2->AHB4LPENR) |= (RCC_AHB4LPENR_BKPRAMLPEN)
#define __HAL_RCC_C2_D3SRAM1_CLK_SLEEP_ENABLE()         (RCC_C2->AHB4LPENR  |= (RCC_AHB4LPENR_D3SRAM1LPEN))

#define __HAL_RCC_C2_GPIOA_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOALPEN)
#define __HAL_RCC_C2_GPIOB_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOBLPEN)
#define __HAL_RCC_C2_GPIOC_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOCLPEN)
#define __HAL_RCC_C2_GPIOD_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIODLPEN)
#define __HAL_RCC_C2_GPIOE_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOELPEN)
#define __HAL_RCC_C2_GPIOF_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOFLPEN)
#define __HAL_RCC_C2_GPIOG_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOGLPEN)
#define __HAL_RCC_C2_GPIOH_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOHLPEN)
#define __HAL_RCC_C2_GPIOI_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOILPEN)
#define __HAL_RCC_C2_GPIOJ_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOJLPEN)
#define __HAL_RCC_C2_GPIOK_CLK_SLEEP_DISABLE()           (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_GPIOKLPEN)
#define __HAL_RCC_C2_CRC_CLK_SLEEP_DISABLE()             (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_CRCLPEN)
#define __HAL_RCC_C2_BDMA_CLK_SLEEP_DISABLE()            (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BDMALPEN)
#define __HAL_RCC_C2_ADC3_CLK_SLEEP_DISABLE()            (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_ADC3LPEN)
#define __HAL_RCC_C2_BKPRAM_CLK_SLEEP_DISABLE()          (RCC_C2->AHB4LPENR) &= ~ (RCC_AHB4LPENR_BKPRAMLPEN)
#define __HAL_RCC_C2_D3SRAM1_CLK_SLEEP_DISABLE()         (RCC_C2->AHB4LPENR  &= ~ (RCC_AHB4LPENR_D3SRAM1LPEN))

/** @brief  ENABLE or disable the APB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_LTDC_CLK_SLEEP_ENABLE()           (RCC_C2->APB3LPENR) |= (RCC_APB3LPENR_LTDCLPEN)
#define __HAL_RCC_C2_DSI_CLK_SLEEP_ENABLE()            (RCC_C2->APB3LPENR) |= (RCC_APB3LPENR_DSILPEN)
#define __HAL_RCC_C2_WWDG1_CLK_SLEEP_ENABLE()          (RCC_C2->APB3LPENR) |= (RCC_APB3LPENR_WWDG1LPEN)

#define __HAL_RCC_C2_LTDC_CLK_SLEEP_DISABLE()           (RCC_C2->APB3LPENR) &= ~ (RCC_APB3LPENR_LTDCLPEN)
#define __HAL_RCC_C2_DSI_CLK_SLEEP_DISABLE()            (RCC_C2->APB3LPENR) &= ~ (RCC_APB3LPENR_DSILPEN)
#define __HAL_RCC_C2_WWDG1_CLK_SLEEP_DISABLE()          (RCC_C2->APB3LPENR) &= ~ (RCC_APB3LPENR_WWDG1LPEN)

/** @brief  ENABLE or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_TIM2_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM2LPEN)
#define __HAL_RCC_C2_TIM3_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM3LPEN)
#define __HAL_RCC_C2_TIM4_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM4LPEN)
#define __HAL_RCC_C2_TIM5_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM5LPEN)
#define __HAL_RCC_C2_TIM6_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM6LPEN)
#define __HAL_RCC_C2_TIM7_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM7LPEN)
#define __HAL_RCC_C2_TIM12_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM12LPEN)
#define __HAL_RCC_C2_TIM13_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM13LPEN)
#define __HAL_RCC_C2_TIM14_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_TIM14LPEN)
#define __HAL_RCC_C2_LPTIM1_CLK_SLEEP_ENABLE()         (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_LPTIM1LPEN)
#define __HAL_RCC_C2_WWDG2_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_WWDG2LPEN)
#define __HAL_RCC_C2_SPI2_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_SPI2LPEN)
#define __HAL_RCC_C2_SPI3_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_SPI3LPEN)
#define __HAL_RCC_C2_SPDIFRX_CLK_SLEEP_ENABLE()        (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_SPDIFRXLPEN)
#define __HAL_RCC_C2_USART2_CLK_SLEEP_ENABLE()         (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_USART2LPEN)
#define __HAL_RCC_C2_USART3_CLK_SLEEP_ENABLE()         (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_USART3LPEN)
#define __HAL_RCC_C2_UART4_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_UART4LPEN)
#define __HAL_RCC_C2_UART5_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_UART5LPEN)
#define __HAL_RCC_C2_I2C1_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_I2C1LPEN)
#define __HAL_RCC_C2_I2C2_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_I2C2LPEN)
#define __HAL_RCC_C2_I2C3_CLK_SLEEP_ENABLE()           (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_I2C3LPEN)
#define __HAL_RCC_C2_CEC_CLK_SLEEP_ENABLE()            (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_CECLPEN)
#define __HAL_RCC_C2_DAC12_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_DAC12LPEN)
#define __HAL_RCC_C2_UART7_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_UART7LPEN)
#define __HAL_RCC_C2_UART8_CLK_SLEEP_ENABLE()          (RCC_C2->APB1LLPENR) |= (RCC_APB1LLPENR_UART8LPEN)
#define __HAL_RCC_C2_CRS_CLK_SLEEP_ENABLE()            (RCC_C2->APB1HLPENR) |= (RCC_APB1HLPENR_CRSLPEN)
#define __HAL_RCC_C2_SWPMI_CLK_SLEEP_ENABLE()          (RCC_C2->APB1HLPENR) |= (RCC_APB1HLPENR_SWPMILPEN)
#define __HAL_RCC_C2_OPAMP_CLK_SLEEP_ENABLE()          (RCC_C2->APB1HLPENR) |= (RCC_APB1HLPENR_OPAMPLPEN)
#define __HAL_RCC_C2_MDIOS_CLK_SLEEP_ENABLE()          (RCC_C2->APB1HLPENR) |= (RCC_APB1HLPENR_MDIOSLPEN)
#define __HAL_RCC_C2_FDCAN_CLK_SLEEP_ENABLE()          (RCC_C2->APB1HLPENR) |= (RCC_APB1HLPENR_FDCANLPEN)


#define __HAL_RCC_C2_TIM2_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM2LPEN)
#define __HAL_RCC_C2_TIM3_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM3LPEN)
#define __HAL_RCC_C2_TIM4_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM4LPEN)
#define __HAL_RCC_C2_TIM5_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM5LPEN)
#define __HAL_RCC_C2_TIM6_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM6LPEN)
#define __HAL_RCC_C2_TIM7_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM7LPEN)
#define __HAL_RCC_C2_TIM12_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM12LPEN)
#define __HAL_RCC_C2_TIM13_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM13LPEN)
#define __HAL_RCC_C2_TIM14_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_TIM14LPEN)
#define __HAL_RCC_C2_LPTIM1_CLK_SLEEP_DISABLE()         (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_LPTIM1LPEN)
#define __HAL_RCC_C2_WWDG2_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_WWDG2LPEN)
#define __HAL_RCC_C2_SPI2_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPI2LPEN)
#define __HAL_RCC_C2_SPI3_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPI3LPEN)
#define __HAL_RCC_C2_SPDIFRX_CLK_SLEEP_DISABLE()        (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_SPDIFRXLPEN)
#define __HAL_RCC_C2_USART2_CLK_SLEEP_DISABLE()         (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_USART2LPEN)
#define __HAL_RCC_C2_USART3_CLK_SLEEP_DISABLE()         (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_USART3LPEN)
#define __HAL_RCC_C2_UART4_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART4LPEN)
#define __HAL_RCC_C2_UART5_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART5LPEN)
#define __HAL_RCC_C2_I2C1_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C1LPEN)
#define __HAL_RCC_C2_I2C2_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C2LPEN)
#define __HAL_RCC_C2_I2C3_CLK_SLEEP_DISABLE()           (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_I2C3LPEN)
#define __HAL_RCC_C2_CEC_CLK_SLEEP_DISABLE()            (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_CECLPEN)
#define __HAL_RCC_C2_DAC12_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_DAC12LPEN)
#define __HAL_RCC_C2_UART7_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART7LPEN)
#define __HAL_RCC_C2_UART8_CLK_SLEEP_DISABLE()          (RCC_C2->APB1LLPENR) &= ~ (RCC_APB1LLPENR_UART8LPEN)
#define __HAL_RCC_C2_CRS_CLK_SLEEP_DISABLE()            (RCC_C2->APB1HLPENR) &= ~ (RCC_APB1HLPENR_CRSLPEN)
#define __HAL_RCC_C2_SWPMI_CLK_SLEEP_DISABLE()          (RCC_C2->APB1HLPENR) &= ~ (RCC_APB1HLPENR_SWPMILPEN)
#define __HAL_RCC_C2_OPAMP_CLK_SLEEP_DISABLE()          (RCC_C2->APB1HLPENR) &= ~ (RCC_APB1HLPENR_OPAMPLPEN)
#define __HAL_RCC_C2_MDIOS_CLK_SLEEP_DISABLE()          (RCC_C2->APB1HLPENR) &= ~ (RCC_APB1HLPENR_MDIOSLPEN)
#define __HAL_RCC_C2_FDCAN_CLK_SLEEP_DISABLE()          (RCC_C2->APB1HLPENR) &= ~ (RCC_APB1HLPENR_FDCANLPEN)

/** @brief  ENABLE or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_TIM1_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_TIM1LPEN)
#define __HAL_RCC_C2_TIM8_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_TIM8LPEN)
#define __HAL_RCC_C2_USART1_CLK_SLEEP_ENABLE()         (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_USART1LPEN)
#define __HAL_RCC_C2_USART6_CLK_SLEEP_ENABLE()         (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_USART6LPEN)
#define __HAL_RCC_C2_SPI1_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_SPI1LPEN)
#define __HAL_RCC_C2_SPI4_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_SPI4LPEN)
#define __HAL_RCC_C2_TIM15_CLK_SLEEP_ENABLE()          (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_TIM15LPEN)
#define __HAL_RCC_C2_TIM16_CLK_SLEEP_ENABLE()          (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_TIM16LPEN)
#define __HAL_RCC_C2_TIM17_CLK_SLEEP_ENABLE()          (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_TIM17LPEN)
#define __HAL_RCC_C2_SPI5_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_SPI5LPEN)
#define __HAL_RCC_C2_SAI1_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_SAI1LPEN)
#define __HAL_RCC_C2_SAI2_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_SAI2LPEN)
#define __HAL_RCC_C2_SAI3_CLK_SLEEP_ENABLE()           (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_SAI3LPEN)
#define __HAL_RCC_C2_DFSDM1_CLK_SLEEP_ENABLE()         (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_DFSDM1LPEN)
#define __HAL_RCC_C2_HRTIM1_CLK_SLEEP_ENABLE()         (RCC_C2->APB2LPENR) |= (RCC_APB2LPENR_HRTIMLPEN)

#define __HAL_RCC_C2_TIM1_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM1LPEN)
#define __HAL_RCC_C2_TIM8_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM8LPEN)
#define __HAL_RCC_C2_USART1_CLK_SLEEP_DISABLE()         (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_USART1LPEN)
#define __HAL_RCC_C2_USART6_CLK_SLEEP_DISABLE()         (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_USART6LPEN)
#define __HAL_RCC_C2_SPI1_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI1LPEN)
#define __HAL_RCC_C2_SPI4_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI4LPEN)
#define __HAL_RCC_C2_TIM15_CLK_SLEEP_DISABLE()          (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM15LPEN)
#define __HAL_RCC_C2_TIM16_CLK_SLEEP_DISABLE()          (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM16LPEN)
#define __HAL_RCC_C2_TIM17_CLK_SLEEP_DISABLE()          (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_TIM17LPEN)
#define __HAL_RCC_C2_SPI5_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_SPI5LPEN)
#define __HAL_RCC_C2_SAI1_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI1LPEN)
#define __HAL_RCC_C2_SAI2_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI2LPEN)
#define __HAL_RCC_C2_SAI3_CLK_SLEEP_DISABLE()           (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_SAI3LPEN)
#define __HAL_RCC_C2_DFSDM1_CLK_SLEEP_DISABLE()         (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_DFSDM1LPEN)
#define __HAL_RCC_C2_HRTIM1_CLK_SLEEP_DISABLE()         (RCC_C2->APB2LPENR) &= ~ (RCC_APB2LPENR_HRTIMLPEN)

/** @brief  ENABLE or disable the APB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is ENABLEd again.
  * @note   By default, all peripheral clocks are ENABLEd during SLEEP mode.
  */

#define __HAL_RCC_C2_SYSCFG_CLK_SLEEP_ENABLE()           (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_SYSCFGLPEN)
#define __HAL_RCC_C2_LPUART1_CLK_SLEEP_ENABLE()          (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_LPUART1LPEN)
#define __HAL_RCC_C2_SPI6_CLK_SLEEP_ENABLE()             (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_SPI6LPEN)
#define __HAL_RCC_C2_I2C4_CLK_SLEEP_ENABLE()             (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_I2C4LPEN)
#define __HAL_RCC_C2_LPTIM2_CLK_SLEEP_ENABLE()           (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_LPTIM2LPEN)
#define __HAL_RCC_C2_LPTIM3_CLK_SLEEP_ENABLE()           (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_LPTIM3LPEN)
#define __HAL_RCC_C2_LPTIM4_CLK_SLEEP_ENABLE()           (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_LPTIM4LPEN)
#define __HAL_RCC_C2_LPTIM5_CLK_SLEEP_ENABLE()           (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_LPTIM5LPEN)
#define __HAL_RCC_C2_COMP12_CLK_SLEEP_ENABLE()           (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_COMP12LPEN)
#define __HAL_RCC_C2_VREF_CLK_SLEEP_ENABLE()             (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_VREFLPEN)
#define __HAL_RCC_C2_SAI4_CLK_SLEEP_ENABLE()             (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_SAI4LPEN)
#define __HAL_RCC_C2_RTC_CLK_SLEEP_ENABLE()              (RCC_C2->APB4LPENR) |= (RCC_APB4LPENR_RTCAPBLPEN)

#define __HAL_RCC_C2_SYSCFG_CLK_SLEEP_DISABLE()           (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_SYSCFGLPEN)
#define __HAL_RCC_C2_LPUART1_CLK_SLEEP_DISABLE()          (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_LPUART1LPEN)
#define __HAL_RCC_C2_SPI6_CLK_SLEEP_DISABLE()             (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_SPI6LPEN)
#define __HAL_RCC_C2_I2C4_CLK_SLEEP_DISABLE()             (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_I2C4LPEN)
#define __HAL_RCC_C2_LPTIM2_CLK_SLEEP_DISABLE()           (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM2LPEN)
#define __HAL_RCC_C2_LPTIM3_CLK_SLEEP_DISABLE()           (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM3LPEN)
#define __HAL_RCC_C2_LPTIM4_CLK_SLEEP_DISABLE()           (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM4LPEN)
#define __HAL_RCC_C2_LPTIM5_CLK_SLEEP_DISABLE()           (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_LPTIM5LPEN)
#define __HAL_RCC_C2_COMP12_CLK_SLEEP_DISABLE()           (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_COMP12LPEN)
#define __HAL_RCC_C2_VREF_CLK_SLEEP_DISABLE()             (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_VREFLPEN)
#define __HAL_RCC_C2_SAI4_CLK_SLEEP_DISABLE()             (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_SAI4LPEN)
#define __HAL_RCC_C2_RTC_CLK_SLEEP_DISABLE()              (RCC_C2->APB4LPENR) &= ~ (RCC_APB4LPENR_RTCAPBLPEN)

#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
/** @brief  Enable or disable peripheral bus clock  when D3 domain is in DRUN
  * @note   After reset (default config), peripheral clock is disabled when both CPUs are in CSTOP
  */
#else
/** @brief  Enable or disable peripheral bus clock  when D3 domain is in DRUN
  * @note   After reset (default config), peripheral clock is disabled when CPU is in CSTOP
  */
#endif /*DUAL_CORE*/

#if defined(RCC_D3AMR_BDMAAMEN)
#define __HAL_RCC_BDMA_CLKAM_ENABLE()             (RCC->D3AMR) |= (RCC_D3AMR_BDMAAMEN)
#endif
#if defined(RCC_D3AMR_LPUART1AMEN)
#define __HAL_RCC_LPUART1_CLKAM_ENABLE()          (RCC->D3AMR) |= (RCC_D3AMR_LPUART1AMEN)
#endif
#if defined(RCC_D3AMR_SPI6AMEN)
#define __HAL_RCC_SPI6_CLKAM_ENABLE()             (RCC->D3AMR) |= (RCC_D3AMR_SPI6AMEN)
#endif
#if defined(RCC_D3AMR_I2C4AMEN)
#define __HAL_RCC_I2C4_CLKAM_ENABLE()             (RCC->D3AMR) |= (RCC_D3AMR_I2C4AMEN)
#endif
#if defined(RCC_D3AMR_LPTIM2AMEN)
#define __HAL_RCC_LPTIM2_CLKAM_ENABLE()           (RCC->D3AMR) |= (RCC_D3AMR_LPTIM2AMEN)
#endif
#if defined(RCC_D3AMR_LPTIM3AMEN)
#define __HAL_RCC_LPTIM3_CLKAM_ENABLE()           (RCC->D3AMR) |= (RCC_D3AMR_LPTIM3AMEN)
#endif
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_CLKAM_ENABLE()           (RCC->D3AMR) |= (RCC_D3AMR_LPTIM4AMEN)
#endif
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_CLKAM_ENABLE()           (RCC->D3AMR) |= (RCC_D3AMR_LPTIM5AMEN)
#endif
#if defined(RCC_D3AMR_COMP12AMEN)
#define __HAL_RCC_COMP12_CLKAM_ENABLE()           (RCC->D3AMR) |= (RCC_D3AMR_COMP12AMEN)
#endif
#if defined(RCC_D3AMR_VREFAMEN)
#define __HAL_RCC_VREF_CLKAM_ENABLE()             (RCC->D3AMR) |= (RCC_D3AMR_VREFAMEN)
#endif
#if defined(RCC_D3AMR_RTCAMEN)
#define __HAL_RCC_RTC_CLKAM_ENABLE()              (RCC->D3AMR) |= (RCC_D3AMR_RTCAMEN)
#endif
#if defined(RCC_D3AMR_CRCAMEN)
#define __HAL_RCC_CRC_CLKAM_ENABLE()              (RCC->D3AMR) |= (RCC_D3AMR_CRCAMEN)
#endif
#if defined(SAI4)
#define __HAL_RCC_SAI4_CLKAM_ENABLE()             (RCC->D3AMR) |= (RCC_D3AMR_SAI4AMEN)
#endif
#if defined(ADC3)
#define __HAL_RCC_ADC3_CLKAM_ENABLE()             (RCC->D3AMR) |= (RCC_D3AMR_ADC3AMEN)
#endif
#if defined(RCC_D3AMR_BKPRAMAMEN)
#define __HAL_RCC_BKPRAM_CLKAM_ENABLE()           (RCC->D3AMR) |= (RCC_D3AMR_BKPRAMAMEN)
#endif
#if defined(RCC_D3AMR_SRAM4AMEN)
#define __HAL_RCC_D3SRAM1_CLKAM_ENABLE()          (RCC->D3AMR) |= (RCC_D3AMR_SRAM4AMEN)
#endif

#if defined(BDMA2)
#define __HAL_RCC_BDMA2_CLKAM_ENABLE()            (RCC->SRDAMR) |= (RCC_SRDAMR_BDMA2AMEN)
#endif
#if defined(RCC_SRDAMR_GPIOAMEN)
#define __HAL_RCC_GPIO_CLKAM_ENABLE()             (RCC->SRDAMR) |= (RCC_SRDAMR_GPIOAMEN)
#endif
#if defined(RCC_SRDAMR_LPUART1AMEN)
#define __HAL_RCC_LPUART1_CLKAM_ENABLE()          (RCC->SRDAMR) |= (RCC_SRDAMR_LPUART1AMEN)
#endif
#if defined(RCC_SRDAMR_SPI6AMEN)
#define __HAL_RCC_SPI6_CLKAM_ENABLE()             (RCC->SRDAMR) |= (RCC_SRDAMR_SPI6AMEN)
#endif
#if defined(RCC_SRDAMR_I2C4AMEN)
#define __HAL_RCC_I2C4_CLKAM_ENABLE()             (RCC->SRDAMR) |= (RCC_SRDAMR_I2C4AMEN)
#endif
#if defined(RCC_SRDAMR_LPTIM2AMEN)
#define __HAL_RCC_LPTIM2_CLKAM_ENABLE()           (RCC->SRDAMR) |= (RCC_SRDAMR_LPTIM2AMEN)
#endif
#if defined(RCC_SRDAMR_LPTIM3AMEN)
#define __HAL_RCC_LPTIM3_CLKAM_ENABLE()           (RCC->SRDAMR) |= (RCC_SRDAMR_LPTIM3AMEN)
#endif
#if defined(DAC2)
#define __HAL_RCC_DAC2_CLKAM_ENABLE()             (RCC->SRDAMR) |= (RCC_SRDAMR_DAC2AMEN)
#endif
#if defined(RCC_SRDAMR_COMP12AMEN)
#define __HAL_RCC_COMP12_CLKAM_ENABLE()           (RCC->SRDAMR) |= (RCC_SRDAMR_COMP12AMEN)
#endif
#if defined(RCC_SRDAMR_VREFAMEN)
#define __HAL_RCC_VREF_CLKAM_ENABLE()             (RCC->SRDAMR) |= (RCC_SRDAMR_VREFAMEN)
#endif
#if defined(RCC_SRDAMR_RTCAMEN)
#define __HAL_RCC_RTC_CLKAM_ENABLE()              (RCC->SRDAMR) |= (RCC_SRDAMR_RTCAMEN)
#endif
#if defined(RCC_SRDAMR_DTSAMEN)
#define __HAL_RCC_DTS_CLKAM_ENABLE()              (RCC->SRDAMR) |= (RCC_SRDAMR_DTSAMEN)
#endif
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_CLKAM_ENABLE()           (RCC->SRDAMR) |= (RCC_SRDAMR_DFSDM2AMEN)
#endif
#if defined(RCC_SRDAMR_BKPRAMAMEN)
#define __HAL_RCC_BKPRAM_CLKAM_ENABLE()           (RCC->SRDAMR) |= (RCC_SRDAMR_BKPRAMAMEN)
#endif
#if defined(RCC_SRDAMR_SRDSRAMAMEN)
#define __HAL_RCC_SRDSRAM_CLKAM_ENABLE()          (RCC->SRDAMR) |= (RCC_SRDAMR_SRDSRAMAMEN)
#endif

#if defined(RCC_D3AMR_BDMAAMEN)
#define __HAL_RCC_BDMA_CLKAM_DISABLE()             (RCC->D3AMR) &= ~ (RCC_D3AMR_BDMAAMEN)
#endif
#if defined(RCC_D3AMR_LPUART1AMEN)
#define __HAL_RCC_LPUART1_CLKAM_DISABLE()          (RCC->D3AMR) &= ~ (RCC_D3AMR_LPUART1AMEN)
#endif
#if defined(RCC_D3AMR_SPI6AMEN)
#define __HAL_RCC_SPI6_CLKAM_DISABLE()             (RCC->D3AMR) &= ~ (RCC_D3AMR_SPI6AMEN)
#endif
#if defined(RCC_D3AMR_I2C4AMEN)
#define __HAL_RCC_I2C4_CLKAM_DISABLE()             (RCC->D3AMR) &= ~ (RCC_D3AMR_I2C4AMEN)
#endif
#if defined(RCC_D3AMR_LPTIM2AMEN)
#define __HAL_RCC_LPTIM2_CLKAM_DISABLE()           (RCC->D3AMR) &= ~ (RCC_D3AMR_LPTIM2AMEN)
#endif
#if defined(RCC_D3AMR_LPTIM3AMEN)
#define __HAL_RCC_LPTIM3_CLKAM_DISABLE()           (RCC->D3AMR) &= ~ (RCC_D3AMR_LPTIM3AMEN)
#endif
#if defined(LPTIM4)
#define __HAL_RCC_LPTIM4_CLKAM_DISABLE()           (RCC->D3AMR) &= ~ (RCC_D3AMR_LPTIM4AMEN)
#endif
#if defined(LPTIM5)
#define __HAL_RCC_LPTIM5_CLKAM_DISABLE()           (RCC->D3AMR) &= ~ (RCC_D3AMR_LPTIM5AMEN)
#endif
#if defined(RCC_D3AMR_COMP12AMEN)
#define __HAL_RCC_COMP12_CLKAM_DISABLE()           (RCC->D3AMR) &= ~ (RCC_D3AMR_COMP12AMEN)
#endif
#if defined(RCC_D3AMR_VREFAMEN)
#define __HAL_RCC_VREF_CLKAM_DISABLE()             (RCC->D3AMR) &= ~ (RCC_D3AMR_VREFAMEN)
#endif
#if defined(RCC_D3AMR_RTCAMEN)
#define __HAL_RCC_RTC_CLKAM_DISABLE()              (RCC->D3AMR) &= ~ (RCC_D3AMR_RTCAMEN)
#endif
#if defined(RCC_D3AMR_CRCAMEN)
#define __HAL_RCC_CRC_CLKAM_DISABLE()              (RCC->D3AMR) &= ~ (RCC_D3AMR_CRCAMEN)
#endif
#if defined(SAI4)
#define __HAL_RCC_SAI4_CLKAM_DISABLE()             (RCC->D3AMR) &= ~ (RCC_D3AMR_SAI4AMEN)
#endif
#if defined(ADC3)
#define __HAL_RCC_ADC3_CLKAM_DISABLE()             (RCC->D3AMR) &= ~ (RCC_D3AMR_ADC3AMEN)
#endif
#if defined(RCC_D3AMR_BKPRAMAMEN)
#define __HAL_RCC_BKPRAM_CLKAM_DISABLE()           (RCC->D3AMR) &= ~ (RCC_D3AMR_BKPRAMAMEN)
#endif
#if defined(RCC_D3AMR_SRAM4AMEN)
#define __HAL_RCC_D3SRAM1_CLKAM_DISABLE()          (RCC->D3AMR)&= ~ (RCC_D3AMR_SRAM4AMEN)
#endif

#if defined(BDMA2)
#define __HAL_RCC_BDMA2_CLKAM_DISABLE()            (RCC->SRDAMR) &= ~ (RCC_SRDAMR_BDMA2AMEN)
#endif
#if defined(RCC_SRDAMR_GPIOAMEN)
#define __HAL_RCC_GPIO_CLKAM_DISABLE()             (RCC->SRDAMR) &= ~ (RCC_SRDAMR_GPIOAMEN)
#endif
#if defined(RCC_SRDAMR_LPUART1AMEN)
#define __HAL_RCC_LPUART1_CLKAM_DISABLE()          (RCC->SRDAMR) &= ~ (RCC_SRDAMR_LPUART1AMEN)
#endif
#if defined(RCC_SRDAMR_SPI6AMEN)
#define __HAL_RCC_SPI6_CLKAM_DISABLE()             (RCC->SRDAMR) &= ~ (RCC_SRDAMR_SPI6AMEN)
#endif
#if defined(RCC_SRDAMR_I2C4AMEN)
#define __HAL_RCC_I2C4_CLKAM_DISABLE()             (RCC->SRDAMR) &= ~ (RCC_SRDAMR_I2C4AMEN)
#endif
#if defined(RCC_SRDAMR_LPTIM2AMEN)
#define __HAL_RCC_LPTIM2_CLKAM_DISABLE()           (RCC->SRDAMR) &= ~ (RCC_SRDAMR_LPTIM2AMEN)
#endif
#if defined(RCC_SRDAMR_LPTIM3AMEN)
#define __HAL_RCC_LPTIM3_CLKAM_DISABLE()           (RCC->SRDAMR) &= ~ (RCC_SRDAMR_LPTIM3AMEN)
#endif
#if defined(RCC_SRDAMR_DAC2AMEN)
#define __HAL_RCC_DAC2_CLKAM_DISABLE()             (RCC->SRDAMR) &= ~ (RCC_SRDAMR_DAC2AMEN)
#endif
#if defined(RCC_SRDAMR_COMP12AMEN)
#define __HAL_RCC_COMP12_CLKAM_DISABLE()           (RCC->SRDAMR) &= ~ (RCC_SRDAMR_COMP12AMEN)
#endif
#if defined(RCC_SRDAMR_VREFAMEN)
#define __HAL_RCC_VREF_CLKAM_DISABLE()             (RCC->SRDAMR) &= ~ (RCC_SRDAMR_VREFAMEN)
#endif
#if defined(RCC_SRDAMR_RTCAMEN)
#define __HAL_RCC_RTC_CLKAM_DISABLE()              (RCC->SRDAMR) &= ~(RCC_SRDAMR_RTCAMEN)
#endif
#if defined(RCC_SRDAMR_DTSAMEN)
#define __HAL_RCC_DTS_CLKAM_DISABLE()              (RCC->SRDAMR) &= ~(RCC_SRDAMR_DTSAMEN)
#endif
#if defined(DFSDM2_BASE)
#define __HAL_RCC_DFSDM2_CLKAM_DISABLE()           (RCC->SRDAMR) &= ~(RCC_SRDAMR_DFSDM2AMEN)
#endif
#if defined(RCC_SRDAMR_BKPRAMAMEN)
#define __HAL_RCC_BKPRAM_CLKAM_DISABLE()           (RCC->SRDAMR) &= ~ (RCC_SRDAMR_BKPRAMAMEN)
#endif
#if defined(RCC_SRDAMR_SRDSRAMAMEN)
#define __HAL_RCC_SRDSRAM_CLKAM_DISABLE()          (RCC->SRDAMR) &= ~ (RCC_SRDAMR_SRDSRAMAMEN)
#endif


#if defined(RCC_CKGAENR_AXICKG)
    /** @brief  Macro to enable or disable the RCC_CKGAENR bits (AXI clocks gating enable register).
  * @note
  * @note
  * @note
  * @param
  * @note
  */

#define __HAL_RCC_AXI_CLKGA_ENABLE()             (RCC->CKGAENR) |= (RCC_CKGAENR_AXICKG)
#define __HAL_RCC_AHB_CLKGA_ENABLE()             (RCC->CKGAENR) |= (RCC_CKGAENR_AHBCKG)
#define __HAL_RCC_CPU_CLKGA_ENABLE()             (RCC->CKGAENR) |= (RCC_CKGAENR_CPUCKG)
#define __HAL_RCC_SDMMC_CLKGA_ENABLE()           (RCC->CKGAENR) |= (RCC_CKGAENR_SDMMCCKG)
#define __HAL_RCC_MDMA_CLKGA_ENABLE()            (RCC->CKGAENR) |= (RCC_CKGAENR_MDMACKG)
#define __HAL_RCC_DMA2D_CLKGA_ENABLE()           (RCC->CKGAENR) |= (RCC_CKGAENR_DMA2DCKG)
#define __HAL_RCC_LTDC_CLKGA_ENABLE()            (RCC->CKGAENR) |= (RCC_CKGAENR_LTDCCKG)
#define __HAL_RCC_GFXMMUM_CLKGA_ENABLE()         (RCC->CKGAENR) |= (RCC_CKGAENR_GFXMMUMCKG)
#define __HAL_RCC_AHB12_CLKGA_ENABLE()           (RCC->CKGAENR) |= (RCC_CKGAENR_AHB12CKG)
#define __HAL_RCC_AHB34_CLKGA_ENABLE()           (RCC->CKGAENR) |= (RCC_CKGAENR_AHB34CKG)
#define __HAL_RCC_FLIFT_CLKGA_ENABLE()           (RCC->CKGAENR) |= (RCC_CKGAENR_FLIFTCKG)
#define __HAL_RCC_OCTOSPI2_CLKGA_ENABLE()        (RCC->CKGAENR) |= (RCC_CKGAENR_OCTOSPI2CKG)
#define __HAL_RCC_FMC_CLKGA_ENABLE()             (RCC->CKGAENR) |= (RCC_CKGAENR_FMCCKG)
#define __HAL_RCC_OCTOSPI1_CLKGA_ENABLE()        (RCC->CKGAENR) |= (RCC_CKGAENR_OCTOSPI1CKG)
#define __HAL_RCC_AXIRAM1_CLKGA_ENABLE()         (RCC->CKGAENR) |= (RCC_CKGAENR_AXIRAM1CKG)
#define __HAL_RCC_AXIRAM2_CLKGA_ENABLE()         (RCC->CKGAENR) |= (RCC_CKGAENR_AXIRAM2CKG)
#define __HAL_RCC_AXIRAM3_CLKGA_ENABLE()         (RCC->CKGAENR) |= (RCC_CKGAENR_AXIRAM3CKG)
#define __HAL_RCC_GFXMMUS_CLKGA_ENABLE()         (RCC->CKGAENR) |= (RCC_CKGAENR_GFXMMUSCKG)
#define __HAL_RCC_ECCRAM_CLKGA_ENABLE()          (RCC->CKGAENR) |= (RCC_CKGAENR_ECCRAMCKG)
#define __HAL_RCC_EXTI_CLKGA_ENABLE()            (RCC->CKGAENR) |= (RCC_CKGAENR_EXTICKG)
#define __HAL_RCC_JTAG_CLKGA_ENABLE()            (RCC->CKGAENR) |= (RCC_CKGAENR_JTAGCKG)


#define __HAL_RCC_AXI_CLKGA_DISABLE()             (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AXICKG)
#define __HAL_RCC_AHB_CLKGA_DISABLE()             (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AHBCKG)
#define __HAL_RCC_CPU_CLKGA_DISABLE()             (RCC->CKGAENR) &= ~ (RCC_CKGAENR_CPUCKG)
#define __HAL_RCC_SDMMC_CLKGA_DISABLE()           (RCC->CKGAENR) &= ~ (RCC_CKGAENR_SDMMCCKG)
#define __HAL_RCC_MDMA_CLKGA_DISABLE()            (RCC->CKGAENR) &= ~ (RCC_CKGAENR_MDMACKG)
#define __HAL_RCC_DMA2D_CLKGA_DISABLE()           (RCC->CKGAENR) &= ~ (RCC_CKGAENR_DMA2DCKG)
#define __HAL_RCC_LTDC_CLKGA_DISABLE()            (RCC->CKGAENR) &= ~ (RCC_CKGAENR_LTDCCKG)
#define __HAL_RCC_GFXMMUM_CLKGA_DISABLE()         (RCC->CKGAENR) &= ~ (RCC_CKGAENR_GFXMMUMCKG)
#define __HAL_RCC_AHB12_CLKGA_DISABLE()           (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AHB12CKG)
#define __HAL_RCC_AHB34_CLKGA_DISABLE()           (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AHB34CKG)
#define __HAL_RCC_FLIFT_CLKGA_DISABLE()           (RCC->CKGAENR) &= ~ (RCC_CKGAENR_FLIFTCKG)
#define __HAL_RCC_OCTOSPI2_CLKGA_DISABLE()        (RCC->CKGAENR) &= ~ (RCC_CKGAENR_OCTOSPI2CKG)
#define __HAL_RCC_FMC_CLKGA_DISABLE()             (RCC->CKGAENR) &= ~ (RCC_CKGAENR_FMCCKG)
#define __HAL_RCC_OCTOSPI1_CLKGA_DISABLE()        (RCC->CKGAENR) &= ~ (RCC_CKGAENR_OCTOSPI1CKG)
#define __HAL_RCC_AXIRAM1_CLKGA_DISABLE()         (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AXIRAM1CKG)
#define __HAL_RCC_AXIRAM2_CLKGA_DISABLE()         (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AXIRAM2CKG)
#define __HAL_RCC_AXIRAM3_CLKGA_DISABLE()         (RCC->CKGAENR) &= ~ (RCC_CKGAENR_AXIRAM3CKG)
#define __HAL_RCC_GFXMMUS_CLKGA_DISABLE()         (RCC->CKGAENR) &= ~ (RCC_CKGAENR_GFXMMUSCKG)
#define __HAL_RCC_ECCRAM_CLKGA_DISABLE()          (RCC->CKGAENR) &= ~ (RCC_CKGAENR_ECCRAMCKG)
#define __HAL_RCC_EXTI_CLKGA_DISABLE()            (RCC->CKGAENR) &= ~ (RCC_CKGAENR_EXTICKG)
#define __HAL_RCC_JTAG_CLKGA_DISABLE()            (RCC->CKGAENR) &= ~ (RCC_CKGAENR_JTAGCKG)

#endif /* RCC_CKGAENR_AXICKG */




/** @brief  Macro to enable or disable the Internal High Speed oscillator (HSI).
  * @note     After enabling the HSI, the application software should wait on
  *           HSIRDY flag to be set indicating that HSI clock is stable and can
  *           be used to clock the PLL and/or system clock.
  * @note     HSI can not be stopped if it is used directly or through the PLL
  *           as system clock. In this case, you have to select another source
  *           of the system clock then stop the HSI.
  * @note     The HSI is stopped by hardware when entering STOP and STANDBY modes.
  * @param    __STATE__ specifies the new state of the HSI.
  *           This parameter can be one of the following values:
  *            @arg RCC_HSI_OFF turn OFF the HSI oscillator
  *            @arg RCC_HSI_ON turn ON the HSI oscillator
  *            @arg RCC_HSI_DIV1 turn ON the HSI oscillator and divide it by 1 (default after reset)
  *            @arg RCC_HSI_DIV2 turn ON the HSI oscillator and divide it by 2
  *            @arg RCC_HSI_DIV4 turn ON the HSI oscillator and divide it by 4
  *            @arg RCC_HSI_DIV8 turn ON the HSI oscillator and divide it by 8
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.
  */
#define __HAL_RCC_HSI_CONFIG(__STATE__) \
                  MODIFY_REG(RCC->CR, RCC_CR_HSION | RCC_CR_HSIDIV , (uint32_t)(__STATE__))


/** @brief  Macro to get the HSI divider.
  * @retval The HSI divider. The returned value can be one
  *         of the following:
  *            - RCC_CR_HSIDIV_1  HSI oscillator divided by 1 (default after reset)
  *            - RCC_CR_HSIDIV_2  HSI oscillator divided by 2
  *            - RCC_CR_HSIDIV_4  HSI oscillator divided by 4
  *            - RCC_CR_HSIDIV_8  HSI oscillator divided by 8
  */
#define __HAL_RCC_GET_HSI_DIVIDER() ((uint32_t)(READ_BIT(RCC->CR, RCC_CR_HSIDIV)))

/** @brief  Macros to enable or disable the Internal High Speed oscillator (HSI).
  * @note   The HSI is stopped by hardware when entering STOP and STANDBY modes.
  *         It is used (enabled by hardware) as system clock source after start-up
  *         from Reset, wakeup from STOP and STANDBY mode, or in case of failure
  *         of the HSE used directly or indirectly as system clock (if the Clock
  *         Security System CSS is enabled).
  * @note   HSI can not be stopped if it is used as system clock source. In this case,
  *         you have to select another source of the system clock then stop the HSI.
  * @note   After enabling the HSI, the application software should wait on HSIRDY
  *         flag to be set indicating that HSI clock is stable and can be used as
  *         system clock source.
  *         This parameter can be: ENABLE or DISABLE.
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.
  */
#define __HAL_RCC_HSI_ENABLE()  SET_BIT(RCC->CR, RCC_CR_HSION)
#define __HAL_RCC_HSI_DISABLE() CLEAR_BIT(RCC->CR, RCC_CR_HSION)


/** @brief  Macro to adjust the Internal High Speed oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  __HSICalibrationValue__: specifies the calibration trimming value.
  *         This parameter must be a number between 0 and 0x7F (3F for Rev Y device).
  */
#if defined(RCC_VER_X)
#define __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(__HSICalibrationValue__)                                                                   \
           do {                                                                                                                          \
              if(HAL_GetREVID() <= REV_ID_Y)                                                                                             \
             {                                                                                                                           \
               MODIFY_REG(RCC->HSICFGR, HAL_RCC_REV_Y_HSITRIM_Msk, (uint32_t)(__HSICalibrationValue__) << HAL_RCC_REV_Y_HSITRIM_Pos);    \
             }                                                                                                                           \
             else                                                                                                                        \
             {                                                                                                                           \
               MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSITRIM, (uint32_t)(__HSICalibrationValue__) << RCC_HSICFGR_HSITRIM_Pos);            \
             }                                                                                                                           \
           } while(0)

#else
#define __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(__HSICalibrationValue__)                                                                   \
               MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSITRIM, (uint32_t)(__HSICalibrationValue__) << RCC_HSICFGR_HSITRIM_Pos);
#endif /*RCC_VER_X*/
/**
  * @brief    Macros to enable or disable the force of the Internal High Speed oscillator (HSI)
  *           in STOP mode to be quickly available as kernel clock for some peripherals.
  * @note     Keeping the HSI ON in STOP mode allows to avoid slowing down the communication
  *           speed because of the HSI start-up time.
  * @note     The enable of this function has not effect on the HSION bit.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define __HAL_RCC_HSISTOP_ENABLE()     SET_BIT(RCC->CR, RCC_CR_HSIKERON)
#define __HAL_RCC_HSISTOP_DISABLE()    CLEAR_BIT(RCC->CR, RCC_CR_HSIKERON)


/**
  * @brief  Macro to enable or disable the Internal High Speed oscillator for USB (HSI48).
  * @note   After enabling the HSI48, the application software should wait on
  *         HSI48RDY flag to be set indicating that HSI48 clock is stable and can
  *         be used to clock the USB.
  * @note   The HSI48 is stopped by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_HSI48_ENABLE()    SET_BIT(RCC->CR, RCC_CR_HSI48ON);

#define __HAL_RCC_HSI48_DISABLE()   CLEAR_BIT(RCC->CR, RCC_CR_HSI48ON);

/**
  * @brief  Macros to enable or disable the Internal  oscillator (CSI).
  * @note     The CSI is stopped by hardware when entering STOP and STANDBY modes.
  *           It is used (enabled by hardware) as system clock source after
  *           start-up from Reset, wakeup from STOP and STANDBY mode, or in case
  *           of failure of the HSE used directly or indirectly as system clock
  *           (if the Clock Security System CSS is enabled).
  * @note     CSI can not be stopped if it is used as system clock source.
  *           In this case, you have to select another source of the system
  *           clock then stop the CSI.
  * @note     After enabling the CSI, the application software should wait on
  *           CSIRDY flag to be set indicating that CSI clock is stable and can
  *           be used as system clock source.
  * @note     When the CSI is stopped, CSIRDY flag goes low after 6 CSI oscillator
  *           clock cycles.
  */
#define __HAL_RCC_CSI_ENABLE()  SET_BIT(RCC->CR, RCC_CR_CSION)
#define __HAL_RCC_CSI_DISABLE() CLEAR_BIT(RCC->CR, RCC_CR_CSION)

/** @brief  Macro Adjusts the Internal  oscillator (CSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal CSI RC.
  * @param  __CSICalibrationValue__: specifies the calibration trimming value.
  *         This parameter must be a number between 0 and 0x1F.
  */
#if defined(RCC_VER_X)
#define __HAL_RCC_CSI_CALIBRATIONVALUE_ADJUST(__CSICalibrationValue__)                                                                   \
           do {                                                                                                                          \
             if(HAL_GetREVID() <= REV_ID_Y)                                                                                              \
             {                                                                                                                           \
               MODIFY_REG(RCC->HSICFGR, HAL_RCC_REV_Y_CSITRIM_Msk, (uint32_t)(__CSICalibrationValue__) << HAL_RCC_REV_Y_CSITRIM_Pos);    \
             }                                                                                                                           \
             else                                                                                                                        \
             {                                                                                                                           \
               MODIFY_REG(RCC->CSICFGR, RCC_CSICFGR_CSITRIM, (uint32_t)(__CSICalibrationValue__) << RCC_CSICFGR_CSITRIM_Pos);            \
             }                                                                                                                           \
           } while(0)

#else
#define __HAL_RCC_CSI_CALIBRATIONVALUE_ADJUST(__CSICalibrationValue__)                                                                   \
           do {                                                                                                                          \
               MODIFY_REG(RCC->CSICFGR, RCC_CSICFGR_CSITRIM, (uint32_t)(__CSICalibrationValue__) << RCC_CSICFGR_CSITRIM_Pos);            \
               } while(0)

#endif /*RCC_VER_X*/
/**
  * @brief    Macros to enable or disable the force of the Low-power Internal oscillator (CSI)
  *           in STOP mode to be quickly available as kernel clock for USARTs and I2Cs.
  * @note     Keeping the CSI ON in STOP mode allows to avoid slowing down the communication
  *           speed because of the CSI start-up time.
  * @note     The enable of this function has not effect on the CSION bit.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define __HAL_RCC_CSISTOP_ENABLE()     SET_BIT(RCC->CR, RCC_CR_CSIKERON)
#define __HAL_RCC_CSISTOP_DISABLE()    CLEAR_BIT(RCC->CR, RCC_CR_CSIKERON)


/** @brief  Macros to enable or disable the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if the IWDG is running.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles.
  */
#define __HAL_RCC_LSI_ENABLE()         SET_BIT(RCC->CSR, RCC_CSR_LSION)
#define __HAL_RCC_LSI_DISABLE()        CLEAR_BIT(RCC->CSR, RCC_CSR_LSION)

/**
  * @brief  Macro to configure the External High Speed oscillator (__HSE__).
  * @note   After enabling the HSE (RCC_HSE_ON or RCC_HSE_Bypass), the application
  *         software should wait on HSERDY flag to be set indicating that HSE clock
  *         is stable and can be used to clock the PLL and/or system clock.
  * @note   HSE state can not be changed if it is used directly or through the
  *         PLL as system clock. In this case, you have to select another source
  *         of the system clock then change the HSE state (ex. disable it).
  * @note   The HSE is stopped by hardware when entering STOP and STANDBY modes.
  * @note   This function reset the CSSON bit, so if the clock security system(CSS)
  *         was previously enabled you have to enable it again after calling this
  *         function.
  * @param  __STATE__: specifies the new state of the HSE.
  *         This parameter can be one of the following values:
  *            @arg RCC_HSE_OFF: turn OFF the HSE oscillator, HSERDY flag goes low after
  *                              6 HSE oscillator clock cycles.
  *            @arg RCC_HSE_ON: turn ON the HSE oscillator.
  *            @arg RCC_HSE_BYPASS: HSE oscillator bypassed with external clock.
  */
#if defined(RCC_CR_HSEEXT)
#define __HAL_RCC_HSE_CONFIG(__STATE__)                                \
                    do {                                               \
                      if ((__STATE__) == RCC_HSE_ON)                   \
                      {                                                \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);                \
                      }                                                \
                      else if ((__STATE__) == RCC_HSE_OFF)             \
                      {                                                \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);              \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEEXT);             \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);             \
                      }                                                \
                      else if ((__STATE__) == RCC_HSE_BYPASS)          \
                      {                                                \
                        SET_BIT(RCC->CR, RCC_CR_HSEBYP);               \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEEXT);             \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);                \
                      }                                                \
                      else if((__STATE__) == RCC_HSE_BYPASS_DIGITAL)   \
                      {                                                \
                        SET_BIT(RCC->CR, RCC_CR_HSEBYP);               \
                        SET_BIT(RCC->CR, RCC_CR_HSEEXT);               \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);                \
                      }                                                \
                      else                                             \
                      {                                                \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);              \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);             \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEEXT);             \
                      }                                                \
                    } while(0)
#else
#define __HAL_RCC_HSE_CONFIG(__STATE__)                         \
                    do {                                        \
                      if ((__STATE__) == RCC_HSE_ON)            \
                      {                                         \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);         \
                      }                                         \
                      else if ((__STATE__) == RCC_HSE_OFF)      \
                      {                                         \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);       \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);      \
                      }                                         \
                      else if ((__STATE__) == RCC_HSE_BYPASS)   \
                      {                                         \
                        SET_BIT(RCC->CR, RCC_CR_HSEBYP);        \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);         \
                      }                                         \
                      else                                      \
                      {                                         \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);       \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);      \
                      }                                         \
                    } while(0)
#endif /* RCC_CR_HSEEXT */

/** @defgroup RCC_LSE_Configuration LSE Configuration
  * @{
  */

/**
  * @brief  Macro to configure the External Low Speed oscillator (LSE).
  * @note   Transition LSE Bypass to LSE On and LSE On to LSE Bypass are not supported by this macro.
  *         User should request a transition to LSE Off first and then LSE On or LSE Bypass.
  * @note   The external input clock can have a frequency up to 1 MHz and be low swing (analog) or digital(*).
            A duty cycle close to 50% is recommended.
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).
  * @note   After enabling the LSE (RCC_LSE_ON, RCC_LSE_BYPASS or RCC_LSE_BYPASS_DIGITAL*), the application
  *         software should wait on LSERDY flag to be set indicating that LSE clock
  *         is stable and can be used to clock the RTC.
  * @note   If the RTC is used, the LSE bypass must not be configured in digital mode but in low swing analog mode (*)
  * @param  __STATE__: specifies the new state of the LSE.
  *         This parameter can be one of the following values:
  *            @arg RCC_LSE_OFF: turn OFF the LSE oscillator, LSERDY flag goes low after
  *                              6 LSE oscillator clock cycles.
  *            @arg RCC_LSE_ON: turn ON the LSE oscillator.
  *            @arg RCC_LSE_BYPASS: LSE oscillator bypassed with external clock.
  *            @arg RCC_LSE_BYPASS_DIGITAL: LSE oscillator bypassed with external digital clock. (*)
  *
  *         (*) Available on some STM32H7 lines only.
  */
#if defined(RCC_BDCR_LSEEXT)
#define __HAL_RCC_LSE_CONFIG(__STATE__) \
                    do {                                               \
                      if((__STATE__) == RCC_LSE_ON)                    \
                      {                                                \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);            \
                      }                                                \
                      else if((__STATE__) == RCC_LSE_OFF)              \
                      {                                                \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);          \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEEXT);         \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);         \
                      }                                                \
                      else if((__STATE__) == RCC_LSE_BYPASS)           \
                      {                                                \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);           \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEEXT);         \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);            \
                      }                                                \
                      else if((__STATE__) == RCC_LSE_BYPASS_DIGITAL)   \
                      {                                                \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);           \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEEXT);           \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);            \
                      }                                                \
                      else                                             \
                      {                                                \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);          \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);         \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEEXT);         \
                      }                                                \
                    } while(0)
#else

#define __HAL_RCC_LSE_CONFIG(__STATE__) \
                    do {                                       \
                      if((__STATE__) == RCC_LSE_ON)            \
                      {                                        \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);    \
                      }                                        \
                      else if((__STATE__) == RCC_LSE_OFF)      \
                      {                                        \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);  \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP); \
                      }                                        \
                      else if((__STATE__) == RCC_LSE_BYPASS)   \
                      {                                        \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);   \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);    \
                      }                                        \
                      else                                     \
                      {                                        \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);  \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP); \
                      }                                        \
                    } while(0)

#endif /* RCC_BDCR_LSEEXT */
/**
  * @}
  */

/** @brief  Macros to enable or disable the the RTC clock.
  * @note   These macros must be used only after the RTC clock source was selected.
  */
#define __HAL_RCC_RTC_ENABLE()         SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN)
#define __HAL_RCC_RTC_DISABLE()        CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCEN)

/** @brief  Macros to configure the RTC clock (RTCCLK).
  * @note   As the RTC clock configuration bits are in the Backup domain and write
  *         access is denied to this domain after reset, you have to enable write
  *         access using the Power Backup Access macro before to configure
  *         the RTC clock source (to be done once after reset).
  * @note   Once the RTC clock is configured it can't be changed unless the
  *         Backup domain is reset using __HAL_RCC_BackupReset_RELEASE() macro, or by
  *         a Power On Reset (POR).
  * @param  __RTCCLKSource__: specifies the RTC clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_RTCCLKSOURCE_LSE: LSE selected as RTC clock.
  *            @arg RCC_RTCCLKSOURCE_LSI: LSI selected as RTC clock.
  *            @arg RCC_RTCCLKSOURCE_HSE_DIVx: HSE clock divided by x selected
  *                                            as RTC clock, where x:[2,31]
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in STOP and STANDBY modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source, the RTC
  *         cannot be used in STOP and STANDBY modes.
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *         RTC clock source).
  */
#define __HAL_RCC_RTC_CLKPRESCALER(__RTCCLKSource__) (((__RTCCLKSource__) & RCC_BDCR_RTCSEL) == RCC_BDCR_RTCSEL) ?    \
                                                 MODIFY_REG(RCC->CFGR, RCC_CFGR_RTCPRE, (((__RTCCLKSource__) & 0xFFFFCFFU) >> 4)) : CLEAR_BIT(RCC->CFGR, RCC_CFGR_RTCPRE)

#define __HAL_RCC_RTC_CONFIG(__RTCCLKSource__) do { __HAL_RCC_RTC_CLKPRESCALER(__RTCCLKSource__);    \
                                                    RCC->BDCR |= ((__RTCCLKSource__) & 0x00000FFFU);  \
                                                   } while (0)

#define  __HAL_RCC_GET_RTC_SOURCE() ((uint32_t)(READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL)))


/** @brief  Macros to force or release the Backup domain reset.
  * @note   This function resets the RTC peripheral (including the backup registers)
  *         and the RTC clock source selection in RCC_BDCR register.
  * @note   The BKPSRAM is not affected by this reset.
  */
#define __HAL_RCC_BACKUPRESET_FORCE()   SET_BIT(RCC->BDCR, RCC_BDCR_BDRST)
#define __HAL_RCC_BACKUPRESET_RELEASE() CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST)

/** @brief  Macros to enable or disable the main PLL.
  * @note   After enabling the main PLL, the application software should wait on
  *         PLLRDY flag to be set indicating that PLL clock is stable and can
  *         be used as system clock source.
  * @note   The main PLL can not be disabled if it is used as system clock source
  * @note   The main PLL is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLL_ENABLE()         SET_BIT(RCC->CR, RCC_CR_PLL1ON)
#define __HAL_RCC_PLL_DISABLE()        CLEAR_BIT(RCC->CR, RCC_CR_PLL1ON)

/**
  * @brief  Enables or disables each clock output (PLL_P_CLK, PLL_Q_CLK, PLL_R_CLK)
  * @note   Enabling/disabling  those Clocks can be done only when the PLL is disabled.
  *         This is mainly used to save Power.
  *        (The ck_pll_p of the System PLL cannot be stopped if used as System Clock).
  * @param  __RCC_PLL1ClockOut__: specifies the PLL clock to be outputted
  *          This parameter can be one of the following values:
  *            @arg RCC_PLL1_DIVP: This clock is used to generate system clock up to 400MHZ or 280MHZ(*)
  *            @arg RCC_PLL1_DIVQ: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *            @arg RCC_PLL1_DIVR: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define __HAL_RCC_PLLCLKOUT_ENABLE(__RCC_PLL1ClockOut__)   SET_BIT(RCC->PLLCFGR, (__RCC_PLL1ClockOut__))

#define __HAL_RCC_PLLCLKOUT_DISABLE(__RCC_PLL1ClockOut__)  CLEAR_BIT(RCC->PLLCFGR, (__RCC_PLL1ClockOut__))


/**
  * @brief  Enables or disables Fractional Part Of The Multiplication Factor of PLL1 VCO
  * @note   Enabling/disabling  Fractional Part can be any time  without the need to stop the PLL1
  * @retval None
  */
#define __HAL_RCC_PLLFRACN_ENABLE()   SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1FRACEN)

#define __HAL_RCC_PLLFRACN_DISABLE()  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1FRACEN)


/**
  * @brief  Macro to configures the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  *
  * @param  __RCC_PLLSOURCE__: specifies the PLL entry clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_PLLSOURCE_CSI: CSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSI: HSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSE: HSE oscillator clock selected as PLL clock entry
  * @note   This clock source (__RCC_PLLSource__) is common for the main PLL1 (main PLL) and PLL2 & PLL3 .
  *
  * @param  __PLLM1__: specifies the division factor for PLL VCO input clock
  *          This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 16 MHz.
  *
  * @param  __PLLN1__: specifies the multiplication factor for PLL VCO output clock
  *          This parameter must be a number between 4 and 512 or between 8 and 420(*).
  * @note   You have to set the PLLN parameter correctly to ensure that the VCO
  *         output frequency is between 150 and 420 MHz (when in medium VCO range) or
  *         between 192 and 836 MHZ or between 128 and 560 MHZ(*) (when in wide VCO range)
  *
  * @param  __PLLP1__: specifies the division factor for system  clock.
  *          This parameter must be a number between 2 and 128 (where odd numbers are not allowed)
  *
  * @param  __PLLQ1__: specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128
  *
  * @param  __PLLR1__: specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128
  *
  * @note   To insure an optimal behavior of the PLL when one of the post-divider (DIVP, DIVQ or DIVR)
  *         is not used, application shall clear the enable bit (DIVyEN) and assign lowest possible 
  *         value to  __PLL1P__, __PLL1Q__ or __PLL1R__ parameters.
  * @retval None
  *
  *  (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  */


#define __HAL_RCC_PLL_CONFIG(__RCC_PLLSOURCE__, __PLLM1__, __PLLN1__, __PLLP1__, __PLLQ1__,__PLLR1__ ) \
                  do{ MODIFY_REG(RCC->PLLCKSELR, (RCC_PLLCKSELR_PLLSRC | RCC_PLLCKSELR_DIVM1) , ((__RCC_PLLSOURCE__) | ( (__PLLM1__) <<4U)));  \
                      WRITE_REG (RCC->PLL1DIVR , ( (((__PLLN1__) - 1U )& RCC_PLL1DIVR_N1) | ((((__PLLP1__) -1U ) << 9U) & RCC_PLL1DIVR_P1) | \
                                ((((__PLLQ1__) -1U) << 16U)& RCC_PLL1DIVR_Q1) | ((((__PLLR1__) - 1U) << 24U)& RCC_PLL1DIVR_R1))); \
                    } while(0)


/** @brief  Macro to configure the PLLs clock source.
  * @note   This function must be used only when all PLLs are disabled.
  * @param  __PLLSOURCE__: specifies the PLLs entry clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_PLLSOURCE_CSI: CSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSI: HSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSE: HSE oscillator clock selected as PLL clock entry
  *
  */
#define __HAL_RCC_PLL_PLLSOURCE_CONFIG(__PLLSOURCE__) MODIFY_REG(RCC->PLLCKSELR, RCC_PLLCKSELR_PLLSRC, (__PLLSOURCE__))


/**
  * @brief  Macro to configures the main PLL clock Fractional Part Of The Multiplication Factor
  *
  * @note   These bits can be written at any time, allowing dynamic fine-tuning of the PLL1 VCO
  *
  * @param  __RCC_PLL1FRACN__: specifies Fractional Part Of The Multiplication Factor for PLL1 VCO
  *                            It should be a value between 0 and 8191
  * @note   Warning: The software has to set correctly these bits to insure that the VCO
  *                  output frequency is between its valid frequency range, which is:
  *                   192 to 836 MHz or 128 to 560 MHz(*) if PLL1VCOSEL = 0
  *                   150 to 420 MHz if PLL1VCOSEL = 1.
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
 #define  __HAL_RCC_PLLFRACN_CONFIG(__RCC_PLL1FRACN__) MODIFY_REG(RCC->PLL1FRACR, RCC_PLL1FRACR_FRACN1, (uint32_t)(__RCC_PLL1FRACN__) << RCC_PLL1FRACR_FRACN1_Pos)


/** @brief  Macro to select  the PLL1  reference frequency range.
  * @param  __RCC_PLL1VCIRange__: specifies the PLL1 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL1VCIRANGE_0: Range frequency is between 1 and 2 MHz
  *            @arg RCC_PLL1VCIRANGE_1: Range frequency is between 2 and 4 MHz
  *            @arg RCC_PLL1VCIRANGE_2: Range frequency is between 4 and 8 MHz
  *            @arg RCC_PLL1VCIRANGE_3: Range frequency is between 8 and 16 MHz
  * @retval None
  */
#define __HAL_RCC_PLL_VCIRANGE(__RCC_PLL1VCIRange__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL1RGE, (__RCC_PLL1VCIRange__))


/** @brief  Macro to select  the PLL1  reference frequency range.
  * @param  __RCC_PLL1VCORange__: specifies the PLL1 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL1VCOWIDE: Range frequency is between 192 and 836 MHz or between 128 to 560 MHz(*)
  *            @arg RCC_PLL1VCOMEDIUM: Range frequency is between 150 and 420 MHz
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define __HAL_RCC_PLL_VCORANGE(__RCC_PLL1VCORange__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL1VCOSEL, (__RCC_PLL1VCORange__))



/** @brief  Macro to get the clock source used as system clock.
  * @retval The clock source used as system clock. The returned value can be one
  *         of the following:
  *              - RCC_CFGR_SWS_CSI: CSI used as system clock.
  *              - RCC_CFGR_SWS_HSI: HSI used as system clock.
  *              - RCC_CFGR_SWS_HSE: HSE used as system clock.
  *              - RCC_CFGR_SWS_PLL: PLL used as system clock.
  */
#define __HAL_RCC_GET_SYSCLK_SOURCE() ((uint32_t)(RCC->CFGR & RCC_CFGR_SWS))


/**
  * @brief Macro to configure the system clock source.
  * @param __RCC_SYSCLKSOURCE__: specifies the system clock source.
  * This parameter can be one of the following values:
  *              - RCC_SYSCLKSOURCE_HSI: HSI oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_CSI: CSI oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_HSE: HSE oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_PLLCLK: PLL output is used as system clock source.
  */
#define __HAL_RCC_SYSCLK_CONFIG(__RCC_SYSCLKSOURCE__) MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (__RCC_SYSCLKSOURCE__))

/** @brief  Macro to get the oscillator used as PLL clock source.
  * @retval The oscillator used as PLL clock source. The returned value can be one
  *         of the following:
  *              - RCC_PLLSOURCE_NONE: No oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_CSI: CSI oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_HSI: HSI oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_HSE: HSE oscillator is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL_OSCSOURCE() ((uint32_t)(RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC))

/** @defgroup RCCEx_MCOx_Clock_Config RCC Extended MCOx Clock Config
  * @{
  */

/** @brief  Macro to configure the MCO1 clock.
  * @param  __MCOCLKSOURCE__ specifies the MCO clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO1SOURCE_HSI: HSI clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_LSE: LSE clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_HSE: HSE clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_PLL1QCLK:  PLL1Q clock selected as MCO1 source
  *            @arg RCC_MCO1SOURCE_HSI48: HSI48 (48MHZ) selected as MCO1 source
  * @param  __MCODIV__ specifies the MCO clock prescaler.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCODIV_1 up to RCC_MCODIV_15  : divider applied to MCO1 clock
  */
#define __HAL_RCC_MCO1_CONFIG(__MCOCLKSOURCE__, __MCODIV__) \
                 MODIFY_REG(RCC->CFGR, (RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE), ((__MCOCLKSOURCE__) | (__MCODIV__)))

/** @brief  Macro to configure the MCO2 clock.
  * @param  __MCOCLKSOURCE__ specifies the MCO clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO2SOURCE_SYSCLK: System clock (SYSCLK) selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_PLL2PCLK: PLL2P clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_HSE: HSE clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_PLLCLK:  PLL1P clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_CSICLK:  CSI clock selected as MCO2 source
  *            @arg RCC_MCO2SOURCE_LSICLK:  LSI clock selected as MCO2 source
  * @param  __MCODIV__ specifies the MCO clock prescaler.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCODIV_1 up to RCC_MCODIV_15  : divider applied to MCO2 clock
  */
#define __HAL_RCC_MCO2_CONFIG(__MCOCLKSOURCE__, __MCODIV__) \
    MODIFY_REG(RCC->CFGR, (RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE), ((__MCOCLKSOURCE__) | ((__MCODIV__) << 7)));

/**
  * @}
  */

/**
  * @brief  Macro to configure the External Low Speed oscillator (LSE) drive capability.
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).
  * @note   On STM32H7 Rev.B and above devices this can't be updated while LSE is ON.
  * @param  __LSEDRIVE__: specifies the new state of the LSE drive capability.
  *          This parameter can be one of the following values:
  *            @arg RCC_LSEDRIVE_LOW: LSE oscillator low drive capability.
  *            @arg RCC_LSEDRIVE_MEDIUMLOW: LSE oscillator medium low drive capability.
  *            @arg RCC_LSEDRIVE_MEDIUMHIGH: LSE oscillator medium high drive capability.
  *            @arg RCC_LSEDRIVE_HIGH: LSE oscillator high drive capability.
  * @retval None
  */
#if defined(RCC_VER_X)
#define __HAL_RCC_LSEDRIVE_CONFIG(__LSEDRIVE__) \
           do{                                                                                                                                \
             if((HAL_GetREVID() <= REV_ID_Y) && (((__LSEDRIVE__) == RCC_LSEDRIVE_MEDIUMLOW) || ((__LSEDRIVE__) == RCC_LSEDRIVE_MEDIUMHIGH)))  \
             {                                                                                                                                \
              MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV, (~(uint32_t)(__LSEDRIVE__)) & RCC_BDCR_LSEDRV_Msk);                                      \
             }                                                                                                                                \
             else                                                                                                                             \
             {                                                                                                                                \
               MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV, (uint32_t)(__LSEDRIVE__));                                                              \
             }                                                                                                                                \
           } while(0)
#else
#define __HAL_RCC_LSEDRIVE_CONFIG(__LSEDRIVE__) \
               MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV, (uint32_t)(__LSEDRIVE__));
#endif /*RCC_VER_X*/
/**
  * @brief  Macro to configure the wake up from stop clock.
  * @param  __RCC_STOPWUCLK__: specifies the clock source used after wake up from stop
  *         This parameter can be one of the following values:
  *            @arg RCC_STOP_WAKEUPCLOCK_CSI: CSI selected as system clock source
  *            @arg RCC_STOP_WAKEUPCLOCK_HSI: HSI selected as system clock source
  * @retval None
  */
#define __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(__RCC_STOPWUCLK__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_STOPWUCK, (__RCC_STOPWUCLK__))

/**
  * @brief  Macro to configure the Kernel wake up from stop clock.
  * @param  __RCC_STOPKERWUCLK__: specifies the Kernel clock source used after wake up from stop
  *         This parameter can be one of the following values:
  *            @arg RCC_STOP_KERWAKEUPCLOCK_CSI: CSI selected as Kernel clock source
  *            @arg RCC_STOP_KERWAKEUPCLOCK_HSI: HSI selected as Kernel clock source
  * @retval None
  */
#define __HAL_RCC_KERWAKEUPSTOP_CLK_CONFIG(__RCC_STOPKERWUCLK__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_STOPKERWUCK, (__RCC_STOPKERWUCLK__))

/** @defgroup RCC_Flags_Interrupts_Management Flags Interrupts Management
  * @brief macros to manage the specified RCC Flags and interrupts.
  * @{
  */
/** @brief  Enable RCC interrupt.
  * @param  __INTERRUPT__: specifies the RCC interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_CSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_HSI48RDY: HSI48 ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLL2RDY: PLL2 ready interrupt
  *            @arg RCC_IT_PLL3RDY: PLL3 ready interrupt
  *            @arg RCC_IT_LSECSS: Clock security system interrupt
  */
#define __HAL_RCC_ENABLE_IT(__INTERRUPT__) SET_BIT(RCC->CIER, (__INTERRUPT__))

/** @brief Disable RCC interrupt
  * @param  __INTERRUPT__: specifies the RCC interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_CSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_HSI48RDY: HSI48 ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLL2RDY: PLL2 ready interrupt
  *            @arg RCC_IT_PLL3RDY: PLL3 ready interrupt
  *            @arg RCC_IT_LSECSS: Clock security system interrupt
  */
#define __HAL_RCC_DISABLE_IT(__INTERRUPT__) CLEAR_BIT(RCC->CIER, (__INTERRUPT__))

/** @brief  Clear the RCC's interrupt pending bits
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_CSIRDY: CSI ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_HSI48RDY: HSI48 ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLL2RDY: PLL2 ready interrupt
  *            @arg RCC_IT_PLL3RDY: PLL3 ready interrupt
  *            @arg RCC_IT_HSECSS: HSE Clock Security interrupt
  *            @arg RCC_IT_LSECSS: Clock security system interrupt
  */
#define __HAL_RCC_CLEAR_IT(__INTERRUPT__) (RCC->CICR = (__INTERRUPT__))

/** @brief  Check the RCC's interrupt has occurred or not.
  * @param  __INTERRUPT__: specifies the RCC interrupt source to check.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_CSIRDY: CSI ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_HSI48RDY: HSI48 ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLL2RDY: PLL2 ready interrupt
  *            @arg RCC_IT_PLL3RDY: PLL3 ready interrupt
  *            @arg RCC_IT_HSECSS: HSE Clock Security interrupt
  *            @arg RCC_IT_LSECSS: Clock security system interrupt
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_RCC_GET_IT(__INTERRUPT__) ((RCC->CIFR & (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief Set RMVF bit to clear the reset flags.
  */
#define __HAL_RCC_CLEAR_RESET_FLAGS() (RCC->RSR |= RCC_RSR_RMVF)

#if defined(DUAL_CORE)
#define __HAL_RCC_C1_CLEAR_RESET_FLAGS() (RCC_C1->RSR |= RCC_RSR_RMVF)

#define __HAL_RCC_C2_CLEAR_RESET_FLAGS() (RCC_C2->RSR |= RCC_RSR_RMVF)
#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
/** @brief  Check RCC flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
  *            @arg RCC_FLAG_HSIDIV: HSI divider flag
  *            @arg RCC_FLAG_CSIRDY: CSI oscillator clock ready
  *            @arg RCC_FLAG_HSI48RDY: HSI48 oscillator clock ready
  *            @arg RCC_FLAG_HSERDY:  HSE oscillator clock ready
  *            @arg RCC_FLAG_D1CKRDY:  Domain1 clock ready
  *            @arg RCC_FLAG_D2CKRDY:  Domain2 clock ready
  *            @arg RCC_FLAG_PLLRDY:  PLL1 clock ready
  *            @arg RCC_FLAG_PLL2RDY: PLL2 clock ready
  *            @arg RCC_FLAG_PLL3RDY: PLL3 clock ready
  *            @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
  *            @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *            @arg RCC_FLAG_C1RST:  CPU reset flag
  *            @arg RCC_FLAG_C2RST:  CPU2 reset flag
  *            @arg RCC_FLAG_D1RST:  D1 domain power switch reset flag
  *            @arg RCC_FLAG_D2RST:  D2 domain power switch reset flag
  *            @arg RCC_FLAG_BORRST: BOR reset flag
  *            @arg RCC_FLAG_PINRST: Pin reset
  *            @arg RCC_FLAG_PORRST: POR/PDR  reset
  *            @arg RCC_FLAG_SFTR1ST: System reset from CPU reset flag
  *            @arg RCC_FLAG_SFTR2ST: System reset from CPU2 reset flag
  *            @arg RCC_FLAG_BORRST:   D2 domain power switch reset flag
  *            @arg RCC_FLAG_IWDG1RST: CPU Independent Watchdog reset
  *            @arg RCC_FLAG_IWDG2RST: CPU2 Independent Watchdog reset
  *            @arg RCC_FLAG_WWDG2RST: Window Watchdog2 reset
  *            @arg RCC_FLAG_WWDG1RST: Window Watchdog1 reset
  *            @arg RCC_FLAG_LPWR1RST: Reset due to illegal D1 DSTANDBY or CPU CSTOP flag
  *            @arg RCC_FLAG_LPWR2RST: Reset due to illegal D2 DSTANDBY or CPU2 CSTOP flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define RCC_FLAG_MASK  ((uint8_t)0x1F)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : \
((((__FLAG__) >> 5U) == 3U)? RCC->CSR : ((((__FLAG__) >> 5U) == 4U)? RCC->RSR :RCC->CIFR))))  & (1U << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)

#define __HAL_RCC_C1_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : \
((((__FLAG__) >> 5U) == 3U)? RCC->CSR : ((((__FLAG__) >> 5U) == 4U)? RCC_C1->RSR :RCC->CIFR))))  & (1U << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)

#define __HAL_RCC_C2_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : \
((((__FLAG__) >> 5U) == 3U)? RCC->CSR : ((((__FLAG__) >> 5U) == 4U)? RCC_C2->RSR :RCC->CIFR))))  & (1U << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)

#else

/** @brief  Check RCC flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
  *            @arg RCC_FLAG_HSIDIV: HSI divider flag
  *            @arg RCC_FLAG_CSIRDY: CSI oscillator clock ready
  *            @arg RCC_FLAG_HSI48RDY: HSI48 oscillator clock ready
  *            @arg RCC_FLAG_HSERDY:  HSE oscillator clock ready
  *            @arg RCC_FLAG_D1CKRDY:  Domain1 clock ready (*)
  *            @arg RCC_FLAG_D2CKRDY:  Domain2 clock ready (*)
  *            @arg RCC_FLAG_CPUCKRDY: CPU Domain clock ready (CPU, APB3, bus matrix1 and related memories) (*)
  *            @arg RCC_FLAG_CDCKRDY:  CPU Domain clock ready (*)
  *            @arg RCC_FLAG_PLLRDY:  PLL1 clock ready
  *            @arg RCC_FLAG_PLL2RDY: PLL2 clock ready
  *            @arg RCC_FLAG_PLL3RDY: PLL3 clock ready
  *            @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
  *            @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *            @arg RCC_FLAG_CPURST: CPU reset flag
  *            @arg RCC_FLAG_D1RST:  D1 domain power switch reset flag (*)
  *            @arg RCC_FLAG_D2RST:  D2 domain power switch reset flag (*)
  *            @arg RCC_FLAG_CDRST:  CD domain power switch reset flag (*)
  *            @arg RCC_FLAG_BORRST: BOR reset flag
  *            @arg RCC_FLAG_PINRST: Pin reset
  *            @arg RCC_FLAG_PORRST: POR/PDR  reset
  *            @arg RCC_FLAG_SFTRST: System reset from CPU reset flag
  *            @arg RCC_FLAG_BORRST:   D2 domain power switch reset flag
  *            @arg RCC_FLAG_IWDG1RST: CPU Independent Watchdog reset
  *            @arg RCC_FLAG_WWDG1RST: Window Watchdog1 reset
  *            @arg RCC_FLAG_LPWR1RST: Reset due to illegal D1 DSTANDBY or CPU CSTOP flag
  *            @arg RCC_FLAG_LPWR2RST: Reset due to illegal D2 DSTANDBY flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  *
  *  (*) Available on some STM32H7 lines only.
  */
#define RCC_FLAG_MASK  ((uint8_t)0x1F)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : \
((((__FLAG__) >> 5U) == 3U)? RCC->CSR : ((((__FLAG__) >> 5U) == 4U)? RCC->RSR :RCC->CIFR))))  & (1UL << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)
#endif /*DUAL_CORE*/

/**
  * @}
  */

#define RCC_GET_PLL_OSCSOURCE() ((RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC) >> RCC_PLLCKSELR_PLLSRC_Pos)

/**
  * @}
  */

/* Include RCC HAL Extension module */
#include "stm32h7xx_hal_rcc_ex.h"

/* Exported functions --------------------------------------------------------*/
 /** @addtogroup RCC_Exported_Functions
  * @{
  */

/** @addtogroup RCC_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);

/**
  * @}
  */

/** @addtogroup RCC_Exported_Functions_Group2
  * @{
  */
/* Peripheral Control functions  ************************************************/
void     HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void     HAL_RCC_EnableCSS(void);
void     HAL_RCC_DisableCSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
void     HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void     HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);
/* CSS NMI IRQ handler */
void     HAL_RCC_NMI_IRQHandler(void);
/* User Callbacks in non blocking mode (IT mode) */
void     HAL_RCC_CCSCallback(void);

/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup RCC_Private_Constants RCC Private Constants
  * @{
  */

#define HSE_TIMEOUT_VALUE          HSE_STARTUP_TIMEOUT
#define HSI_TIMEOUT_VALUE          (2U)    /* 2 ms */
#define HSI48_TIMEOUT_VALUE        (2U)    /* 2 ms */
#define CSI_TIMEOUT_VALUE          (2U)    /* 2 ms */
#define LSI_TIMEOUT_VALUE          (2U)    /* 2 ms */
#define PLL_TIMEOUT_VALUE          (2U)    /* 2 ms */
#define CLOCKSWITCH_TIMEOUT_VALUE  (5000U) /* 5 s  */
#define RCC_DBP_TIMEOUT_VALUE      (100U)
#define RCC_LSE_TIMEOUT_VALUE      LSE_STARTUP_TIMEOUT

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup RCC_Private_Macros RCC Private Macros
  * @{
  */

/** @defgroup RCC_IS_RCC_Definitions RCC Private macros to check input parameters
  * @{
  */

#define IS_RCC_OSCILLATORTYPE(OSCILLATOR) (((OSCILLATOR) == RCC_OSCILLATORTYPE_NONE)                           || \
                                           (((OSCILLATOR) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE) || \
                                           (((OSCILLATOR) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI) || \
                                           (((OSCILLATOR) & RCC_OSCILLATORTYPE_CSI) == RCC_OSCILLATORTYPE_CSI) || \
                                           (((OSCILLATOR) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI) || \
                                           (((OSCILLATOR) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE) || \
                                           (((OSCILLATOR) & RCC_OSCILLATORTYPE_HSI48) == RCC_OSCILLATORTYPE_HSI48))

#if defined(RCC_CR_HSEEXT)
#define IS_RCC_HSE(HSE) (((HSE) == RCC_HSE_OFF) || ((HSE) == RCC_HSE_ON) || \
                         ((HSE) == RCC_HSE_BYPASS) || ((HSE) == RCC_HSE_BYPASS_DIGITAL))
#else
#define IS_RCC_HSE(HSE) (((HSE) == RCC_HSE_OFF) || ((HSE) == RCC_HSE_ON) || \
                         ((HSE) == RCC_HSE_BYPASS))
#endif /* RCC_CR_HSEEXT */

#if defined(RCC_BDCR_LSEEXT)
#define IS_RCC_LSE(LSE) (((LSE) == RCC_LSE_OFF) || ((LSE) == RCC_LSE_ON) || \
                         ((LSE) == RCC_LSE_BYPASS) || ((LSE) == RCC_LSE_BYPASS_DIGITAL))
#else
#define IS_RCC_LSE(LSE) (((LSE) == RCC_LSE_OFF) || ((LSE) == RCC_LSE_ON) || \
                         ((LSE) == RCC_LSE_BYPASS))
#endif /* RCC_BDCR_LSEEXT */

#define IS_RCC_HSI(HSI) (((HSI) == RCC_HSI_OFF) || ((HSI) == RCC_HSI_ON)    || \
                         ((HSI) == RCC_HSI_DIV1) || ((HSI) == RCC_HSI_DIV2) || \
                         ((HSI) == RCC_HSI_DIV4) || ((HSI) == RCC_HSI_DIV8))

#define IS_RCC_HSI48(HSI48) (((HSI48) == RCC_HSI48_OFF) || ((HSI48) == RCC_HSI48_ON))

#define IS_RCC_LSI(LSI) (((LSI) == RCC_LSI_OFF) || ((LSI) == RCC_LSI_ON))

#define IS_RCC_CSI(CSI) (((CSI) == RCC_CSI_OFF) || ((CSI) == RCC_CSI_ON))

#define IS_RCC_PLL(PLL) (((PLL) == RCC_PLL_NONE) ||((PLL) == RCC_PLL_OFF) || \
                         ((PLL) == RCC_PLL_ON))

#define IS_RCC_PLLSOURCE(SOURCE) (((SOURCE) == RCC_PLLSOURCE_CSI)  || \
                                  ((SOURCE) == RCC_PLLSOURCE_HSI)  || \
                                  ((SOURCE) == RCC_PLLSOURCE_NONE) || \
                                  ((SOURCE) == RCC_PLLSOURCE_HSE))
#define IS_RCC_PLLM_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 63U))
#if !defined(RCC_VER_2_0)
#define IS_RCC_PLLN_VALUE(VALUE) ((4U <= (VALUE)) && ((VALUE) <= 512U))
#else
#define IS_RCC_PLLN_VALUE(VALUE) ((8U <= (VALUE)) && ((VALUE) <= 420U))
#endif /* !RCC_VER_2_0 */
#define IS_RCC_PLLP_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))
#define IS_RCC_PLLQ_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))
#define IS_RCC_PLLR_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))

#define IS_RCC_PLLCLOCKOUT_VALUE(VALUE) (((VALUE) == RCC_PLL1_DIVP) || \
                                         ((VALUE) == RCC_PLL1_DIVQ) || \
                                         ((VALUE) == RCC_PLL1_DIVR))

#define IS_RCC_CLOCKTYPE(CLK) ((1U <= (CLK)) && ((CLK) <= 0x3FU))

#define IS_RCC_SYSCLKSOURCE(SOURCE) (((SOURCE) == RCC_SYSCLKSOURCE_CSI) || \
                                     ((SOURCE) == RCC_SYSCLKSOURCE_HSI) || \
                                     ((SOURCE) == RCC_SYSCLKSOURCE_HSE) || \
                                     ((SOURCE) == RCC_SYSCLKSOURCE_PLLCLK))

#define IS_RCC_SYSCLK(SYSCLK) (((SYSCLK) == RCC_SYSCLK_DIV1)   || ((SYSCLK) == RCC_SYSCLK_DIV2)   || \
                               ((SYSCLK) == RCC_SYSCLK_DIV4)   || ((SYSCLK) == RCC_SYSCLK_DIV8)   || \
                               ((SYSCLK) == RCC_SYSCLK_DIV16)  || ((SYSCLK) == RCC_SYSCLK_DIV64)  || \
                               ((SYSCLK) == RCC_SYSCLK_DIV128) || ((SYSCLK) == RCC_SYSCLK_DIV256) || \
                               ((SYSCLK) == RCC_SYSCLK_DIV512))


#define IS_RCC_HCLK(HCLK) (((HCLK) == RCC_HCLK_DIV1)   || ((HCLK) == RCC_HCLK_DIV2)   || \
                           ((HCLK) == RCC_HCLK_DIV4)   || ((HCLK) == RCC_HCLK_DIV8)   || \
                           ((HCLK) == RCC_HCLK_DIV16)  || ((HCLK) == RCC_HCLK_DIV64)  || \
                           ((HCLK) == RCC_HCLK_DIV128) || ((HCLK) == RCC_HCLK_DIV256) || \
                           ((HCLK) == RCC_HCLK_DIV512))

#define IS_RCC_CDPCLK1(CDPCLK1) (((CDPCLK1) == RCC_APB3_DIV1) || ((CDPCLK1) == RCC_APB3_DIV2) || \
                                 ((CDPCLK1) == RCC_APB3_DIV4) || ((CDPCLK1) == RCC_APB3_DIV8) || \
                                 ((CDPCLK1) == RCC_APB3_DIV16))

#define IS_RCC_D1PCLK1 IS_RCC_CDPCLK1  /* for legacy compatibility between H7 lines */

#define IS_RCC_PCLK1(PCLK1) (((PCLK1) == RCC_APB1_DIV1) || ((PCLK1) == RCC_APB1_DIV2) || \
                             ((PCLK1) == RCC_APB1_DIV4) || ((PCLK1) == RCC_APB1_DIV8) || \
                            ((PCLK1) == RCC_APB1_DIV16))

#define IS_RCC_PCLK2(PCLK2) (((PCLK2) == RCC_APB2_DIV1) || ((PCLK2) == RCC_APB2_DIV2) || \
                             ((PCLK2) == RCC_APB2_DIV4) || ((PCLK2) == RCC_APB2_DIV8) || \
                             ((PCLK2) == RCC_APB2_DIV16))

#define IS_RCC_SRDPCLK1(SRDPCLK1) (((SRDPCLK1) == RCC_APB4_DIV1) || ((SRDPCLK1) == RCC_APB4_DIV2) || \
                                  ((SRDPCLK1) == RCC_APB4_DIV4)  || ((SRDPCLK1) == RCC_APB4_DIV8) || \
                                  ((SRDPCLK1) == RCC_APB4_DIV16))

#define IS_RCC_D3PCLK1 IS_RCC_SRDPCLK1 /* for legacy compatibility between H7 lines*/

#define IS_RCC_RTCCLKSOURCE(SOURCE) (((SOURCE) == RCC_RTCCLKSOURCE_LSE)       || ((SOURCE) == RCC_RTCCLKSOURCE_LSI)       || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV2)  || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV3)  || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV4)  || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV5)  || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV6)  || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV7)  || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV8)  || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV9)  || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV10) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV11) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV12) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV13) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV14) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV15) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV16) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV17) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV18) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV19) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV20) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV21) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV22) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV23) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV24) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV25) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV26) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV27) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV28) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV29) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV30) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV31) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV32) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV33) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV34) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV35) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV36) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV37) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV38) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV39) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV40) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV41) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV42) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV43) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV44) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV45) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV46) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV47) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV48) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV49) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV50) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV51) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV52) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV53) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV54) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV55) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV56) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV57) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV58) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV59) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV60) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV61) || \
                                     ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV62) || ((SOURCE) == RCC_RTCCLKSOURCE_HSE_DIV63))

#define IS_RCC_MCO(MCOx) (((MCOx) == RCC_MCO1) || ((MCOx) == RCC_MCO2))

#define IS_RCC_MCO1SOURCE(SOURCE) (((SOURCE) == RCC_MCO1SOURCE_HSI) || ((SOURCE) == RCC_MCO1SOURCE_LSE)       || \
                                   ((SOURCE) == RCC_MCO1SOURCE_HSE) || ((SOURCE) == RCC_MCO1SOURCE_PLL1QCLK)  || \
                                   ((SOURCE) == RCC_MCO1SOURCE_HSI48))

#define IS_RCC_MCO2SOURCE(SOURCE) (((SOURCE) == RCC_MCO2SOURCE_SYSCLK)    || ((SOURCE) == RCC_MCO2SOURCE_PLL2PCLK) || \
                                   ((SOURCE) == RCC_MCO2SOURCE_HSE)       || ((SOURCE) == RCC_MCO2SOURCE_PLLCLK)   || \
                                   ((SOURCE) == RCC_MCO2SOURCE_CSICLK)    || ((SOURCE) == RCC_MCO2SOURCE_LSICLK))

#define IS_RCC_MCODIV(DIV) (((DIV) == RCC_MCODIV_1)  || ((DIV) == RCC_MCODIV_2)   || \
                            ((DIV) == RCC_MCODIV_3)  || ((DIV) == RCC_MCODIV_4)   || \
                            ((DIV) == RCC_MCODIV_5)  || ((DIV) == RCC_MCODIV_6)   || \
                            ((DIV) == RCC_MCODIV_7)  || ((DIV) == RCC_MCODIV_8)   || \
                            ((DIV) == RCC_MCODIV_9)  || ((DIV) == RCC_MCODIV_10)  || \
                            ((DIV) == RCC_MCODIV_11) || ((DIV) == RCC_MCODIV_12)  || \
                            ((DIV) == RCC_MCODIV_13) || ((DIV) == RCC_MCODIV_14)  || \
                            ((DIV) == RCC_MCODIV_15))

#if defined(DUAL_CORE)
#define IS_RCC_FLAG(FLAG) (((FLAG) == RCC_FLAG_HSIRDY)    || ((FLAG) == RCC_FLAG_CSIRDY)  || \
                           ((FLAG) == RCC_FLAG_HSI48RDY)  || ((FLAG) == RCC_FLAG_HSERDY)  || \
                           ((FLAG) == RCC_FLAG_D1CKRDY)   || ((FLAG) == RCC_FLAG_D2CKRDY) || \
                           ((FLAG) == RCC_FLAG_PLLRDY)    || ((FLAG) == RCC_FLAG_PLL2RDY) || \
                           ((FLAG) == RCC_FLAG_PLL3RDY)   || ((FLAG) == RCC_FLAG_LSERDY)  || \
                           ((FLAG) == RCC_FLAG_LSIRDY)    || \
                           ((FLAG) == RCC_FLAG_C1RST)     || ((FLAG) == RCC_FLAG_C2RST)   || \
                           ((FLAG) == RCC_FLAG_SFTR2ST)   || ((FLAG) == RCC_FLAG_WWDG2RST)|| \
                           ((FLAG) == RCC_FLAG_IWDG2RST)  || ((FLAG) == RCC_FLAG_D1RST)   || \
                           ((FLAG) == RCC_FLAG_D2RST)     || ((FLAG) == RCC_FLAG_BORRST)  || \
                           ((FLAG) == RCC_FLAG_PINRST)    || ((FLAG) == RCC_FLAG_PORRST)  || \
                           ((FLAG) == RCC_FLAG_SFTR1ST)   || ((FLAG) == RCC_FLAG_IWDG1RST)|| \
                           ((FLAG) == RCC_FLAG_WWDG1RST)  || ((FLAG) == RCC_FLAG_LPWR1RST)|| \
                           ((FLAG) == RCC_FLAG_LPWR2RST)  || ((FLAG) == RCC_FLAG_HSIDIV))

#else

#if defined(RCC_CR_D2CKRDY)
#define IS_RCC_FLAG(FLAG) (((FLAG) == RCC_FLAG_HSIRDY)    || ((FLAG) == RCC_FLAG_CSIRDY)  || \
                           ((FLAG) == RCC_FLAG_HSI48RDY)  || ((FLAG) == RCC_FLAG_HSERDY)  || \
                           ((FLAG) == RCC_FLAG_D1CKRDY)   || ((FLAG) == RCC_FLAG_D2CKRDY) || \
                           ((FLAG) == RCC_FLAG_PLLRDY)    || ((FLAG) == RCC_FLAG_PLL2RDY) || \
                           ((FLAG) == RCC_FLAG_PLL3RDY)   || ((FLAG) == RCC_FLAG_LSERDY)  || \
                           ((FLAG) == RCC_FLAG_LSIRDY)    || \
                           ((FLAG) == RCC_FLAG_CPURST)    || ((FLAG) == RCC_FLAG_D1RST)   || \
                           ((FLAG) == RCC_FLAG_D2RST)     || ((FLAG) == RCC_FLAG_BORRST)  || \
                           ((FLAG) == RCC_FLAG_PINRST)    || ((FLAG) == RCC_FLAG_PORRST)  || \
                           ((FLAG) == RCC_FLAG_SFTRST)    || ((FLAG) == RCC_FLAG_IWDG1RST)|| \
                           ((FLAG) == RCC_FLAG_WWDG1RST)  || ((FLAG) == RCC_FLAG_LPWR1RST)|| \
                           ((FLAG) == RCC_FLAG_LPWR2RST)  || ((FLAG) == RCC_FLAG_HSIDIV ))
#else
#define IS_RCC_FLAG(FLAG) (((FLAG) == RCC_FLAG_HSIRDY)    || ((FLAG) == RCC_FLAG_CSIRDY)  || \
                           ((FLAG) == RCC_FLAG_HSI48RDY)  || ((FLAG) == RCC_FLAG_HSERDY)  || \
                           ((FLAG) == RCC_FLAG_CPUCKRDY)  || ((FLAG) == RCC_FLAG_CDCKRDY) || \
                           ((FLAG) == RCC_FLAG_PLLRDY)    || ((FLAG) == RCC_FLAG_PLL2RDY) || \
                           ((FLAG) == RCC_FLAG_PLL3RDY)   || ((FLAG) == RCC_FLAG_LSERDY)  || \
                           ((FLAG) == RCC_FLAG_LSIRDY)    || \
                           ((FLAG) == RCC_FLAG_CDRST)     || ((FLAG) == RCC_FLAG_BORRST)  || \
                           ((FLAG) == RCC_FLAG_PINRST)    || ((FLAG) == RCC_FLAG_PORRST)  || \
                           ((FLAG) == RCC_FLAG_SFTRST)    || ((FLAG) == RCC_FLAG_IWDG1RST)|| \
                           ((FLAG) == RCC_FLAG_WWDG1RST)  || ((FLAG) == RCC_FLAG_LPWR1RST)|| \
                           ((FLAG) == RCC_FLAG_LPWR2RST)  || ((FLAG) == RCC_FLAG_HSIDIV ))
#endif /* RCC_CR_D2CKRDY */

#endif /*DUAL_CORE*/

#define IS_RCC_HSICALIBRATION_VALUE(VALUE) ((VALUE) <= 0x7FU)
#define IS_RCC_CSICALIBRATION_VALUE(VALUE) ((VALUE) <= 0x3FU)

#define IS_RCC_STOP_WAKEUPCLOCK(SOURCE) (((SOURCE) == RCC_STOP_WAKEUPCLOCK_CSI) || \
                                         ((SOURCE) == RCC_STOP_WAKEUPCLOCK_HSI))

#define IS_RCC_STOP_KERWAKEUPCLOCK(SOURCE) (((SOURCE) == RCC_STOP_KERWAKEUPCLOCK_CSI) || \
                                          ((SOURCE) == RCC_STOP_KERWAKEUPCLOCK_HSI))
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

#endif /* STM32H7xx_HAL_RCC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
