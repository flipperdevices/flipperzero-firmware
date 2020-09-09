/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rcc_ex.h
  * @author  MCD Application Team
  * @brief   Header file of RCC HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#ifndef STM32H7xx_HAL_RCC_EX_H
#define STM32H7xx_HAL_RCC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup RCCEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Types RCCEx Exported Types
  * @{
  */

/**
  * @brief  PLL2 Clock structure definition
  */
typedef struct
{

  uint32_t PLL2M;       /*!< PLL2M: Division factor for PLL2 VCO input clock.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 63    */

  uint32_t PLL2N;       /*!< PLL2N: Multiplication factor for PLL2 VCO output clock.
                             This parameter must be a number between Min_Data = 4 and Max_Data = 512
                             or between Min_Data = 8 and Max_Data = 420(*)
                             (*) : For stm32h7a3xx and stm32h7b3xx family lines.                       */   

  uint32_t PLL2P;       /*!< PLL2P: Division factor for system clock.
                             This parameter must be a number between Min_Data = 2 and Max_Data = 128
                             odd division factors are not allowed                                      */

  uint32_t PLL2Q;        /*!< PLL2Q: Division factor for peripheral clocks.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 128   */

  uint32_t PLL2R;        /*!< PLL2R: Division factor for peripheral clocks.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 128   */
  uint32_t PLL2RGE;      /*!<PLL2RGE: PLL2 clock Input range
                          This parameter must be a value of @ref RCC_PLL2_VCI_Range                    */
  uint32_t PLL2VCOSEL;   /*!<PLL2VCOSEL: PLL2 clock Output range
                          This parameter must be a value of @ref RCC_PLL2_VCO_Range                    */

  uint32_t PLL2FRACN;    /*!<PLL2FRACN: Specifies Fractional Part Of The Multiplication Factor for
                            PLL2 VCO It should be a value between 0 and 8191                           */
}RCC_PLL2InitTypeDef;

/**
  * @brief  PLL3 Clock structure definition
  */
typedef struct
{

  uint32_t PLL3M;       /*!< PLL3M: Division factor for PLL3 VCO input clock.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 63    */

  uint32_t PLL3N;       /*!< PLL3N: Multiplication factor for PLL3 VCO output clock.
                             This parameter must be a number between Min_Data = 4 and Max_Data = 512
                             or between Min_Data = 8 and Max_Data = 420(*)
                             (*) : For stm32h7a3xx and stm32h7b3xx family lines.                       */  

  uint32_t PLL3P;       /*!< PLL3P: Division factor for system clock.
                             This parameter must be a number between Min_Data = 2 and Max_Data = 128
                             odd division factors are not allowed                                      */

  uint32_t PLL3Q;        /*!< PLL3Q: Division factor for peripheral clocks.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 128   */

  uint32_t PLL3R;        /*!< PLL3R: Division factor for peripheral clocks.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 128   */
  uint32_t PLL3RGE;      /*!<PLL3RGE: PLL3 clock Input range
                          This parameter must be a value of @ref RCC_PLL3_VCI_Range                    */
  uint32_t PLL3VCOSEL;   /*!<PLL3VCOSEL: PLL3 clock Output range
                          This parameter must be a value of @ref RCC_PLL3_VCO_Range                    */

  uint32_t PLL3FRACN;    /*!<PLL3FRACN: Specifies Fractional Part Of The Multiplication Factor for
                            PLL3 VCO It should be a value between 0 and 8191                           */
}RCC_PLL3InitTypeDef;

/**
  * @brief  RCC PLL1 Clocks structure definition
  */
typedef struct
{
  uint32_t PLL1_P_Frequency;
  uint32_t PLL1_Q_Frequency;
  uint32_t PLL1_R_Frequency;
}PLL1_ClocksTypeDef;

/**
  * @brief  RCC PLL2 Clocks structure definition
  */
typedef struct
{
  uint32_t PLL2_P_Frequency;
  uint32_t PLL2_Q_Frequency;
  uint32_t PLL2_R_Frequency;
}PLL2_ClocksTypeDef;

/**
  * @brief  RCC PLL3 Clocks structure definition
  */
typedef struct
{
  uint32_t PLL3_P_Frequency;
  uint32_t PLL3_Q_Frequency;
  uint32_t PLL3_R_Frequency;
}PLL3_ClocksTypeDef;


/**
  * @brief  RCC extended clocks structure definition
  */
typedef struct
{
  uint32_t PeriphClockSelection;   /*!< The Extended Clock to be configured.
                                        This parameter can be a value of @ref RCCEx_Periph_Clock_Selection */

  RCC_PLL2InitTypeDef PLL2;        /*!< PLL2structure parameters.
                                        This parameter will be used only when PLL2 is selected as kernel clock Source for some peripherals */

  RCC_PLL3InitTypeDef PLL3;        /*!< PLL3 structure parameters.
                                        This parameter will be used only when PLL2 is selected as kernel clock Source for some peripherals */

  uint32_t FmcClockSelection;     /*!< Specifies FMC clock source
                                        This parameter can be a value of @ref RCCEx_FMC_Clock_Source     */

#if defined(QUADSPI)
  uint32_t QspiClockSelection;    /*!< Specifies QSPI clock source
                                        This parameter can be a value of @ref RCCEx_QSPI_Clock_Source    */
#endif /* QUADSPI */

#if defined(OCTOSPI1) || defined(OCTOSPI2)
  uint32_t OspiClockSelection;    /*!< Specifies OSPI clock source
                                        This parameter can be a value of @ref RCCEx_OSPI_Clock_Source    */
#endif /*(OCTOSPI1) || (OCTOSPI2)*/


#if defined(DSI)
  uint32_t DsiClockSelection;     /*!< Specifies DSI clock source
                                     This parameter can be a value of @ref RCCEx_DSI_Clock_Source        */
#endif /* DSI */

  uint32_t SdmmcClockSelection;    /*!< Specifies SDMMC clock source
                                        This parameter can be a value of @ref RCCEx_SDMMC_Clock_Source   */

  uint32_t CkperClockSelection;   /*!< Specifies CKPER clock source
                                        This parameter can be a value of @ref RCCEx_CLKP_Clock_Source   */

  uint32_t Sai1ClockSelection;     /*!< Specifies SAI1 clock source
                                        This parameter can be a value of @ref RCCEx_SAI1_Clock_Source    */

#if defined(SAI3)
  uint32_t Sai23ClockSelection;     /*!< Specifies SAI2/3 clock source
                                         This parameter can be a value of @ref RCCEx_SAI23_Clock_Source  */
#endif /* SAI3 */

#if defined(RCC_CDCCIP1R_SAI2ASEL)
  uint32_t Sai2AClockSelection;     /*!< Specifies SAI2A clock source
                                        This parameter can be a value of @ref RCCEx_SAI2A_Clock_Source  */
#endif /* RCC_CDCCIP1R_SAI2ASEL */

#if defined(RCC_CDCCIP1R_SAI2BSEL)
  uint32_t Sai2BClockSelection;     /*!< Specifies SAI2B clock source
                                         This parameter can be a value of @ref RCCEx_SAI2B_Clock_Source    */
#endif /* RCC_CDCCIP1R_SAI2BSEL */

  uint32_t Spi123ClockSelection;     /*!< Specifies SPI1/2/3 clock source
                                          This parameter can be a value of @ref RCCEx_SPI123_Clock_Source    */

  uint32_t Spi45ClockSelection;     /*!< Specifies SPI4/5 clock source
                                         This parameter can be a value of @ref RCCEx_SPI45_Clock_Source    */

  uint32_t SpdifrxClockSelection;   /*!< Specifies SPDIFRX Clock clock source
                                        This parameter can be a value of @ref RCCEx_SPDIFRX_Clock_Source */

  uint32_t Dfsdm1ClockSelection;    /*!< Specifies DFSDM1 Clock clock source
                                        This parameter can be a value of @ref RCCEx_DFSDM1_Clock_Source  */

#if defined(DFSDM2_BASE)
  uint32_t Dfsdm2ClockSelection;    /*!< Specifies DFSDM2 Clock clock source
                                        This parameter can be a value of @ref RCCEx_DFSDM2_Clock_Source  */
#endif /* DFSDM2_BASE */

#if defined(FDCAN1) || defined(FDCAN2)
  uint32_t FdcanClockSelection;   /*!< Specifies FDCAN Clock clock source
                                        This parameter can be a value of @ref RCCEx_FDCAN_Clock_Source   */
#endif /*FDCAN1 || FDCAN2*/

  uint32_t Swpmi1ClockSelection;   /*!< Specifies SWPMI1 Clock clock source
                                        This parameter can be a value of @ref RCCEx_SWPMI1_Clock_Source  */

  uint32_t Usart234578ClockSelection;   /*!< Specifies USART2/3/4/5/7/8 clock source
                                             This parameter can be a value of @ref RCCEx_USART234578_Clock_Source  */

  uint32_t Usart16ClockSelection;  /*!< Specifies USART1/6 clock source
                                        This parameter can be a value of @ref RCCEx_USART16_Clock_Source  */

   uint32_t RngClockSelection;      /*!< Specifies RNG clock source
                                        This parameter can be a value of @ref RCCEx_RNG_Clock_Source     */

   uint32_t I2c123ClockSelection;   /*!< Specifies I2C1/2/3 clock source
                                        This parameter can be a value of @ref RCCEx_I2C123_Clock_Source    */

  uint32_t UsbClockSelection;      /*!< Specifies USB clock source
                                        This parameter can be a value of @ref RCCEx_USB_Clock_Source     */

  uint32_t CecClockSelection;     /*!< Specifies CEC clock source
                                        This parameter can be a value of @ref RCCEx_CEC_Clock_Source     */

  uint32_t Lptim1ClockSelection;   /*!< Specifies LPTIM1 clock source
                                        This parameter can be a value of @ref RCCEx_LPTIM1_Clock_Source  */

  uint32_t Lpuart1ClockSelection;  /*!< Specifies LPUART1 clock source
                                        This parameter can be a value of @ref RCCEx_LPUART1_Clock_Source */

  uint32_t I2c4ClockSelection;     /*!< Specifies I2C4 clock source
                                        This parameter can be a value of @ref RCCEx_I2C4_Clock_Source    */

  uint32_t Lptim2ClockSelection;   /*!< Specifies LPTIM2 clock source
                                        This parameter can be a value of @ref RCCEx_LPTIM2_Clock_Source  */

  uint32_t Lptim345ClockSelection;   /*!< Specifies LPTIM3/4/5 clock source
                                          This parameter can be a value of @ref RCCEx_LPTIM345_Clock_Source  */

  uint32_t AdcClockSelection;      /*!< Specifies ADC interface clock source
                                        This parameter can be a value of @ref RCCEx_ADC_Clock_Source     */
#if defined(SAI4)
  uint32_t Sai4AClockSelection;     /*!< Specifies SAI4A clock source
                                        This parameter can be a value of @ref RCCEx_SAI4A_Clock_Source   */

  uint32_t Sai4BClockSelection;     /*!< Specifies SAI4B clock source
                                        This parameter can be a value of @ref RCCEx_SAI4B_Clock_Source   */
#endif /* SAI4 */

  uint32_t Spi6ClockSelection;     /*!< Specifies SPI6 clock source
                                        This parameter can be a value of @ref RCCEx_SPI6_Clock_Source    */

  uint32_t RTCClockSelection;      /*!< Specifies RTC Clock clock source
                                        This parameter can be a value of @ref RCC_RTC_Clock_Source       */

#if defined(HRTIM1)
  uint32_t Hrtim1ClockSelection;      /*!< Specifies HRTIM1 Clock clock source
                                        This parameter can be a value of @ref RCCEx_HRTIM1_Clock_Source   */
#endif /* HRTIM1 */

  uint32_t TIMPresSelection;       /*!< Specifies TIM Clock Prescalers Selection.
                                       This parameter can be a value of @ref RCCEx_TIM_Prescaler_Selection */
}RCC_PeriphCLKInitTypeDef;


/**
  * @brief RCC_CRS Init structure definition
  */
typedef struct
{
  uint32_t Prescaler;             /*!< Specifies the division factor of the SYNC signal.
                                     This parameter can be a value of @ref RCCEx_CRS_SynchroDivider  */

  uint32_t Source;                /*!< Specifies the SYNC signal source.
                                     This parameter can be a value of @ref RCCEx_CRS_SynchroSource   */

  uint32_t Polarity;              /*!< Specifies the input polarity for the SYNC signal source.
                                     This parameter can be a value of @ref RCCEx_CRS_SynchroPolarity */

  uint32_t ReloadValue;           /*!< Specifies the value to be loaded in the frequency error counter with each SYNC event.
                                      It can be calculated in using macro __HAL_RCC_CRS_RELOADVALUE_CALCULATE(__FTARGET__, __FSYNC__)
                                     This parameter must be a number between 0 and 0xFFFF or a value of @ref RCCEx_CRS_ReloadValueDefault .*/

  uint32_t ErrorLimitValue;       /*!< Specifies the value to be used to evaluate the captured frequency error value.
                                     This parameter must be a number between 0 and 0xFF or a value of @ref RCCEx_CRS_ErrorLimitDefault */

  uint32_t HSI48CalibrationValue; /*!< Specifies a user-programmable trimming value to the HSI48 oscillator.
                                     This parameter must be a number between 0 and 0x3F or a value of @ref RCCEx_CRS_HSI48CalibrationDefault */

}RCC_CRSInitTypeDef;

/**
  * @brief RCC_CRS Synchronization structure definition
  */
typedef struct
{
  uint32_t ReloadValue;           /*!< Specifies the value loaded in the Counter reload value.
                                     This parameter must be a number between 0 and 0xFFFF */

  uint32_t HSI48CalibrationValue; /*!< Specifies value loaded in HSI48 oscillator smooth trimming.
                                     This parameter must be a number between 0 and 0x3F */

  uint32_t FreqErrorCapture;      /*!< Specifies the value loaded in the .FECAP, the frequency error counter
                                                                    value latched in the time of the last SYNC event.
                                    This parameter must be a number between 0 and 0xFFFF */

  uint32_t FreqErrorDirection;    /*!< Specifies the value loaded in the .FEDIR, the counting direction of the
                                                                    frequency error counter latched in the time of the last SYNC event.
                                                                    It shows whether the actual frequency is below or above the target.
                                    This parameter must be a value of @ref RCCEx_CRS_FreqErrorDirection*/

}RCC_CRSSynchroInfoTypeDef;

/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Constants  RCCEx Exported Constants
  * @{
  */

/** @defgroup RCCEx_Periph_Clock_Selection  RCCEx Periph Clock Selection
  * @{
  */

#if defined(UART9) && defined(USART10)
#define RCC_PERIPHCLK_USART16910       (0x00000001U)
#define RCC_PERIPHCLK_USART1           RCC_PERIPHCLK_USART16910
#define RCC_PERIPHCLK_USART6           RCC_PERIPHCLK_USART16910
#define RCC_PERIPHCLK_UART9            RCC_PERIPHCLK_USART16910
#define RCC_PERIPHCLK_USART10          RCC_PERIPHCLK_USART16910
/*alias*/
#define RCC_PERIPHCLK_USART16          RCC_PERIPHCLK_USART16910
#else
#define RCC_PERIPHCLK_USART16          (0x00000001U)
#define RCC_PERIPHCLK_USART1           RCC_PERIPHCLK_USART16
#define RCC_PERIPHCLK_USART6           RCC_PERIPHCLK_USART16
/* alias */
#define RCC_PERIPHCLK_USART16910       RCC_PERIPHCLK_USART16
#endif /* UART9 && USART10*/
#define RCC_PERIPHCLK_USART234578      (0x00000002U)
#define RCC_PERIPHCLK_USART2           RCC_PERIPHCLK_USART234578
#define RCC_PERIPHCLK_USART3           RCC_PERIPHCLK_USART234578
#define RCC_PERIPHCLK_UART4            RCC_PERIPHCLK_USART234578
#define RCC_PERIPHCLK_UART5            RCC_PERIPHCLK_USART234578
#define RCC_PERIPHCLK_UART7            RCC_PERIPHCLK_USART234578
#define RCC_PERIPHCLK_UART8            RCC_PERIPHCLK_USART234578
#define RCC_PERIPHCLK_LPUART1          (0x00000004U)
#define RCC_PERIPHCLK_I2C123           (0x00000008U)
#define RCC_PERIPHCLK_I2C1             RCC_PERIPHCLK_I2C123
#define RCC_PERIPHCLK_I2C2             RCC_PERIPHCLK_I2C123
#define RCC_PERIPHCLK_I2C3             RCC_PERIPHCLK_I2C123
#define RCC_PERIPHCLK_I2C4             (0x00000010U)
#define RCC_PERIPHCLK_LPTIM1           (0x00000020U)
#define RCC_PERIPHCLK_LPTIM2           (0x00000040U)
#define RCC_PERIPHCLK_LPTIM345         (0x00000080U)
#define RCC_PERIPHCLK_LPTIM3           RCC_PERIPHCLK_LPTIM345
#if defined(LPTIM4)
#define RCC_PERIPHCLK_LPTIM4           RCC_PERIPHCLK_LPTIM345
#endif /*LPTIM4*/
#if defined(LPTIM5)
#define RCC_PERIPHCLK_LPTIM5           RCC_PERIPHCLK_LPTIM345
#endif /*LPTIM5*/
#define RCC_PERIPHCLK_SAI1             (0x00000100U)
#if defined(SAI3)
#define RCC_PERIPHCLK_SAI23            (0x00000200U)
#define RCC_PERIPHCLK_SAI2             RCC_PERIPHCLK_SAI23
#define RCC_PERIPHCLK_SAI3             RCC_PERIPHCLK_SAI23
#endif /* SAI3 */
#if defined(RCC_CDCCIP1R_SAI2ASEL_0)
#define RCC_PERIPHCLK_SAI2A            (0x00000200U)
#endif /* RCC_CDCCIP1R_SAI2ASEL_0 */
#if defined(RCC_CDCCIP1R_SAI2BSEL_0)
#define RCC_PERIPHCLK_SAI2B            (0x00000400U)
#endif /* RCC_CDCCIP1R_SAI2BSEL_0 */
#if defined(SAI4)
#define RCC_PERIPHCLK_SAI4A            (0x00000400U)
#define RCC_PERIPHCLK_SAI4B            (0x00000800U)
#endif /* SAI4 */
#define RCC_PERIPHCLK_SPI123           (0x00001000U)
#define RCC_PERIPHCLK_SPI1             RCC_PERIPHCLK_SPI123
#define RCC_PERIPHCLK_SPI2             RCC_PERIPHCLK_SPI123
#define RCC_PERIPHCLK_SPI3             RCC_PERIPHCLK_SPI123
#define RCC_PERIPHCLK_SPI45            (0x00002000U)
#define RCC_PERIPHCLK_SPI4             RCC_PERIPHCLK_SPI45
#define RCC_PERIPHCLK_SPI5             RCC_PERIPHCLK_SPI45
#define RCC_PERIPHCLK_SPI6             (0x00004000U)
#define RCC_PERIPHCLK_FDCAN            (0x00008000U)
#define RCC_PERIPHCLK_SDMMC            (0x00010000U)
#define RCC_PERIPHCLK_RNG              (0x00020000U)
#define RCC_PERIPHCLK_USB              (0x00040000U)
#define RCC_PERIPHCLK_ADC              (0x00080000U)
#define RCC_PERIPHCLK_SWPMI1           (0x00100000U)
#define RCC_PERIPHCLK_DFSDM1           (0x00200000U)
#if defined(DFSDM2_BASE)
#define RCC_PERIPHCLK_DFSDM2           (0x00000800U)
#endif /* DFSDM2 */
#define RCC_PERIPHCLK_RTC              (0x00400000U)
#define RCC_PERIPHCLK_CEC              (0x00800000U)
#define RCC_PERIPHCLK_FMC              (0x01000000U)
#if defined(QUADSPI)
#define RCC_PERIPHCLK_QSPI             (0x02000000U)
#endif /* QUADSPI */
#if defined(OCTOSPI1) || defined(OCTOSPI2)
#define RCC_PERIPHCLK_OSPI             (0x02000000U)
#endif /* defined(OCTOSPI1) || defined(OCTOSPI2) */
#define RCC_PERIPHCLK_DSI              (0x04000000U)
#define RCC_PERIPHCLK_SPDIFRX          (0x08000000U)
#if defined(HRTIM1)
#define RCC_PERIPHCLK_HRTIM1           (0x10000000U)
#endif /* HRTIM1 */
#if defined(LTDC)
#define RCC_PERIPHCLK_LTDC             (0x20000000U)
#endif /* LTDC */
#define RCC_PERIPHCLK_TIM              (0x40000000U)
#define RCC_PERIPHCLK_CKPER            (0x80000000U)

/**
  * @}
  */


/** @defgroup RCC_PLL2_Clock_Output  RCC PLL2 Clock Output
  * @{
  */
#define RCC_PLL2_DIVP                RCC_PLLCFGR_DIVP2EN
#define RCC_PLL2_DIVQ                RCC_PLLCFGR_DIVQ2EN
#define RCC_PLL2_DIVR                RCC_PLLCFGR_DIVR2EN

/**
  * @}
  */

/** @defgroup RCC_PLL3_Clock_Output  RCC PLL3 Clock Output
  * @{
  */
#define RCC_PLL3_DIVP                RCC_PLLCFGR_DIVP3EN
#define RCC_PLL3_DIVQ                RCC_PLLCFGR_DIVQ3EN
#define RCC_PLL3_DIVR                RCC_PLLCFGR_DIVR3EN

/**
  * @}
  */

/** @defgroup RCC_PLL2_VCI_Range  RCC PLL2 VCI Range
  * @{
  */
#define RCC_PLL2VCIRANGE_0                RCC_PLLCFGR_PLL2RGE_0        /*!< Clock range frequency between 1 and 2 MHz  */
#define RCC_PLL2VCIRANGE_1                RCC_PLLCFGR_PLL2RGE_1        /*!< Clock range frequency between 2 and 4 MHz  */
#define RCC_PLL2VCIRANGE_2                RCC_PLLCFGR_PLL2RGE_2        /*!< Clock range frequency between 4 and 8 MHz  */
#define RCC_PLL2VCIRANGE_3                RCC_PLLCFGR_PLL2RGE_3        /*!< Clock range frequency between 8 and 16 MHz */

/**
  * @}
  */


/** @defgroup RCC_PLL2_VCO_Range  RCC PLL2 VCO Range
  * @{
  */
#define RCC_PLL2VCOWIDE                 (0x00000000U)
#define RCC_PLL2VCOMEDIUM               RCC_PLLCFGR_PLL2VCOSEL

/**
  * @}
  */

/** @defgroup RCC_PLL3_VCI_Range  RCC PLL3 VCI Range
  * @{
  */
#define RCC_PLL3VCIRANGE_0                RCC_PLLCFGR_PLL3RGE_0         /*!< Clock range frequency between 1 and 2 MHz  */
#define RCC_PLL3VCIRANGE_1                RCC_PLLCFGR_PLL3RGE_1         /*!< Clock range frequency between 2 and 4 MHz  */
#define RCC_PLL3VCIRANGE_2                RCC_PLLCFGR_PLL3RGE_2         /*!< Clock range frequency between 4 and 8 MHz  */
#define RCC_PLL3VCIRANGE_3                RCC_PLLCFGR_PLL3RGE_3         /*!< Clock range frequency between 8 and 16 MHz */

/**
  * @}
  */


/** @defgroup RCC_PLL3_VCO_Range  RCC PLL3 VCO Range
  * @{
  */
#define RCC_PLL3VCOWIDE                 (0x00000000U)
#define RCC_PLL3VCOMEDIUM               RCC_PLLCFGR_PLL3VCOSEL

/**
  * @}
  */

/** @defgroup RCCEx_USART16_Clock_Source  RCCEx USART1/6 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP2R_USART16SEL)
#define RCC_USART16CLKSOURCE_D2PCLK2    (0x00000000U)
/* alias */
#define RCC_USART16CLKSOURCE_PCLK2        RCC_USART16CLKSOURCE_D2PCLK2
#define RCC_USART16CLKSOURCE_PLL2         RCC_D2CCIP2R_USART16SEL_0
#define RCC_USART16CLKSOURCE_PLL3         RCC_D2CCIP2R_USART16SEL_1
#define RCC_USART16CLKSOURCE_HSI         (RCC_D2CCIP2R_USART16SEL_0 | RCC_D2CCIP2R_USART16SEL_1)
#define RCC_USART16CLKSOURCE_CSI          RCC_D2CCIP2R_USART16SEL_2
#define RCC_USART16CLKSOURCE_LSE         (RCC_D2CCIP2R_USART16SEL_0 | RCC_D2CCIP2R_USART16SEL_2)

#else
#define RCC_USART16910CLKSOURCE_CDPCLK2   (0x00000000U)
/* alias */
#define RCC_USART16910CLKSOURCE_D2PCLK2   RCC_USART16910CLKSOURCE_CDPCLK2
#define RCC_USART16910CLKSOURCE_PLL2      RCC_CDCCIP2R_USART16910SEL_0
#define RCC_USART16910CLKSOURCE_PLL3      RCC_CDCCIP2R_USART16910SEL_1
#define RCC_USART16910CLKSOURCE_HSI      (RCC_CDCCIP2R_USART16910SEL_0 | RCC_CDCCIP2R_USART16910SEL_1)
#define RCC_USART16910CLKSOURCE_CSI       RCC_CDCCIP2R_USART16910SEL_2
#define RCC_USART16910CLKSOURCE_LSE      (RCC_CDCCIP2R_USART16910SEL_0 | RCC_CDCCIP2R_USART16910SEL_2)

/*  Aliases */
#define RCC_USART16CLKSOURCE_CDPCLK2     RCC_USART16910CLKSOURCE_CDPCLK2
#define RCC_USART16CLKSOURCE_PCLK2       RCC_USART16CLKSOURCE_CDPCLK2
#define RCC_USART16CLKSOURCE_D2PCLK2     RCC_USART16CLKSOURCE_CDPCLK2
#define RCC_USART16CLKSOURCE_PLL2        RCC_USART16910CLKSOURCE_PLL2
#define RCC_USART16CLKSOURCE_PLL3        RCC_USART16910CLKSOURCE_PLL3
#define RCC_USART16CLKSOURCE_HSI         RCC_USART16910CLKSOURCE_HSI
#define RCC_USART16CLKSOURCE_CSI         RCC_USART16910CLKSOURCE_CSI
#define RCC_USART16CLKSOURCE_LSE         RCC_USART16910CLKSOURCE_LSE
#endif /* RCC_D2CCIP2R_USART16SEL */
/**
  * @}
  */

/** @defgroup RCCEx_USART1_Clock_Source  RCCEx USART1 Clock Source
  * @{
  */
#define RCC_USART1CLKSOURCE_D2PCLK2   RCC_USART16CLKSOURCE_D2PCLK2
#define RCC_USART1CLKSOURCE_PLL2      RCC_USART16CLKSOURCE_PLL2
#define RCC_USART1CLKSOURCE_PLL3      RCC_USART16CLKSOURCE_PLL3
#define RCC_USART1CLKSOURCE_HSI       RCC_USART16CLKSOURCE_HSI
#define RCC_USART1CLKSOURCE_CSI       RCC_USART16CLKSOURCE_CSI
#define RCC_USART1CLKSOURCE_LSE       RCC_USART16CLKSOURCE_LSE
/**
  * @}
  */

/** @defgroup RCCEx_USART6_Clock_Source  RCCEx USART6 Clock Source
  * @{
  */
#define RCC_USART6CLKSOURCE_D2PCLK2   RCC_USART16CLKSOURCE_D2PCLK2
#define RCC_USART6CLKSOURCE_PLL2      RCC_USART16CLKSOURCE_PLL2
#define RCC_USART6CLKSOURCE_PLL3      RCC_USART16CLKSOURCE_PLL3
#define RCC_USART6CLKSOURCE_HSI       RCC_USART16CLKSOURCE_HSI
#define RCC_USART6CLKSOURCE_CSI       RCC_USART16CLKSOURCE_CSI
#define RCC_USART6CLKSOURCE_LSE       RCC_USART16CLKSOURCE_LSE

/**
  * @}
  */

#if defined(UART9)
/** @defgroup RCCEx_UART9_Clock_Source  RCCEx UART9 Clock Source
  * @{
  */
#define RCC_UART9CLKSOURCE_D2PCLK2   RCC_USART16CLKSOURCE_D2PCLK2
#define RCC_UART9CLKSOURCE_PLL2      RCC_USART16CLKSOURCE_PLL2
#define RCC_UART9CLKSOURCE_PLL3      RCC_USART16CLKSOURCE_PLL3
#define RCC_UART9CLKSOURCE_HSI       RCC_USART16CLKSOURCE_HSI
#define RCC_UART9CLKSOURCE_CSI       RCC_USART16CLKSOURCE_CSI
#define RCC_UART9CLKSOURCE_LSE       RCC_USART16CLKSOURCE_LSE
/**
  * @}
  */
#endif /* UART9 */

#if defined(USART10)
/** @defgroup RCCEx_USART10_Clock_Source  RCCEx USART10 Clock Source
  * @{
  */
#define RCC_USART10CLKSOURCE_D2PCLK2   RCC_USART16CLKSOURCE_D2PCLK2
#define RCC_USART10CLKSOURCE_PLL2      RCC_USART16CLKSOURCE_PLL2
#define RCC_USART10CLKSOURCE_PLL3      RCC_USART16CLKSOURCE_PLL3
#define RCC_USART10CLKSOURCE_HSI       RCC_USART16CLKSOURCE_HSI
#define RCC_USART10CLKSOURCE_CSI       RCC_USART16CLKSOURCE_CSI
#define RCC_USART10CLKSOURCE_LSE       RCC_USART16CLKSOURCE_LSE
/**
  * @}
  */
#endif /* USART10 */

/** @defgroup RCCEx_USART234578_Clock_Source  RCCEx USART2/3/4/5/7/8 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP2R_USART28SEL)
#define RCC_USART234578CLKSOURCE_D2PCLK1    (0x00000000U)
/* alias */
#define RCC_USART234578CLKSOURCE_PCLK1      RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_USART234578CLKSOURCE_PLL2       RCC_D2CCIP2R_USART28SEL_0
#define RCC_USART234578CLKSOURCE_PLL3       RCC_D2CCIP2R_USART28SEL_1
#define RCC_USART234578CLKSOURCE_HSI        (RCC_D2CCIP2R_USART28SEL_0 | RCC_D2CCIP2R_USART28SEL_1)
#define RCC_USART234578CLKSOURCE_CSI        RCC_D2CCIP2R_USART28SEL_2
#define RCC_USART234578CLKSOURCE_LSE        (RCC_D2CCIP2R_USART28SEL_0 | RCC_D2CCIP2R_USART28SEL_2)
#else
#define RCC_USART234578CLKSOURCE_CDPCLK1   (0x00000000U)
/* alias */
#define RCC_USART234578CLKSOURCE_PCLK1     RCC_USART234578CLKSOURCE_CDPCLK1
#define RCC_USART234578CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_CDPCLK1
#define RCC_USART234578CLKSOURCE_PLL2      RCC_CDCCIP2R_USART234578SEL_0
#define RCC_USART234578CLKSOURCE_PLL3      RCC_CDCCIP2R_USART234578SEL_1
#define RCC_USART234578CLKSOURCE_HSI      (RCC_CDCCIP2R_USART234578SEL_0 | RCC_CDCCIP2R_USART234578SEL_1)
#define RCC_USART234578CLKSOURCE_CSI       RCC_CDCCIP2R_USART234578SEL_2
#define RCC_USART234578CLKSOURCE_LSE      (RCC_CDCCIP2R_USART234578SEL_0 | RCC_CDCCIP2R_USART234578SEL_2)
#endif /* RCC_D2CCIP2R_USART28SEL */
/**
  * @}
  */

/** @defgroup RCCEx_USART2_Clock_Source  RCCEx USART2 Clock Source
  * @{
  */
#define RCC_USART2CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_USART2CLKSOURCE_PLL2      RCC_USART234578CLKSOURCE_PLL2
#define RCC_USART2CLKSOURCE_PLL3      RCC_USART234578CLKSOURCE_PLL3
#define RCC_USART2CLKSOURCE_HSI       RCC_USART234578CLKSOURCE_HSI
#define RCC_USART2CLKSOURCE_CSI       RCC_USART234578CLKSOURCE_CSI
#define RCC_USART2CLKSOURCE_LSE       RCC_USART234578CLKSOURCE_LSE

/**
  * @}
  */

/** @defgroup RCCEx_USART3_Clock_Source  RCCEx USART3 Clock Source
  * @{
  */
#define RCC_USART3CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_USART3CLKSOURCE_PLL2      RCC_USART234578CLKSOURCE_PLL2
#define RCC_USART3CLKSOURCE_PLL3      RCC_USART234578CLKSOURCE_PLL3
#define RCC_USART3CLKSOURCE_HSI       RCC_USART234578CLKSOURCE_HSI
#define RCC_USART3CLKSOURCE_CSI       RCC_USART234578CLKSOURCE_CSI
#define RCC_USART3CLKSOURCE_LSE       RCC_USART234578CLKSOURCE_LSE

/**
  * @}
  */

/** @defgroup RCCEx_UART4_Clock_Source  RCCEx UART4 Clock Source
  * @{
  */
#define RCC_UART4CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_UART4CLKSOURCE_PLL2      RCC_USART234578CLKSOURCE_PLL2
#define RCC_UART4CLKSOURCE_PLL3      RCC_USART234578CLKSOURCE_PLL3
#define RCC_UART4CLKSOURCE_HSI       RCC_USART234578CLKSOURCE_HSI
#define RCC_UART4CLKSOURCE_CSI       RCC_USART234578CLKSOURCE_CSI
#define RCC_UART4CLKSOURCE_LSE       RCC_USART234578CLKSOURCE_LSE

/**
  * @}
  */

/** @defgroup RCCEx_UART5_Clock_Source  RCCEx UART5 Clock Source
  * @{
  */
#define RCC_UART5CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_UART5CLKSOURCE_PLL2      RCC_USART234578CLKSOURCE_PLL2
#define RCC_UART5CLKSOURCE_PLL3      RCC_USART234578CLKSOURCE_PLL3
#define RCC_UART5CLKSOURCE_HSI       RCC_USART234578CLKSOURCE_HSI
#define RCC_UART5CLKSOURCE_CSI       RCC_USART234578CLKSOURCE_CSI
#define RCC_UART5CLKSOURCE_LSE       RCC_USART234578CLKSOURCE_LSE

/**
  * @}
  */

/** @defgroup RCCEx_UART7_Clock_Source  RCCEx UART7 Clock Source
  * @{
  */
#define RCC_UART7CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_UART7CLKSOURCE_PLL2      RCC_USART234578CLKSOURCE_PLL2
#define RCC_UART7CLKSOURCE_PLL3      RCC_USART234578CLKSOURCE_PLL3
#define RCC_UART7CLKSOURCE_HSI       RCC_USART234578CLKSOURCE_HSI
#define RCC_UART7CLKSOURCE_CSI       RCC_USART234578CLKSOURCE_CSI
#define RCC_UART7CLKSOURCE_LSE       RCC_USART234578CLKSOURCE_LSE

/**
  * @}
  */

/** @defgroup RCCEx_UART8_Clock_Source  RCCEx UART8 Clock Source
  * @{
  */
#define RCC_UART8CLKSOURCE_D2PCLK1   RCC_USART234578CLKSOURCE_D2PCLK1
#define RCC_UART8CLKSOURCE_PLL2      RCC_USART234578CLKSOURCE_PLL2
#define RCC_UART8CLKSOURCE_PLL3      RCC_USART234578CLKSOURCE_PLL3
#define RCC_UART8CLKSOURCE_HSI       RCC_USART234578CLKSOURCE_HSI
#define RCC_UART8CLKSOURCE_CSI       RCC_USART234578CLKSOURCE_CSI
#define RCC_UART8CLKSOURCE_LSE       RCC_USART234578CLKSOURCE_LSE

/**
  * @}
  */

/** @defgroup RCCEx_LPUART1_Clock_Source  RCCEx LPUART1 Clock Source
  * @{
  */
#if defined(RCC_D3CCIPR_LPUART1SEL)
#define RCC_LPUART1CLKSOURCE_D3PCLK1    (0x00000000U)
/* alias */
#define RCC_LPUART1CLKSOURCE_PCLK4     RCC_LPUART1CLKSOURCE_D3PCLK1
#define RCC_LPUART1CLKSOURCE_PLL2      RCC_D3CCIPR_LPUART1SEL_0
#define RCC_LPUART1CLKSOURCE_PLL3      RCC_D3CCIPR_LPUART1SEL_1
#define RCC_LPUART1CLKSOURCE_HSI       (RCC_D3CCIPR_LPUART1SEL_0 | RCC_D3CCIPR_LPUART1SEL_1)
#define RCC_LPUART1CLKSOURCE_CSI        RCC_D3CCIPR_LPUART1SEL_2
#define RCC_LPUART1CLKSOURCE_LSE       (RCC_D3CCIPR_LPUART1SEL_2 | RCC_D3CCIPR_LPUART1SEL_0)
#else
#define RCC_LPUART1CLKSOURCE_SRDPCLK4   (0x00000000U)
/* alias*/
#define RCC_LPUART1CLKSOURCE_PCLK4     RCC_LPUART1CLKSOURCE_SRDPCLK4
#define RCC_LPUART1CLKSOURCE_D3PCLK1   RCC_LPUART1CLKSOURCE_SRDPCLK4
#define RCC_LPUART1CLKSOURCE_PLL2      RCC_SRDCCIPR_LPUART1SEL_0
#define RCC_LPUART1CLKSOURCE_PLL3      RCC_SRDCCIPR_LPUART1SEL_1
#define RCC_LPUART1CLKSOURCE_HSI       (RCC_SRDCCIPR_LPUART1SEL_0 | RCC_SRDCCIPR_LPUART1SEL_1)
#define RCC_LPUART1CLKSOURCE_CSI        RCC_SRDCCIPR_LPUART1SEL_2
#define RCC_LPUART1CLKSOURCE_LSE       (RCC_SRDCCIPR_LPUART1SEL_2 | RCC_SRDCCIPR_LPUART1SEL_0)
#endif /* RCC_D3CCIPR_LPUART1SEL */
/**
  * @}
  */

/** @defgroup RCCEx_I2C123_Clock_Source  RCCEx I2C1/2/3 Clock Source
  * @{
  */
#if defined (RCC_D2CCIP2R_I2C123SEL)
#define RCC_I2C123CLKSOURCE_D2PCLK1      (0x00000000U)
#define RCC_I2C123CLKSOURCE_PLL3         RCC_D2CCIP2R_I2C123SEL_0
#define RCC_I2C123CLKSOURCE_HSI          RCC_D2CCIP2R_I2C123SEL_1
#define RCC_I2C123CLKSOURCE_CSI         (RCC_D2CCIP2R_I2C123SEL_0 | RCC_D2CCIP2R_I2C123SEL_1)
#else
#define RCC_I2C123CLKSOURCE_CDPCLK1      (0x00000000U)
/* alias */
#define RCC_I2C123CLKSOURCE_D2PCLK1      RCC_I2C123CLKSOURCE_CDPCLK1
#define RCC_I2C123CLKSOURCE_PLL3         RCC_CDCCIP2R_I2C123SEL_0
#define RCC_I2C123CLKSOURCE_HSI          RCC_CDCCIP2R_I2C123SEL_1
#define RCC_I2C123CLKSOURCE_CSI         (RCC_CDCCIP2R_I2C123SEL_0 | RCC_CDCCIP2R_I2C123SEL_1)
#endif /* RCC_D2CCIP2R_I2C123SEL */
/**
  * @}
  */

/** @defgroup RCCEx_I2C1_Clock_Source  RCCEx I2C1 Clock Source
  * @{
  */
#define RCC_I2C1CLKSOURCE_D2PCLK1     RCC_I2C123CLKSOURCE_D2PCLK1
#define RCC_I2C1CLKSOURCE_PLL3        RCC_I2C123CLKSOURCE_PLL3
#define RCC_I2C1CLKSOURCE_HSI         RCC_I2C123CLKSOURCE_HSI
#define RCC_I2C1CLKSOURCE_CSI         RCC_I2C123CLKSOURCE_CSI

/**
  * @}
  */

/** @defgroup RCCEx_I2C2_Clock_Source  RCCEx I2C2 Clock Source
  * @{
  */
#define RCC_I2C2CLKSOURCE_D2PCLK1     RCC_I2C123CLKSOURCE_D2PCLK1
#define RCC_I2C2CLKSOURCE_PLL3        RCC_I2C123CLKSOURCE_PLL3
#define RCC_I2C2CLKSOURCE_HSI         RCC_I2C123CLKSOURCE_HSI
#define RCC_I2C2CLKSOURCE_CSI         RCC_I2C123CLKSOURCE_CSI

/**
  * @}
  */

/** @defgroup RCCEx_I2C3_Clock_Source  RCCEx I2C3 Clock Source
  * @{
  */
#define RCC_I2C3CLKSOURCE_D2PCLK1     RCC_I2C123CLKSOURCE_D2PCLK1
#define RCC_I2C3CLKSOURCE_PLL3        RCC_I2C123CLKSOURCE_PLL3
#define RCC_I2C3CLKSOURCE_HSI         RCC_I2C123CLKSOURCE_HSI
#define RCC_I2C3CLKSOURCE_CSI         RCC_I2C123CLKSOURCE_CSI

/**
  * @}
  */

/** @defgroup RCCEx_I2C4_Clock_Source  RCCEx I2C4 Clock Source
  * @{
  */
#if defined(RCC_D3CCIPR_I2C4SEL)
#define RCC_I2C4CLKSOURCE_D3PCLK1      (0x00000000U)
#define RCC_I2C4CLKSOURCE_PLL3         RCC_D3CCIPR_I2C4SEL_0
#define RCC_I2C4CLKSOURCE_HSI          RCC_D3CCIPR_I2C4SEL_1
#define RCC_I2C4CLKSOURCE_CSI         (RCC_D3CCIPR_I2C4SEL_0 | RCC_D3CCIPR_I2C4SEL_1)
#else
#define RCC_I2C4CLKSOURCE_SRDPCLK4     (0x00000000U)
/* alias */
#define RCC_I2C4CLKSOURCE_D3PCLK1     RCC_I2C4CLKSOURCE_SRDPCLK4
#define RCC_I2C4CLKSOURCE_PLL3         RCC_SRDCCIPR_I2C4SEL_0
#define RCC_I2C4CLKSOURCE_HSI          RCC_SRDCCIPR_I2C4SEL_1
#define RCC_I2C4CLKSOURCE_CSI         (RCC_SRDCCIPR_I2C4SEL_0 | RCC_SRDCCIPR_I2C4SEL_1)
#endif /* RCC_D3CCIPR_I2C4SEL */

/**
  * @}
  */

/** @defgroup RCCEx_RNG_Clock_Source  RCCEx RNG Clock Source
  * @{
  */
#if defined(RCC_D2CCIP2R_RNGSEL)
#define RCC_RNGCLKSOURCE_HSI48        (0x00000000U)
#define RCC_RNGCLKSOURCE_PLL           RCC_D2CCIP2R_RNGSEL_0
#define RCC_RNGCLKSOURCE_LSE           RCC_D2CCIP2R_RNGSEL_1
#define RCC_RNGCLKSOURCE_LSI           RCC_D2CCIP2R_RNGSEL
#else
#define RCC_RNGCLKSOURCE_HSI48        (0x00000000U)
#define RCC_RNGCLKSOURCE_PLL           RCC_CDCCIP2R_RNGSEL_0
#define RCC_RNGCLKSOURCE_LSE           RCC_CDCCIP2R_RNGSEL_1
#define RCC_RNGCLKSOURCE_LSI           RCC_CDCCIP2R_RNGSEL
#endif /* RCC_D2CCIP2R_RNGSEL */

/**
  * @}
  */
#if defined(HRTIM1)

/** @defgroup RCCEx_HRTIM1_Clock_Source RCC Extended HRTIM1 Clock Source
  * @{
  */
#define RCC_HRTIM1CLK_TIMCLK                (0x00000000U)
#define RCC_HRTIM1CLK_CPUCLK                RCC_CFGR_HRTIMSEL

/**
  * @}
  */
#endif /*HRTIM1*/

/** @defgroup RCCEx_USB_Clock_Source  RCCEx USB Clock Source
  * @{
  */
#if defined(RCC_D2CCIP2R_USBSEL)
#define RCC_USBCLKSOURCE_PLL                  RCC_D2CCIP2R_USBSEL_0
#define RCC_USBCLKSOURCE_PLL3                 RCC_D2CCIP2R_USBSEL_1
#define RCC_USBCLKSOURCE_HSI48                RCC_D2CCIP2R_USBSEL
#else
#define RCC_USBCLKSOURCE_PLL                  RCC_CDCCIP2R_USBSEL_0
#define RCC_USBCLKSOURCE_PLL3                 RCC_CDCCIP2R_USBSEL_1
#define RCC_USBCLKSOURCE_HSI48                RCC_CDCCIP2R_USBSEL
#endif /* RCC_D2CCIP2R_USBSEL */

/**
  * @}
  */

/** @defgroup RCCEx_SAI1_Clock_Source SAI1 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_SAI1SEL)
#define RCC_SAI1CLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI1CLKSOURCE_PLL2         RCC_D2CCIP1R_SAI1SEL_0
#define RCC_SAI1CLKSOURCE_PLL3         RCC_D2CCIP1R_SAI1SEL_1
#define RCC_SAI1CLKSOURCE_PIN         (RCC_D2CCIP1R_SAI1SEL_0 | RCC_D2CCIP1R_SAI1SEL_1)
#define RCC_SAI1CLKSOURCE_CLKP         RCC_D2CCIP1R_SAI1SEL_2
#else
#define RCC_SAI1CLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI1CLKSOURCE_PLL2         RCC_CDCCIP1R_SAI1SEL_0
#define RCC_SAI1CLKSOURCE_PLL3         RCC_CDCCIP1R_SAI1SEL_1
#define RCC_SAI1CLKSOURCE_PIN         (RCC_CDCCIP1R_SAI1SEL_0 | RCC_CDCCIP1R_SAI1SEL_1)
#define RCC_SAI1CLKSOURCE_CLKP         RCC_CDCCIP1R_SAI1SEL_2
#endif /* RCC_D2CCIP1R_SAI1SEL */
/**
  * @}
  */

#if defined(SAI3)
/** @defgroup RCCEx_SAI23_Clock_Source SAI2/3 Clock Source
  * @{
  */
#define RCC_SAI23CLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI23CLKSOURCE_PLL2         RCC_D2CCIP1R_SAI23SEL_0
#define RCC_SAI23CLKSOURCE_PLL3         RCC_D2CCIP1R_SAI23SEL_1
#define RCC_SAI23CLKSOURCE_PIN         (RCC_D2CCIP1R_SAI23SEL_0 | RCC_D2CCIP1R_SAI23SEL_1)
#define RCC_SAI23CLKSOURCE_CLKP         RCC_D2CCIP1R_SAI23SEL_2
/**
  * @}
  */

/** @defgroup RCCEx_SAI2_Clock_Source SAI2 Clock Source
  * @{
  */
#define RCC_SAI2CLKSOURCE_PLL         RCC_SAI23CLKSOURCE_PLL
#define RCC_SAI2CLKSOURCE_PLL2        RCC_SAI23CLKSOURCE_PLL2
#define RCC_SAI2CLKSOURCE_PLL3        RCC_SAI23CLKSOURCE_PLL3
#define RCC_SAI2CLKSOURCE_PIN         RCC_SAI23CLKSOURCE_PIN
#define RCC_SAI2CLKSOURCE_CLKP        RCC_SAI23CLKSOURCE_CLKP

/**
  * @}
  */

/** @defgroup RCCEx_SAI3_Clock_Source SAI3 Clock Source
  * @{
  */
#define RCC_SAI3CLKSOURCE_PLL         RCC_SAI23CLKSOURCE_PLL
#define RCC_SAI3CLKSOURCE_PLL2        RCC_SAI23CLKSOURCE_PLL2
#define RCC_SAI3CLKSOURCE_PLL3        RCC_SAI23CLKSOURCE_PLL3
#define RCC_SAI3CLKSOURCE_PIN         RCC_SAI23CLKSOURCE_PIN
#define RCC_SAI3CLKSOURCE_CLKP        RCC_SAI23CLKSOURCE_CLKP
/**
  * @}
  */
#endif /* SAI3 */

#if defined(RCC_CDCCIP1R_SAI2ASEL)
/** @defgroup RCCEx_SAI2_Clock_A Source SAI2A Clock Source
  * @{
  */
#define RCC_SAI2ACLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI2ACLKSOURCE_PLL2         RCC_CDCCIP1R_SAI2ASEL_0
#define RCC_SAI2ACLKSOURCE_PLL3         RCC_CDCCIP1R_SAI2ASEL_1
#define RCC_SAI2ACLKSOURCE_PIN         (RCC_CDCCIP1R_SAI2ASEL_0 | RCC_CDCCIP1R_SAI2ASEL_1)
#define RCC_SAI2ACLKSOURCE_CLKP         RCC_CDCCIP1R_SAI2ASEL_2
#define RCC_SAI2ACLKSOURCE_SPDIF       (RCC_CDCCIP1R_SAI2ASEL_0 | RCC_CDCCIP1R_SAI2ASEL_2)
 /**
  * @}
  */
#endif /* RCC_CDCCIP1R_SAI2ASEL */

#if defined(RCC_CDCCIP1R_SAI2BSEL)
/** @defgroup RCCEx_SAI2_Block_B Clock_Source SAI2B Clock Source
  * @{
  */
#define RCC_SAI2BCLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI2BCLKSOURCE_PLL2         RCC_CDCCIP1R_SAI2BSEL_0
#define RCC_SAI2BCLKSOURCE_PLL3         RCC_CDCCIP1R_SAI2BSEL_1
#define RCC_SAI2BCLKSOURCE_PIN         (RCC_CDCCIP1R_SAI2BSEL_0 | RCC_CDCCIP1R_SAI2BSEL_1)
#define RCC_SAI2BCLKSOURCE_CLKP         RCC_CDCCIP1R_SAI2BSEL_2
#define RCC_SAI2BCLKSOURCE_SPDIF       (RCC_CDCCIP1R_SAI2BSEL_0 | RCC_CDCCIP1R_SAI2BSEL_2)
/**
  * @}
  */
#endif /* RCC_CDCCIP1R_SAI2BSEL */


/** @defgroup RCCEx_SPI123_Clock_Source SPI1/2/3 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_SPI123SEL)
#define RCC_SPI123CLKSOURCE_PLL         (0x00000000U)
#define RCC_SPI123CLKSOURCE_PLL2         RCC_D2CCIP1R_SPI123SEL_0
#define RCC_SPI123CLKSOURCE_PLL3         RCC_D2CCIP1R_SPI123SEL_1
#define RCC_SPI123CLKSOURCE_PIN         (RCC_D2CCIP1R_SPI123SEL_0 | RCC_D2CCIP1R_SPI123SEL_1)
#define RCC_SPI123CLKSOURCE_CLKP         RCC_D2CCIP1R_SPI123SEL_2
#else
#define RCC_SPI123CLKSOURCE_PLL         (0x00000000U)
#define RCC_SPI123CLKSOURCE_PLL2         RCC_CDCCIP1R_SPI123SEL_0
#define RCC_SPI123CLKSOURCE_PLL3         RCC_CDCCIP1R_SPI123SEL_1
#define RCC_SPI123CLKSOURCE_PIN         (RCC_CDCCIP1R_SPI123SEL_0 | RCC_CDCCIP1R_SPI123SEL_1)
#define RCC_SPI123CLKSOURCE_CLKP         RCC_CDCCIP1R_SPI123SEL_2
#endif /* RCC_D2CCIP1R_SPI123SEL */
/**
  * @}
  */

/** @defgroup RCCEx_SPI1_Clock_Source SPI1 Clock Source
  * @{
  */
#define RCC_SPI1CLKSOURCE_PLL         RCC_SPI123CLKSOURCE_PLL
#define RCC_SPI1CLKSOURCE_PLL2        RCC_SPI123CLKSOURCE_PLL2
#define RCC_SPI1CLKSOURCE_PLL3        RCC_SPI123CLKSOURCE_PLL3
#define RCC_SPI1CLKSOURCE_PIN         RCC_SPI123CLKSOURCE_PIN
#define RCC_SPI1CLKSOURCE_CLKP        RCC_SPI123CLKSOURCE_CLKP

/**
  * @}
  */

/** @defgroup RCCEx_SPI2_Clock_Source SPI2 Clock Source
  * @{
  */
#define RCC_SPI2CLKSOURCE_PLL         RCC_SPI123CLKSOURCE_PLL
#define RCC_SPI2CLKSOURCE_PLL2        RCC_SPI123CLKSOURCE_PLL2
#define RCC_SPI2CLKSOURCE_PLL3        RCC_SPI123CLKSOURCE_PLL3
#define RCC_SPI2CLKSOURCE_PIN         RCC_SPI123CLKSOURCE_PIN
#define RCC_SPI2CLKSOURCE_CLKP        RCC_SPI123CLKSOURCE_CLKP

/**
  * @}
  */

/** @defgroup RCCEx_SPI3_Clock_Source SPI3 Clock Source
  * @{
  */
#define RCC_SPI3CLKSOURCE_PLL         RCC_SPI123CLKSOURCE_PLL
#define RCC_SPI3CLKSOURCE_PLL2        RCC_SPI123CLKSOURCE_PLL2
#define RCC_SPI3CLKSOURCE_PLL3        RCC_SPI123CLKSOURCE_PLL3
#define RCC_SPI3CLKSOURCE_PIN         RCC_SPI123CLKSOURCE_PIN
#define RCC_SPI3CLKSOURCE_CLKP        RCC_SPI123CLKSOURCE_CLKP

/**
  * @}
  */

/** @defgroup RCCEx_SPI45_Clock_Source SPI4/5 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_SPI45SEL)
#define RCC_SPI45CLKSOURCE_D2PCLK1     (0x00000000U)
#define RCC_SPI45CLKSOURCE_PCLK1        RCC_SPI45CLKSOURCE_D2PCLK1
#define RCC_SPI45CLKSOURCE_PLL2         RCC_D2CCIP1R_SPI45SEL_0
#define RCC_SPI45CLKSOURCE_PLL3         RCC_D2CCIP1R_SPI45SEL_1
#define RCC_SPI45CLKSOURCE_HSI         (RCC_D2CCIP1R_SPI45SEL_0 | RCC_D2CCIP1R_SPI45SEL_1)
#define RCC_SPI45CLKSOURCE_CSI          RCC_D2CCIP1R_SPI45SEL_2
#define RCC_SPI45CLKSOURCE_HSE         (RCC_D2CCIP1R_SPI45SEL_0 | RCC_D2CCIP1R_SPI45SEL_2)
#else
#define RCC_SPI45CLKSOURCE_CDPCLK1     (0x00000000U)
/* aliases */
#define RCC_SPI45CLKSOURCE_D2PCLK1      RCC_SPI45CLKSOURCE_CDPCLK1  /* D2PCLK1 is used in STM32H74xxx, STM32H75xxx, STM32H72xxx and STM32H73xxx family lines */
#define RCC_SPI45CLKSOURCE_PCLK1        RCC_SPI45CLKSOURCE_CDPCLK1
#define RCC_SPI45CLKSOURCE_PLL2         RCC_CDCCIP1R_SPI45SEL_0
#define RCC_SPI45CLKSOURCE_PLL3         RCC_CDCCIP1R_SPI45SEL_1
#define RCC_SPI45CLKSOURCE_HSI         (RCC_CDCCIP1R_SPI45SEL_0 | RCC_CDCCIP1R_SPI45SEL_1)
#define RCC_SPI45CLKSOURCE_CSI          RCC_CDCCIP1R_SPI45SEL_2
#define RCC_SPI45CLKSOURCE_HSE         (RCC_CDCCIP1R_SPI45SEL_0 | RCC_CDCCIP1R_SPI45SEL_2)
#endif /* RCC_D2CCIP1R_SPI45SEL */
/**
  * @}
  */

/** @defgroup RCCEx_SPI4_Clock_Source SPI4 Clock Source
  * @{
  */
#define RCC_SPI4CLKSOURCE_D2PCLK1     RCC_SPI45CLKSOURCE_D2PCLK1
#define RCC_SPI4CLKSOURCE_PLL2        RCC_SPI45CLKSOURCE_PLL2
#define RCC_SPI4CLKSOURCE_PLL3        RCC_SPI45CLKSOURCE_PLL3
#define RCC_SPI4CLKSOURCE_HSI         RCC_SPI45CLKSOURCE_HSI
#define RCC_SPI4CLKSOURCE_CSI         RCC_SPI45CLKSOURCE_CSI
#define RCC_SPI4CLKSOURCE_HSE         RCC_SPI45CLKSOURCE_HSE

/**
  * @}
  */

/** @defgroup RCCEx_SPI5_Clock_Source SPI5 Clock Source
  * @{
  */
#define RCC_SPI5CLKSOURCE_D2PCLK1     RCC_SPI45CLKSOURCE_D2PCLK1
#define RCC_SPI5CLKSOURCE_PLL2        RCC_SPI45CLKSOURCE_PLL2
#define RCC_SPI5CLKSOURCE_PLL3        RCC_SPI45CLKSOURCE_PLL3
#define RCC_SPI5CLKSOURCE_HSI         RCC_SPI45CLKSOURCE_HSI
#define RCC_SPI5CLKSOURCE_CSI         RCC_SPI45CLKSOURCE_CSI
#define RCC_SPI5CLKSOURCE_HSE         RCC_SPI45CLKSOURCE_HSE

/**
  * @}
  */

/** @defgroup RCCEx_SPI6_Clock_Source SPI6 Clock Source
  * @{
  */
#if defined(RCC_D3CCIPR_SPI6SEL)
#define RCC_SPI6CLKSOURCE_D3PCLK1     (0x00000000U)
#define RCC_SPI6CLKSOURCE_PCLK4        RCC_SPI6CLKSOURCE_D3PCLK1
#define RCC_SPI6CLKSOURCE_PLL2         RCC_D3CCIPR_SPI6SEL_0
#define RCC_SPI6CLKSOURCE_PLL3         RCC_D3CCIPR_SPI6SEL_1
#define RCC_SPI6CLKSOURCE_HSI         (RCC_D3CCIPR_SPI6SEL_0 | RCC_D3CCIPR_SPI6SEL_1)
#define RCC_SPI6CLKSOURCE_CSI          RCC_D3CCIPR_SPI6SEL_2
#define RCC_SPI6CLKSOURCE_HSE         (RCC_D3CCIPR_SPI6SEL_0 | RCC_D3CCIPR_SPI6SEL_2)
#else
#define RCC_SPI6CLKSOURCE_SRDPCLK4    (0x00000000U)
/* alias */
#define RCC_SPI6CLKSOURCE_D3PCLK1      RCC_SPI6CLKSOURCE_SRDPCLK4  /* D3PCLK1 is used in STM32H74xxx, STM32H75xxx, STM32H72xxx and STM32H73xxx family lines */
#define RCC_SPI6CLKSOURCE_PCLK4        RCC_SPI6CLKSOURCE_SRDPCLK4
#define RCC_SPI6CLKSOURCE_PLL2         RCC_SRDCCIPR_SPI6SEL_0
#define RCC_SPI6CLKSOURCE_PLL3         RCC_SRDCCIPR_SPI6SEL_1
#define RCC_SPI6CLKSOURCE_HSI         (RCC_SRDCCIPR_SPI6SEL_0 | RCC_SRDCCIPR_SPI6SEL_1)
#define RCC_SPI6CLKSOURCE_CSI          RCC_SRDCCIPR_SPI6SEL_2
#define RCC_SPI6CLKSOURCE_HSE         (RCC_SRDCCIPR_SPI6SEL_0 | RCC_SRDCCIPR_SPI6SEL_2)
#define RCC_SPI6CLKSOURCE_PIN         (RCC_SRDCCIPR_SPI6SEL_1 | RCC_SRDCCIPR_SPI6SEL_2)
#endif /* RCC_D3CCIPR_SPI6SEL */

/**
  * @}
  */


#if defined(SAI4_Block_A)
/** @defgroup RCCEx_SAI4A_Clock_Source SAI4A Clock Source
  * @{
  */
#define RCC_SAI4ACLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI4ACLKSOURCE_PLL2         RCC_D3CCIPR_SAI4ASEL_0
#define RCC_SAI4ACLKSOURCE_PLL3         RCC_D3CCIPR_SAI4ASEL_1
#define RCC_SAI4ACLKSOURCE_PIN         (RCC_D3CCIPR_SAI4ASEL_0 | RCC_D3CCIPR_SAI4ASEL_1)
#define RCC_SAI4ACLKSOURCE_CLKP         RCC_D3CCIPR_SAI4ASEL_2

/**
  * @}
  */
#endif /* SAI4_Block_A */



#if defined(SAI4_Block_B)
/** @defgroup RCCEx_SAI4B_Clock_Source SAI4B Clock Source
  * @{
  */
#define RCC_SAI4BCLKSOURCE_PLL         (0x00000000U)
#define RCC_SAI4BCLKSOURCE_PLL2         RCC_D3CCIPR_SAI4BSEL_0
#define RCC_SAI4BCLKSOURCE_PLL3         RCC_D3CCIPR_SAI4BSEL_1
#define RCC_SAI4BCLKSOURCE_PIN         (RCC_D3CCIPR_SAI4BSEL_0 | RCC_D3CCIPR_SAI4BSEL_1)
#define RCC_SAI4BCLKSOURCE_CLKP         RCC_D3CCIPR_SAI4BSEL_2

/**
  * @}
  */
#endif /* SAI4_Block_B */


/** @defgroup RCCEx_LPTIM1_Clock_Source  RCCEx LPTIM1 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP2R_LPTIM1SEL)
#define RCC_LPTIM1CLKSOURCE_D2PCLK1        (0x00000000U)
/* alias */
#define RCC_LPTIM1CLKSOURCE_PCLK1         RCC_LPTIM1CLKSOURCE_D2PCLK1
#define RCC_LPTIM1CLKSOURCE_PLL2          RCC_D2CCIP2R_LPTIM1SEL_0
#define RCC_LPTIM1CLKSOURCE_PLL3          RCC_D2CCIP2R_LPTIM1SEL_1
#define RCC_LPTIM1CLKSOURCE_LSE          (RCC_D2CCIP2R_LPTIM1SEL_0 | RCC_D2CCIP2R_LPTIM1SEL_1)
#define RCC_LPTIM1CLKSOURCE_LSI           RCC_D2CCIP2R_LPTIM1SEL_2
#define RCC_LPTIM1CLKSOURCE_CLKP         (RCC_D2CCIP2R_LPTIM1SEL_0 | RCC_D2CCIP2R_LPTIM1SEL_2)
#else
#define RCC_LPTIM1CLKSOURCE_CDPCLK1        (0x00000000U)
/* alias */
#define RCC_LPTIM1CLKSOURCE_PCLK1         RCC_LPTIM1CLKSOURCE_CDPCLK1
#define RCC_LPTIM1CLKSOURCE_D2PCLK1       RCC_LPTIM1CLKSOURCE_CDPCLK1
#define RCC_LPTIM1CLKSOURCE_PLL2          RCC_CDCCIP2R_LPTIM1SEL_0
#define RCC_LPTIM1CLKSOURCE_PLL3          RCC_CDCCIP2R_LPTIM1SEL_1
#define RCC_LPTIM1CLKSOURCE_LSE          (RCC_CDCCIP2R_LPTIM1SEL_0 | RCC_CDCCIP2R_LPTIM1SEL_1)
#define RCC_LPTIM1CLKSOURCE_LSI           RCC_CDCCIP2R_LPTIM1SEL_2
#define RCC_LPTIM1CLKSOURCE_CLKP         (RCC_CDCCIP2R_LPTIM1SEL_0 | RCC_CDCCIP2R_LPTIM1SEL_2)
#endif /* RCC_D2CCIP2R_LPTIM1SEL */

/**
  * @}
  */

/** @defgroup RCCEx_LPTIM2_Clock_Source  RCCEx LPTIM2 Clock Source
  * @{
  */
#if defined(RCC_D3CCIPR_LPTIM2SEL)
#define RCC_LPTIM2CLKSOURCE_D3PCLK1       (0x00000000U)
/* alias */
#define RCC_LPTIM2CLKSOURCE_PCLK4         RCC_LPTIM2CLKSOURCE_D3PCLK1
#define RCC_LPTIM2CLKSOURCE_PLL2          RCC_D3CCIPR_LPTIM2SEL_0
#define RCC_LPTIM2CLKSOURCE_PLL3          RCC_D3CCIPR_LPTIM2SEL_1
#define RCC_LPTIM2CLKSOURCE_LSE          (RCC_D3CCIPR_LPTIM2SEL_0 | RCC_D3CCIPR_LPTIM2SEL_1)
#define RCC_LPTIM2CLKSOURCE_LSI           RCC_D3CCIPR_LPTIM2SEL_2
#define RCC_LPTIM2CLKSOURCE_CLKP         (RCC_D3CCIPR_LPTIM2SEL_0 | RCC_D3CCIPR_LPTIM2SEL_2)
#else
#define RCC_LPTIM2CLKSOURCE_SRDPCLK4       (0x00000000U)
/*alias*/
#define RCC_LPTIM2CLKSOURCE_PCLK4         RCC_LPTIM2CLKSOURCE_SRDPCLK4
#define RCC_LPTIM2CLKSOURCE_D3PCLK1       RCC_LPTIM2CLKSOURCE_SRDPCLK4
#define RCC_LPTIM2CLKSOURCE_PLL2          RCC_SRDCCIPR_LPTIM2SEL_0
#define RCC_LPTIM2CLKSOURCE_PLL3          RCC_SRDCCIPR_LPTIM2SEL_1
#define RCC_LPTIM2CLKSOURCE_LSE          (RCC_SRDCCIPR_LPTIM2SEL_0 | RCC_SRDCCIPR_LPTIM2SEL_1)
#define RCC_LPTIM2CLKSOURCE_LSI           RCC_SRDCCIPR_LPTIM2SEL_2
#define RCC_LPTIM2CLKSOURCE_CLKP         (RCC_SRDCCIPR_LPTIM2SEL_0 | RCC_SRDCCIPR_LPTIM2SEL_2)
#endif /* RCC_D3CCIPR_LPTIM2SEL */
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM345_Clock_Source  RCCEx LPTIM3/4/5 Clock Source
  * @{
  */
#if defined(RCC_D3CCIPR_LPTIM345SEL)
#define RCC_LPTIM345CLKSOURCE_D3PCLK1        (0x00000000U)
/* alias*/
#define RCC_LPTIM345CLKSOURCE_PCLK4         RCC_LPTIM345CLKSOURCE_D3PCLK1
#define RCC_LPTIM345CLKSOURCE_PLL2          RCC_D3CCIPR_LPTIM345SEL_0
#define RCC_LPTIM345CLKSOURCE_PLL3          RCC_D3CCIPR_LPTIM345SEL_1
#define RCC_LPTIM345CLKSOURCE_LSE          (RCC_D3CCIPR_LPTIM345SEL_0 | RCC_D3CCIPR_LPTIM345SEL_1)
#define RCC_LPTIM345CLKSOURCE_LSI           RCC_D3CCIPR_LPTIM345SEL_2
#define RCC_LPTIM345CLKSOURCE_CLKP         (RCC_D3CCIPR_LPTIM345SEL_0 | RCC_D3CCIPR_LPTIM345SEL_2)
#else
#define RCC_LPTIM345CLKSOURCE_SRDPCLK4      (0x00000000U)
/* alias */
#define RCC_LPTIM345CLKSOURCE_PCLK4         RCC_LPTIM345CLKSOURCE_SRDPCLK4
#define RCC_LPTIM345CLKSOURCE_D3PCLK1       RCC_LPTIM345CLKSOURCE_SRDPCLK4
#define RCC_LPTIM345CLKSOURCE_PLL2          RCC_SRDCCIPR_LPTIM3SEL_0
#define RCC_LPTIM345CLKSOURCE_PLL3          RCC_SRDCCIPR_LPTIM3SEL_1
#define RCC_LPTIM345CLKSOURCE_LSE          (RCC_SRDCCIPR_LPTIM3SEL_0 | RCC_SRDCCIPR_LPTIM3SEL_1)
#define RCC_LPTIM345CLKSOURCE_LSI           RCC_SRDCCIPR_LPTIM3SEL_2
#define RCC_LPTIM345CLKSOURCE_CLKP         (RCC_SRDCCIPR_LPTIM3SEL_0 | RCC_SRDCCIPR_LPTIM3SEL_2)
#endif /* RCC_D3CCIPR_LPTIM345SEL */
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM3_Clock_Source  RCCEx LPTIM3 Clock Source
  * @{
  */
#define RCC_LPTIM3CLKSOURCE_D3PCLK1       RCC_LPTIM345CLKSOURCE_D3PCLK1
#define RCC_LPTIM3CLKSOURCE_PLL2          RCC_LPTIM345CLKSOURCE_PLL2
#define RCC_LPTIM3CLKSOURCE_PLL3          RCC_LPTIM345CLKSOURCE_PLL3
#define RCC_LPTIM3CLKSOURCE_LSE           RCC_LPTIM345CLKSOURCE_LSE
#define RCC_LPTIM3CLKSOURCE_LSI           RCC_LPTIM345CLKSOURCE_LSI
#define RCC_LPTIM3CLKSOURCE_CLKP          RCC_LPTIM345CLKSOURCE_CLKP

/**
  * @}
  */
#if defined(LPTIM4)
/** @defgroup RCCEx_LPTIM4_Clock_Source  RCCEx LPTIM4 Clock Source
  * @{
  */
#define RCC_LPTIM4CLKSOURCE_D3PCLK1       RCC_LPTIM345CLKSOURCE_D3PCLK1
#define RCC_LPTIM4CLKSOURCE_PLL2          RCC_LPTIM345CLKSOURCE_PLL2
#define RCC_LPTIM4CLKSOURCE_PLL3          RCC_LPTIM345CLKSOURCE_PLL3
#define RCC_LPTIM4CLKSOURCE_LSE           RCC_LPTIM345CLKSOURCE_LSE
#define RCC_LPTIM4CLKSOURCE_LSI           RCC_LPTIM345CLKSOURCE_LSI
#define RCC_LPTIM4CLKSOURCE_CLKP          RCC_LPTIM345CLKSOURCE_CLKP
/**
  * @}
  */
#endif /* LPTIM4 */

#if defined(LPTIM5)
/** @defgroup RCCEx_LPTIM5_Clock_Source  RCCEx LPTIM5 Clock Source
  * @{
  */
#define RCC_LPTIM5CLKSOURCE_D3PCLK1       RCC_LPTIM345CLKSOURCE_D3PCLK1
#define RCC_LPTIM5CLKSOURCE_PLL2          RCC_LPTIM345CLKSOURCE_PLL2
#define RCC_LPTIM5CLKSOURCE_PLL3          RCC_LPTIM345CLKSOURCE_PLL3
#define RCC_LPTIM5CLKSOURCE_LSE           RCC_LPTIM345CLKSOURCE_LSE
#define RCC_LPTIM5CLKSOURCE_LSI           RCC_LPTIM345CLKSOURCE_LSI
#define RCC_LPTIM5CLKSOURCE_CLKP          RCC_LPTIM345CLKSOURCE_CLKP

/**
  * @}
  */
#endif /* LPTIM5 */

#if defined(QUADSPI)
/** @defgroup RCCEx_QSPI_Clock_Source  RCCEx QSPI Clock Source
  * @{
  */
#define RCC_QSPICLKSOURCE_D1HCLK       (0x00000000U)
#define RCC_QSPICLKSOURCE_PLL          RCC_D1CCIPR_QSPISEL_0
#define RCC_QSPICLKSOURCE_PLL2         RCC_D1CCIPR_QSPISEL_1
#define RCC_QSPICLKSOURCE_CLKP         RCC_D1CCIPR_QSPISEL

/**
  * @}
  */
#endif /* QUADSPI */


#if defined(OCTOSPI1) || defined(OCTOSPI2)
/** @defgroup RCCEx_OSPI_Clock_Source  RCCEx OSPI Clock Source
  * @{
  */
#define RCC_OSPICLKSOURCE_CDHCLK       (0x00000000U)
/*aliases*/
#define RCC_OSPICLKSOURCE_D1HCLK       RCC_OSPICLKSOURCE_CDHCLK
#define RCC_OSPICLKSOURCE_HCLK         RCC_OSPICLKSOURCE_CDHCLK
#define RCC_OSPICLKSOURCE_PLL          RCC_CDCCIPR_OCTOSPISEL_0
#define RCC_OSPICLKSOURCE_PLL2         RCC_CDCCIPR_OCTOSPISEL_1
#define RCC_OSPICLKSOURCE_CLKP         RCC_CDCCIPR_OCTOSPISEL

/**
  * @}
  */
#endif /* defined(OCTOSPI1) || defined(OCTOSPI2) */

#if defined(DSI)
/** @defgroup RCCEx_DSI_Clock_Source  RCCEx DSI Clock Source
  * @{
  */
#define RCC_DSICLKSOURCE_PHY       (0x00000000U)
#define RCC_DSICLKSOURCE_PLL2       RCC_D1CCIPR_DSISEL

/**
  * @}
  */
#endif /* DSI */

/** @defgroup RCCEx_FMC_Clock_Source  RCCEx FMC Clock Source
  * @{
  */
#if defined(RCC_D1CCIPR_FMCSEL)
#define RCC_FMCCLKSOURCE_D1HCLK       (0x00000000U)
#define RCC_FMCCLKSOURCE_HCLK         RCC_FMCCLKSOURCE_D1HCLK
#define RCC_FMCCLKSOURCE_PLL          RCC_D1CCIPR_FMCSEL_0
#define RCC_FMCCLKSOURCE_PLL2         RCC_D1CCIPR_FMCSEL_1
#define RCC_FMCCLKSOURCE_CLKP         RCC_D1CCIPR_FMCSEL
#else
#define RCC_FMCCLKSOURCE_CDHCLK       (0x00000000U)
#define RCC_FMCCLKSOURCE_HCLK         RCC_FMCCLKSOURCE_CDHCLK
/*alias*/
#define RCC_FMCCLKSOURCE_D1HCLK       RCC_FMCCLKSOURCE_CDHCLK
#define RCC_FMCCLKSOURCE_PLL          RCC_CDCCIPR_FMCSEL_0
#define RCC_FMCCLKSOURCE_PLL2         RCC_CDCCIPR_FMCSEL_1
#define RCC_FMCCLKSOURCE_CLKP         RCC_CDCCIPR_FMCSEL
#endif /* RCC_D1CCIPR_FMCSEL */
/**
  * @}
  */

#if defined(FDCAN1) || defined(FDCAN2)
/** @defgroup RCCEx_FDCAN_Clock_Source  RCCEx FDCAN Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_FDCANSEL)
#define RCC_FDCANCLKSOURCE_HSE         (0x00000000U)
#define RCC_FDCANCLKSOURCE_PLL          RCC_D2CCIP1R_FDCANSEL_0
#define RCC_FDCANCLKSOURCE_PLL2         RCC_D2CCIP1R_FDCANSEL_1
#else
#define RCC_FDCANCLKSOURCE_HSE         (0x00000000U)
#define RCC_FDCANCLKSOURCE_PLL          RCC_CDCCIP1R_FDCANSEL_0
#define RCC_FDCANCLKSOURCE_PLL2         RCC_CDCCIP1R_FDCANSEL_1
#endif /* D3_SRAM_BASE */
/**
  * @}
  */
#endif /*FDCAN1 || FDCAN2*/


/** @defgroup RCCEx_SDMMC_Clock_Source  RCCEx SDMMC Clock Source
  * @{
  */
#if defined(RCC_D1CCIPR_SDMMCSEL)
#define RCC_SDMMCCLKSOURCE_PLL           (0x00000000U)
#define RCC_SDMMCCLKSOURCE_PLL2           RCC_D1CCIPR_SDMMCSEL
#else
#define RCC_SDMMCCLKSOURCE_PLL           (0x00000000U)
#define RCC_SDMMCCLKSOURCE_PLL2           RCC_CDCCIPR_SDMMCSEL
#endif /* RCC_D1CCIPR_SDMMCSEL */
/**
  * @}
  */


/** @defgroup RCCEx_ADC_Clock_Source  RCCEx ADC Clock Source
  * @{
  */
#if defined(RCC_D3CCIPR_ADCSEL_0)
#define RCC_ADCCLKSOURCE_PLL2       (0x00000000U)
#define RCC_ADCCLKSOURCE_PLL3       RCC_D3CCIPR_ADCSEL_0
#define RCC_ADCCLKSOURCE_CLKP       RCC_D3CCIPR_ADCSEL_1
#else
#define RCC_ADCCLKSOURCE_PLL2       (0x00000000U)
#define RCC_ADCCLKSOURCE_PLL3       RCC_SRDCCIPR_ADCSEL_0
#define RCC_ADCCLKSOURCE_CLKP       RCC_SRDCCIPR_ADCSEL_1
#endif /* RCC_D3CCIPR_ADCSEL_0  */
/**
  * @}
  */

/** @defgroup RCCEx_SWPMI1_Clock_Source  RCCEx SWPMI1 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_SWPSEL)
#define RCC_SWPMI1CLKSOURCE_D2PCLK1       (0x00000000U)
#define RCC_SWPMI1CLKSOURCE_HSI            RCC_D2CCIP1R_SWPSEL
#else
#define RCC_SWPMI1CLKSOURCE_CDPCLK1       (0x00000000U)
/* alias */
#define RCC_SWPMI1CLKSOURCE_D2PCLK1        RCC_SWPMI1CLKSOURCE_CDPCLK1
#define RCC_SWPMI1CLKSOURCE_HSI            RCC_CDCCIP1R_SWPSEL
#endif /* RCC_D2CCIP1R_SWPSEL */
/**
  * @}
  */

/** @defgroup RCCEx_DFSDM1_Clock_Source  RCCEx DFSDM1 Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_DFSDM1SEL)
#define RCC_DFSDM1CLKSOURCE_D2PCLK1        (0x00000000U)
#define RCC_DFSDM1CLKSOURCE_SYS            RCC_D2CCIP1R_DFSDM1SEL
#else
#define RCC_DFSDM1CLKSOURCE_CDPCLK1        (0x00000000U)
/* alias */
#define RCC_DFSDM1CLKSOURCE_D2PCLK1        RCC_DFSDM1CLKSOURCE_CDPCLK1
#define RCC_DFSDM1CLKSOURCE_SYS            RCC_CDCCIP1R_DFSDM1SEL
#endif /* RCC_D2CCIP1R_DFSDM1SEL */
/**
  * @}
  */

#if defined(DFSDM2_BASE)
/** @defgroup RCCEx_DFSDM2_Clock_Source  RCCEx DFSDM2 Clock Source
  * @{
  */
#define RCC_DFSDM2CLKSOURCE_SRDPCLK4       (0x00000000U)
/* alias */
#define RCC_DFSDM2CLKSOURCE_SRDPCLK1       RCC_DFSDM2CLKSOURCE_SRDPCLK4
#define RCC_DFSDM2CLKSOURCE_SYS            RCC_SRDCCIPR_DFSDM2SEL
/**
  * @}
  */
#endif /* DFSDM2 */

/** @defgroup RCCEx_SPDIFRX_Clock_Source  RCCEx SPDIFRX Clock Source
  * @{
  */
#if defined(RCC_D2CCIP1R_SPDIFSEL_0)
#define RCC_SPDIFRXCLKSOURCE_PLL        (0x00000000U)
#define RCC_SPDIFRXCLKSOURCE_PLL2       RCC_D2CCIP1R_SPDIFSEL_0
#define RCC_SPDIFRXCLKSOURCE_PLL3       RCC_D2CCIP1R_SPDIFSEL_1
#define RCC_SPDIFRXCLKSOURCE_HSI        RCC_D2CCIP1R_SPDIFSEL
#else
#define RCC_SPDIFRXCLKSOURCE_PLL        (0x00000000U)
#define RCC_SPDIFRXCLKSOURCE_PLL2       RCC_CDCCIP1R_SPDIFSEL_0
#define RCC_SPDIFRXCLKSOURCE_PLL3       RCC_CDCCIP1R_SPDIFSEL_1
#define RCC_SPDIFRXCLKSOURCE_HSI        RCC_CDCCIP1R_SPDIFSEL
#endif /* RCC_D2CCIP1R_SPDIFSEL_0 */
/**
  * @}
  */

/** @defgroup RCCEx_CEC_Clock_Source  RCCEx CEC Clock Source
  * @{
  */
#if defined(RCC_D2CCIP2R_CECSEL_0)
#define RCC_CECCLKSOURCE_LSE        (0x00000000U)
#define RCC_CECCLKSOURCE_LSI         RCC_D2CCIP2R_CECSEL_0
#define RCC_CECCLKSOURCE_CSI         RCC_D2CCIP2R_CECSEL_1
#else
#define RCC_CECCLKSOURCE_LSE        (0x00000000U)
#define RCC_CECCLKSOURCE_LSI         RCC_CDCCIP2R_CECSEL_0
#define RCC_CECCLKSOURCE_CSI         RCC_CDCCIP2R_CECSEL_1
#endif /* RCC_D2CCIP2R_CECSEL_0 */
/**
  * @}
  */


/** @defgroup RCCEx_CLKP_Clock_Source  RCCEx CLKP Clock Source
  * @{
  */
#if defined(RCC_D1CCIPR_CKPERSEL_0)
#define RCC_CLKPSOURCE_HSI        (0x00000000U)
#define RCC_CLKPSOURCE_CSI         RCC_D1CCIPR_CKPERSEL_0
#define RCC_CLKPSOURCE_HSE         RCC_D1CCIPR_CKPERSEL_1
#else
#define RCC_CLKPSOURCE_HSI        (0x00000000U)
#define RCC_CLKPSOURCE_CSI         RCC_CDCCIPR_CKPERSEL_0
#define RCC_CLKPSOURCE_HSE         RCC_CDCCIPR_CKPERSEL_1
#endif /* RCC_D1CCIPR_CKPERSEL_0 */
/**
  * @}
  */

/** @defgroup RCCEx_TIM_Prescaler_Selection RCCEx TIM Prescaler Selection
  * @{
  */
#define RCC_TIMPRES_DESACTIVATED        (0x00000000U)
#define RCC_TIMPRES_ACTIVATED            RCC_CFGR_TIMPRE

/**
  * @}
  */

#if defined(DUAL_CORE)

/** @defgroup RCCEx_RCC_BootCx RCCEx RCC BootCx
  * @{
  */
#define RCC_BOOT_C1        RCC_GCR_BOOT_C1
#define RCC_BOOT_C2        RCC_GCR_BOOT_C2

/**
  * @}
  */
#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
/** @defgroup RCCEx_RCC_WWDGx  RCCEx RCC WWDGx
  * @{
  */
#define RCC_WWDG1        RCC_GCR_WW1RSC
#define RCC_WWDG2        RCC_GCR_WW2RSC

/**
  * @}
  */

#else

/** @defgroup RCCEx_RCC_WWDGx  RCCEx RCC WWDGx
  * @{
  */
#define RCC_WWDG1        RCC_GCR_WW1RSC

/**
  * @}
  */

#endif /*DUAL_CORE*/

/** @defgroup RCCEx_EXTI_LINE_LSECSS  RCC LSE CSS external interrupt line
  * @{
  */
#define RCC_EXTI_LINE_LSECSS           EXTI_IMR1_IM18        /*!< External interrupt line 19 connected to the LSE CSS EXTI Line */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_Status RCCEx CRS Status
  * @{
  */
#define RCC_CRS_NONE                   (0x00000000U)
#define RCC_CRS_TIMEOUT                (0x00000001U)
#define RCC_CRS_SYNCOK                 (0x00000002U)
#define RCC_CRS_SYNCWARN               (0x00000004U)
#define RCC_CRS_SYNCERR                (0x00000008U)
#define RCC_CRS_SYNCMISS               (0x00000010U)
#define RCC_CRS_TRIMOVF                (0x00000020U)
/**
  * @}
  */

/** @defgroup RCCEx_CRS_SynchroSource RCCEx CRS SynchroSource
  * @{
  */
#define RCC_CRS_SYNC_SOURCE_PIN       (0x00000000U)                            /*!< Synchro Signal source external pin, Available on STM32H7 Rev.B and above devices only */
#define RCC_CRS_SYNC_SOURCE_LSE        CRS_CFGR_SYNCSRC_0                      /*!< Synchro Signal source LSE */
#define RCC_CRS_SYNC_SOURCE_USB1       CRS_CFGR_SYNCSRC_1                      /*!< Synchro Signal source USB1 SOF (default) */
#define RCC_CRS_SYNC_SOURCE_USB2      (CRS_CFGR_SYNCSRC_1|CRS_CFGR_SYNCSRC_0)  /*!< Synchro Signal source USB2 SOF */


/**
  * @}
  */

/** @defgroup RCCEx_CRS_SynchroDivider RCCEx CRS SynchroDivider
  * @{
  */
#define RCC_CRS_SYNC_DIV1        (0x00000000U)           /*!< Synchro Signal not divided (default) */
#define RCC_CRS_SYNC_DIV2        CRS_CFGR_SYNCDIV_0                         /*!< Synchro Signal divided by 2 */
#define RCC_CRS_SYNC_DIV4        CRS_CFGR_SYNCDIV_1                         /*!< Synchro Signal divided by 4 */
#define RCC_CRS_SYNC_DIV8        (CRS_CFGR_SYNCDIV_1 | CRS_CFGR_SYNCDIV_0)  /*!< Synchro Signal divided by 8 */
#define RCC_CRS_SYNC_DIV16       CRS_CFGR_SYNCDIV_2                        /*!< Synchro Signal divided by 16 */
#define RCC_CRS_SYNC_DIV32       (CRS_CFGR_SYNCDIV_2 | CRS_CFGR_SYNCDIV_0) /*!< Synchro Signal divided by 32 */
#define RCC_CRS_SYNC_DIV64       (CRS_CFGR_SYNCDIV_2 | CRS_CFGR_SYNCDIV_1) /*!< Synchro Signal divided by 64 */
#define RCC_CRS_SYNC_DIV128      CRS_CFGR_SYNCDIV                         /*!< Synchro Signal divided by 128 */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_SynchroPolarity RCCEx CRS SynchroPolarity
  * @{
  */
#define RCC_CRS_SYNC_POLARITY_RISING   (0x00000000U) /*!< Synchro Active on rising edge (default) */
#define RCC_CRS_SYNC_POLARITY_FALLING  CRS_CFGR_SYNCPOL        /*!< Synchro Active on falling edge */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_ReloadValueDefault RCCEx CRS ReloadValueDefault
  * @{
  */
#define RCC_CRS_RELOADVALUE_DEFAULT    (0x0000BB7FU) /*!< The reset value of the RELOAD field corresponds
                                                                    to a target frequency of 48 MHz and a synchronization signal frequency of 1 kHz (SOF signal from USB). */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_ErrorLimitDefault RCCEx CRS ErrorLimitDefault
  * @{
  */
#define RCC_CRS_ERRORLIMIT_DEFAULT     (0x00000022U) /*!< Default Frequency error limit */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_HSI48CalibrationDefault RCCEx CRS HSI48CalibrationDefault
  * @{
  */
#define RCC_CRS_HSI48CALIBRATION_DEFAULT (0x00000020U) /*!< The default value is 32, which corresponds to the middle of the trimming interval.
                                                                      The trimming step is around 67 kHz between two consecutive TRIM steps. A higher TRIM value
                                                                      corresponds to a higher output frequency */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_FreqErrorDirection RCCEx CRS FreqErrorDirection
  * @{
  */
#define RCC_CRS_FREQERRORDIR_UP        (0x00000000U)   /*!< Upcounting direction, the actual frequency is above the target */
#define RCC_CRS_FREQERRORDIR_DOWN      (CRS_ISR_FEDIR) /*!< Downcounting direction, the actual frequency is below the target */
/**
  * @}
  */

/** @defgroup RCCEx_CRS_Interrupt_Sources RCCEx CRS Interrupt Sources
  * @{
  */
#define RCC_CRS_IT_SYNCOK              CRS_CR_SYNCOKIE       /*!< SYNC event OK */
#define RCC_CRS_IT_SYNCWARN            CRS_CR_SYNCWARNIE     /*!< SYNC warning */
#define RCC_CRS_IT_ERR                 CRS_CR_ERRIE          /*!< Error */
#define RCC_CRS_IT_ESYNC               CRS_CR_ESYNCIE        /*!< Expected SYNC */
#define RCC_CRS_IT_SYNCERR             CRS_CR_ERRIE          /*!< SYNC error */
#define RCC_CRS_IT_SYNCMISS            CRS_CR_ERRIE          /*!< SYNC missed */
#define RCC_CRS_IT_TRIMOVF             CRS_CR_ERRIE          /*!< Trimming overflow or underflow */

/**
  * @}
  */

/** @defgroup RCCEx_CRS_Flags RCCEx CRS Flags
  * @{
  */
#define RCC_CRS_FLAG_SYNCOK            CRS_ISR_SYNCOKF       /*!< SYNC event OK flag     */
#define RCC_CRS_FLAG_SYNCWARN          CRS_ISR_SYNCWARNF     /*!< SYNC warning flag      */
#define RCC_CRS_FLAG_ERR               CRS_ISR_ERRF          /*!< Error flag        */
#define RCC_CRS_FLAG_ESYNC             CRS_ISR_ESYNCF        /*!< Expected SYNC flag     */
#define RCC_CRS_FLAG_SYNCERR           CRS_ISR_SYNCERR       /*!< SYNC error */
#define RCC_CRS_FLAG_SYNCMISS          CRS_ISR_SYNCMISS      /*!< SYNC missed*/
#define RCC_CRS_FLAG_TRIMOVF           CRS_ISR_TRIMOVF       /*!< Trimming overflow or underflow */

/**
  * @}
  */

/**
  * @}
  */



/* Exported macro ------------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Macros RCCEx Exported Macros
  * @{
  */

/** @brief  Macros to enable or disable PLL2.
  * @note   After enabling PLL2, the application software should wait on
  *         PLL2RDY flag to be set indicating that PLL2 clock is stable and can
  *         be used as kernel clock source.
  * @note   PLL2 is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLL2_ENABLE()         SET_BIT(RCC->CR, RCC_CR_PLL2ON)
#define __HAL_RCC_PLL2_DISABLE()        CLEAR_BIT(RCC->CR, RCC_CR_PLL2ON)

/**
  * @brief  Enables or disables each clock output (PLL2_P_CLK, PLL2_Q_CLK, PLL2_R_CLK)
  * @note   Enabling/disabling  those Clocks can be done only when the PLL2 is disabled,
  *         This is mainly used to save Power.
  * @param  __RCC_PLL2ClockOut__ Specifies the PLL2 clock to be outputted
  *          This parameter can be one of the following values:
  *            @arg RCC_PLL2_DIVP: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *            @arg RCC_PLL2_DIVQ: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *            @arg RCC_PLL2_DIVR: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define __HAL_RCC_PLL2CLKOUT_ENABLE(__RCC_PLL2ClockOut__)   SET_BIT(RCC->PLLCFGR, (__RCC_PLL2ClockOut__))

#define __HAL_RCC_PLL2CLKOUT_DISABLE(__RCC_PLL2ClockOut__)  CLEAR_BIT(RCC->PLLCFGR, (__RCC_PLL2ClockOut__))

/**
  * @brief  Enables or disables Fractional Part Of The Multiplication Factor of PLL2 VCO
  * @note   Enabling/disabling  Fractional Part can be any time  without the need to stop the PLL2
  * @retval None
  */
#define __HAL_RCC_PLL2FRACN_ENABLE()   SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL2FRACEN)

#define __HAL_RCC_PLL2FRACN_DISABLE()  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL2FRACEN)

/**
  * @brief  Macro to configures the PLL2  multiplication and division factors.
  * @note   This function must be used only when PLL2 is disabled.
  *
  * @param  __PLL2M__ specifies the division factor for PLL2 VCO input clock
  *          This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 16 MHz.
  *
  * @param  __PLL2N__ specifies the multiplication factor for PLL2 VCO output clock
  *          This parameter must be a number between 4 and 512 or between 8 and 420(*).
  * @note   You have to set the PLL2N parameter correctly to ensure that the VCO
  *         output frequency is between 150 and 420 MHz (when in medium VCO range) or
  *         between 192 and 836 MHZ or between 128 and 560 MHZ(*) (when in wide VCO range)
  *
  * @param  __PLL2P__ specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128.
  *
  * @param  __PLL2Q__ specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128.
  *
  * @param  __PLL2R__ specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128.
  *
  * @note   To insure an optimal behavior of the PLL when one of the post-divider (DIVP, DIVQ or DIVR)
  *         is not used, application shall clear the enable bit (DIVyEN) and assign lowest possible
  *         value to  __PLL2P__, __PLL2Q__ or __PLL2R__ parameters.
  * @retval None
  *
  *  (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  */

#define __HAL_RCC_PLL2_CONFIG(__PLL2M__, __PLL2N__, __PLL2P__, __PLL2Q__,__PLL2R__ ) \
                  do{ \
                       MODIFY_REG(RCC->PLLCKSELR, ( RCC_PLLCKSELR_DIVM2) , ( (__PLL2M__) <<12U));  \
                       WRITE_REG (RCC->PLL2DIVR , ( (((__PLL2N__) - 1U ) & RCC_PLL2DIVR_N2) | ((((__PLL2P__) -1U ) << 9U) & RCC_PLL2DIVR_P2) | \
                       ((((__PLL2Q__) -1U) << 16U) & RCC_PLL2DIVR_Q2) | ((((__PLL2R__)- 1U) << 24U) & RCC_PLL2DIVR_R2))); \
                    } while(0)

/**
  * @brief  Macro to configures PLL2 clock Fractional Part Of The Multiplication Factor
  *
  * @note   These bits can be written at any time, allowing dynamic fine-tuning of the PLL2 VCO
  *
  * @param  __RCC_PLL2FRACN__ Specifies Fractional Part Of The Multiplication factor for PLL2 VCO
  *                           It should be a value between 0 and 8191
  * @note   Warning: the software has to set correctly these bits to insure that the VCO
  *                  output frequency is between its valid frequency range, which is:
  *                  192 to 836 MHz or 128 to 560 MHz(*) if PLL2VCOSEL = 0
  *                  150 to 420 MHz if PLL2VCOSEL = 1.
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define  __HAL_RCC_PLL2FRACN_CONFIG(__RCC_PLL2FRACN__) \
                 MODIFY_REG(RCC->PLL2FRACR, RCC_PLL2FRACR_FRACN2,((uint32_t)(__RCC_PLL2FRACN__) << RCC_PLL2FRACR_FRACN2_Pos))

/** @brief  Macro to select  the PLL2  reference frequency range.
  * @param  __RCC_PLL2VCIRange__ specifies the PLL2 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL2VCIRANGE_0: Range frequency is between 1 and 2 MHz
  *            @arg RCC_PLL2VCIRANGE_1: Range frequency is between 2 and 4 MHz
  *            @arg RCC_PLL2VCIRANGE_2: Range frequency is between 4 and 8 MHz
  *            @arg RCC_PLL2VCIRANGE_3: Range frequency is between 8 and 16 MHz
  * @retval None
  */
#define __HAL_RCC_PLL2_VCIRANGE(__RCC_PLL2VCIRange__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL2RGE, (__RCC_PLL2VCIRange__))


/** @brief  Macro to select  the PLL2  reference frequency range.
  * @param  __RCC_PLL2VCORange__ Specifies the PLL2 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL2VCOWIDE: Range frequency is between 192 and 836 MHz or between 128 to 560 MHz(*)
  *            @arg RCC_PLL2VCOMEDIUM: Range frequency is between 150 and 420 MHz
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define __HAL_RCC_PLL2_VCORANGE(__RCC_PLL2VCORange__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL2VCOSEL, (__RCC_PLL2VCORange__))

/** @brief  Macros to enable or disable the main PLL3.
  * @note   After enabling  PLL3, the application software should wait on
  *         PLL3RDY flag to be set indicating that PLL3 clock is stable and can
  *         be used as kernel clock source.
  * @note   PLL3 is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLL3_ENABLE()         SET_BIT(RCC->CR, RCC_CR_PLL3ON)
#define __HAL_RCC_PLL3_DISABLE()        CLEAR_BIT(RCC->CR, RCC_CR_PLL3ON)

/**
  * @brief  Enables or disables Fractional Part Of The Multiplication Factor of PLL3 VCO
  * @note   Enabling/disabling  Fractional Part can be any time  without the need to stop the PLL3
  * @retval None
  */
#define __HAL_RCC_PLL3FRACN_ENABLE()   SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL3FRACEN)

#define __HAL_RCC_PLL3FRACN_DISABLE()  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL3FRACEN)

/**
  * @brief  Enables or disables each clock output (PLL3_P_CLK, PLL3_Q_CLK, PLL3_R_CLK)
  * @note   Enabling/disabling  those Clocks can be done only when the PLL3 is disabled,
  *         This is mainly used to save Power.
  * @param  __RCC_PLL3ClockOut__ specifies the PLL3 clock to be outputted
  *          This parameter can be one of the following values:
  *            @arg RCC_PLL3_DIVP: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *            @arg RCC_PLL3_DIVQ: This clock is used to generate peripherals clock  up to 400MHZ or 280MHZ(*)
  *            @arg RCC_PLL3_DIVR: This clock is used to generate peripherals clock up to 400MHZ or 280MHZ(*)
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define __HAL_RCC_PLL3CLKOUT_ENABLE(__RCC_PLL3ClockOut__)   SET_BIT(RCC->PLLCFGR, (__RCC_PLL3ClockOut__))

#define __HAL_RCC_PLL3CLKOUT_DISABLE(__RCC_PLL3ClockOut__)  CLEAR_BIT(RCC->PLLCFGR, (__RCC_PLL3ClockOut__))

/**
  * @brief  Macro to configures the PLL3  multiplication and division factors.
  * @note   This function must be used only when PLL3 is disabled.
  *
  * @param  __PLL3M__ specifies the division factor for PLL3 VCO input clock
  *          This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 16 MHz.
  *
  * @param  __PLL3N__ specifies the multiplication factor for PLL3 VCO output clock
  *          This parameter must be a number between 4 and 512.
  * @note   You have to set the PLL3N parameter correctly to ensure that the VCO
  *         output frequency is between 150 and 420 MHz (when in medium VCO range) or
  *         between 192 and 836 MHZ or between 128 and 560 MHZ(*) (when in wide VCO range)
  *
  * @param  __PLL3P__ specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 2 and 128 (where odd numbers not allowed)
  *
  * @param  __PLL3Q__ specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128
  *
  * @param  __PLL3R__ specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128
  *
  * @note   To insure an optimal behavior of the PLL when one of the post-divider (DIVP, DIVQ or DIVR)
  *         is not used, application shall clear the enable bit (DIVyEN) and assign lowest possible
  *         value to  __PLL3P__, __PLL3Q__ or __PLL3R__ parameters.
  * @retval None
  *
  *  (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  */

#define __HAL_RCC_PLL3_CONFIG(__PLL3M__, __PLL3N__, __PLL3P__, __PLL3Q__,__PLL3R__ ) \
                  do{ MODIFY_REG(RCC->PLLCKSELR, ( RCC_PLLCKSELR_DIVM3) , ( (__PLL3M__) <<20U));  \
                         WRITE_REG (RCC->PLL3DIVR , ( (((__PLL3N__) - 1U ) & RCC_PLL3DIVR_N3) | ((((__PLL3P__) -1U ) << 9U) & RCC_PLL3DIVR_P3) | \
                                   ((((__PLL3Q__) -1U) << 16U) & RCC_PLL3DIVR_Q3) | ((((__PLL3R__) - 1U) << 24U) & RCC_PLL3DIVR_R3))); \
                       } while(0)



/**
  * @brief  Macro to configures  PLL3 clock Fractional Part of The Multiplication Factor
  *
  * @note   These bits can be written at any time, allowing dynamic fine-tuning of the PLL3 VCO
  *
  * @param  __RCC_PLL3FRACN__ specifies Fractional Part Of The Multiplication Factor for PLL3 VCO
  *                            It should be a value between 0 and 8191
  * @note   Warning: the software has to set correctly these bits to insure that the VCO
  *                  output frequency is between its valid frequency range, which is:
  *                  192 to 836 MHz or 128 to 560 MHz(*) if PLL3VCOSEL = 0
  *                  150 to 420 MHz if PLL3VCOSEL = 1.
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
 #define  __HAL_RCC_PLL3FRACN_CONFIG(__RCC_PLL3FRACN__) MODIFY_REG(RCC->PLL3FRACR, RCC_PLL3FRACR_FRACN3, (uint32_t)(__RCC_PLL3FRACN__) << RCC_PLL3FRACR_FRACN3_Pos)

/** @brief  Macro to select  the PLL3  reference frequency range.
  * @param  __RCC_PLL3VCIRange__ specifies the PLL1 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL3VCIRANGE_0: Range frequency is between 1 and 2 MHz
  *            @arg RCC_PLL3VCIRANGE_1: Range frequency is between 2 and 4 MHz
  *            @arg RCC_PLL3VCIRANGE_2: Range frequency is between 4 and 8 MHz
  *            @arg RCC_PLL3VCIRANGE_3: Range frequency is between 8 and 16 MHz
  * @retval None
  */
#define __HAL_RCC_PLL3_VCIRANGE(__RCC_PLL3VCIRange__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL3RGE, (__RCC_PLL3VCIRange__))


/** @brief  Macro to select  the PLL3  reference frequency range.
  * @param  __RCC_PLL3VCORange__ specifies the PLL1 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL3VCOWIDE: Range frequency is between 192 and 836 MHz  or between 128 to 560 MHz(*)
  *            @arg RCC_PLL3VCOMEDIUM: Range frequency is between 150 and 420 MHz
  *
  * (*) : For stm32h7a3xx and stm32h7b3xx family lines.
  *
  * @retval None
  */
#define __HAL_RCC_PLL3_VCORANGE(__RCC_PLL3VCORange__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL3VCOSEL, (__RCC_PLL3VCORange__))
/**
  * @brief  Macro to Configure the SAI1 clock source.
  * @param  __RCC_SAI1CLKSource__ defines the SAI1 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI1CLKSOURCE_PLL: SAI1 clock = PLL
  *             @arg RCC_SAI1CLKSOURCE_PLL2: SAI1 clock = PLL2
  *             @arg RCC_SAI1CLKSOURCE_PLL3: SAI1 clock = PLL3
  *             @arg RCC_SAI1CLKSOURCE_OSC: SAI1 clock  = OSC
  *             @arg RCC_SAI1CLKSOURCE_PIN: SAI1 clock = External Clock
  * @retval None
  */
#if defined(RCC_D2CCIP1R_SAI1SEL)
#define __HAL_RCC_SAI1_CONFIG(__RCC_SAI1CLKSource__ )\
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_SAI1SEL, (__RCC_SAI1CLKSource__))
#else
#define __HAL_RCC_SAI1_CONFIG(__RCC_SAI1CLKSource__ )\
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SAI1SEL, (__RCC_SAI1CLKSource__))
#endif /* RCC_D2CCIP1R_SAI1SEL */

/** @brief  Macro to get the SAI1 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI1CLKSOURCE_PLL: SAI1 clock = PLL
  *             @arg RCC_SAI1CLKSOURCE_PLL2: SAI1 clock = PLL2
  *             @arg RCC_SAI1CLKSOURCE_PLL3: SAI1 clock = PLL3
  *             @arg RCC_SAI1CLKSOURCE_CLKP: SAI1 clock  = CLKP
  *             @arg RCC_SAI1CLKSOURCE_PIN: SAI1 clock = External Clock
  */
#if defined(RCC_D2CCIP1R_SAI1SEL)
#define __HAL_RCC_GET_SAI1_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_SAI1SEL)))
#else
#define __HAL_RCC_GET_SAI1_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SAI1SEL)))
#endif /* RCC_D2CCIP1R_SAI1SEL */

/**
  * @brief  Macro to Configure the SPDIFRX clock source.
  * @param  __RCC_SPDIFCLKSource__ defines the SPDIFRX clock source. This clock is derived
  *         from system PLL, PLL2, PLL3,  or internal OSC clock
  *          This parameter can be one of the following values:
  *             @arg RCC_SPDIFRXCLKSOURCE_PLL:  SPDIFRX clock = PLL
  *             @arg RCC_SPDIFRXCLKSOURCE_PLL2: SPDIFRX clock = PLL2
  *             @arg RCC_SPDIFRXCLKSOURCE_PLL3: SPDIFRX clock = PLL3
  *             @arg RCC_SPDIFRXCLKSOURCE_HSI:  SPDIFRX clock  = HSI
  * @retval None
  */
#if defined(RCC_D2CCIP1R_SPDIFSEL)
#define __HAL_RCC_SPDIFRX_CONFIG(__RCC_SPDIFCLKSource__ )\
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_SPDIFSEL, (__RCC_SPDIFCLKSource__))
#else
#define __HAL_RCC_SPDIFRX_CONFIG(__RCC_SPDIFCLKSource__ )\
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SPDIFSEL, (__RCC_SPDIFCLKSource__))
#endif /* RCC_D2CCIP1R_SPDIFSEL */

/**
  * @brief  Macro to get the SPDIFRX clock source.
  * @retval None
  */
#if defined(RCC_D2CCIP1R_SPDIFSEL)
#define __HAL_RCC_GET_SPDIFRX_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_SPDIFSEL)))
#else
#define __HAL_RCC_GET_SPDIFRX_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SPDIFSEL)))
#endif /* RCC_D2CCIP1R_SPDIFSEL */

#if defined(SAI3)
/**
  * @brief  Macro to Configure the SAI2/3 clock source.
  * @param  __RCC_SAI23CLKSource__ defines the SAI2/3 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI23CLKSOURCE_PLL: SAI2/3 clock = PLL
  *             @arg RCC_SAI23CLKSOURCE_PLL2: SAI2/3 clock = PLL2
  *             @arg RCC_SAI23CLKSOURCE_PLL3: SAI2/3 clock = PLL3
  *             @arg RCC_SAI23CLKSOURCE_CLKP: SAI2/3 clock  = CLKP
  *             @arg RCC_SAI23CLKSOURCE_PIN: SAI2/3 clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SAI23_CONFIG(__RCC_SAI23CLKSource__ )\
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_SAI23SEL, (__RCC_SAI23CLKSource__))

/** @brief  Macro to get the SAI2/3 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI23CLKSOURCE_PLL: SAI2/3 clock = PLL
  *             @arg RCC_SAI23CLKSOURCE_PLL2: SAI2/3 clock = PLL2
  *             @arg RCC_SAI23CLKSOURCE_PLL3: SAI2/3 clock = PLL3
  *             @arg RCC_SAI23CLKSOURCE_CLKP: SAI2/3 clock  = CLKP
  *             @arg RCC_SAI23CLKSOURCE_PIN: SAI2/3 clock = External Clock
  */
#define __HAL_RCC_GET_SAI23_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_SAI23SEL)))

/**
  * @brief  Macro to Configure the SAI2 clock source.
  * @param  __RCC_SAI2CLKSource__ defines the SAI2 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI2CLKSOURCE_PLL: SAI2 clock = PLL
  *             @arg RCC_SAI2CLKSOURCE_PLL2: SAI2 clock = PLL2
  *             @arg RCC_SAI2CLKSOURCE_PLL3: SAI2 clock = PLL3
  *             @arg RCC_SAI2CLKSOURCE_CLKP: SAI2 clock  = CLKP
  *             @arg RCC_SAI2CLKSOURCE_PIN: SAI2 clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SAI2_CONFIG  __HAL_RCC_SAI23_CONFIG

/** @brief  Macro to get the SAI2 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI2CLKSOURCE_PLL: SAI2 clock = PLL
  *             @arg RCC_SAI2CLKSOURCE_PLL2: SAI2 clock = PLL2
  *             @arg RCC_SAI2CLKSOURCE_PLL3: SAI2 clock = PLL3
  *             @arg RCC_SAI2CLKSOURCE_CLKP: SAI2 clock  = CLKP
  *             @arg RCC_SAI2CLKSOURCE_PIN: SAI2 clock = External Clock
  */
#define __HAL_RCC_GET_SAI2_SOURCE  __HAL_RCC_GET_SAI23_SOURCE

/**
  * @brief  Macro to Configure the SAI3 clock source.
  * @param  __RCC_SAI3CLKSource__ defines the SAI3 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI3CLKSOURCE_PLL: SAI3 clock = PLL
  *             @arg RCC_SAI3CLKSOURCE_PLL2: SAI3 clock = PLL2
  *             @arg RCC_SAI3CLKSOURCE_PLL3: SAI3 clock = PLL3
  *             @arg RCC_SAI3CLKSOURCE_CLKP: SAI3 clock  = CLKP
  *             @arg RCC_SAI3CLKSOURCE_PIN: SAI3 clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SAI3_CONFIG __HAL_RCC_SAI23_CONFIG

/** @brief  Macro to get the SAI3 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI3CLKSOURCE_PLL: SAI3 clock = PLL
  *             @arg RCC_SAI3CLKSOURCE_PLL2: SAI3 clock = PLL2
  *             @arg RCC_SAI3CLKSOURCE_PLL3: SAI3 clock = PLL3
  *             @arg RCC_SAI3CLKSOURCE_CLKP: SAI3 clock  = CLKP
  *             @arg RCC_SAI3CLKSOURCE_PIN: SAI3 clock = External Clock
  */
#define __HAL_RCC_GET_SAI3_SOURCE  __HAL_RCC_GET_SAI23_SOURCE
#endif /* SAI3 */

#if defined(RCC_CDCCIP1R_SAI2ASEL)
/**
  * @brief  Macro to Configure the SAI2A clock source.
  * @param  __RCC_SAI2ACLKSource__ defines the SAI2A clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI2ACLKSOURCE_PLL: SAI2A clock = PLL
  *             @arg RCC_SAI2ACLKSOURCE_PLL2: SAI2A clock = PLL2
  *             @arg RCC_SAI2ACLKSOURCE_PLL3: SAI2A clock = PLL3
  *             @arg RCC_SAI2ACLKSOURCE_CLKP: SAI2A clock  = CLKP
  *             @arg RCC_SAI2ACLKSOURCE_PIN: SAI2A clock = External Clock
  *             @arg RCC_SAI2ACLKSOURCE_SPDIF: SAI2A clock = SPDIF Clock
  * @retval None
  */
#define __HAL_RCC_SAI2A_CONFIG(__RCC_SAI2ACLKSource__ )\
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SAI2ASEL, (__RCC_SAI2ACLKSource__))

/** @brief  Macro to get the SAI2A clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI2CLKSOURCE_PLL: SAI2A clock = PLL
  *             @arg RCC_SAI2CLKSOURCE_PLL2: SAI2A clock = PLL2
  *             @arg RCC_SAI2CLKSOURCE_PLL3: SAI2A clock = PLL3
  *             @arg RCC_SAI2CLKSOURCE_CLKP: SAI2A clock  = CLKP
  *             @arg RCC_SAI2CLKSOURCE_PIN: SAI2A clock = External Clock
  *             @arg RCC_SAI2ACLKSOURCE_SPDIF: SAI2A clock = SPDIF Clock
  */
#define __HAL_RCC_GET_SAI2A_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SAI2ASEL)))
#endif /* defined(RCC_CDCCIP1R_SAI2ASEL) */

#if defined(RCC_CDCCIP1R_SAI2BSEL)
/**
  * @brief  Macro to Configure the SAI2B clock source.
  * @param  __RCC_SAI2BCLKSource__ defines the SAI2B clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI2BCLKSOURCE_PLL: SAI2B clock = PLL
  *             @arg RCC_SAI2BCLKSOURCE_PLL2: SAI2B clock = PLL2
  *             @arg RCC_SAI2BCLKSOURCE_PLL3: SAI2B clock = PLL3
  *             @arg RCC_SAI2BCLKSOURCE_CLKP: SAI2B clock  = CLKP
  *             @arg RCC_SAI2BCLKSOURCE_PIN: SAI2B clock = External Clock
  *             @arg RCC_SAI2BCLKSOURCE_SPDIF: SAI2B clock = SPDIF Clock
  * @retval None
  */
#define __HAL_RCC_SAI2B_CONFIG(__RCC_SAI2BCLKSource__ )\
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SAI2BSEL, (__RCC_SAI2BCLKSource__))

/** @brief  Macro to get the SAI2B clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI2BCLKSOURCE_PLL: SAI2B clock = PLL
  *             @arg RCC_SAI2BCLKSOURCE_PLL2: SAI2B clock = PLL2
  *             @arg RCC_SAI2BCLKSOURCE_PLL3: SAI2B clock = PLL3
  *             @arg RCC_SAI2BCLKSOURCE_CLKP: SAI2B clock  = CLKP
  *             @arg RCC_SAI2BCLKSOURCE_PIN: SAI2B clock = External Clock
  *             @arg RCC_SAI2BCLKSOURCE_SPDIF: SAI2B clock = SPDIF Clock
  */
#define __HAL_RCC_GET_SAI2B_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SAI2BSEL)))
#endif /* defined(RCC_CDCCIP1R_SAI2BSEL) */


#if defined(SAI4_Block_A)
/**
  * @brief  Macro to Configure the SAI4A clock source.
  * @param  __RCC_SAI4ACLKSource__ defines the SAI4A clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI4ACLKSOURCE_PLL: SAI4A clock = PLL
  *             @arg RCC_SAI4ACLKSOURCE_PLL2: SAI4A clock = PLL2
  *             @arg RCC_SAI4ACLKSOURCE_PLL3: SAI4A clock = PLL3
  *             @arg RCC_SAI4ACLKSOURCE_CLKP: SAI4A clock  = CLKP
  *             @arg RCC_SAI4ACLKSOURCE_PIN: SAI4A clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SAI4A_CONFIG(__RCC_SAI4ACLKSource__ )\
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_SAI4ASEL, (__RCC_SAI4ACLKSource__))

/** @brief  Macro to get the SAI4A clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI4ACLKSOURCE_PLL: SAI4B clock = PLL
  *             @arg RCC_SAI4ACLKSOURCE_PLL2: SAI4B clock = PLL2
  *             @arg RCC_SAI4ACLKSOURCE_PLL3: SAI4B clock = PLL3
  *             @arg RCC_SAI4ACLKSOURCE_CLKP: SAI4B clock  = CLKP
  *             @arg RCC_SAI4ACLKSOURCE_PIN: SAI4B clock = External Clock
  */
#define __HAL_RCC_GET_SAI4A_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_SAI4ASEL)))
#endif /* SAI4_Block_A */

#if defined(SAI4_Block_B)
/**
  * @brief  Macro to Configure the SAI4B clock source.
  * @param  __RCC_SAI4BCLKSource__ defines the SAI4B clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SAI4BCLKSOURCE_PLL: SAI4B clock = PLL
  *             @arg RCC_SAI4BCLKSOURCE_PLL2: SAI4B clock = PLL2
  *             @arg RCC_SAI4BCLKSOURCE_PLL3: SAI4B clock = PLL3
  *             @arg RCC_SAI4BCLKSOURCE_CLKP: SAI4B clock  = CLKP
  *             @arg RCC_SAI4BCLKSOURCE_PIN: SAI4B clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SAI4B_CONFIG(__RCC_SAI4BCLKSource__ )\
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_SAI4BSEL, (__RCC_SAI4BCLKSource__))

/** @brief  Macro to get the SAI4B clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SAI4BCLKSOURCE_PLL: SAI4B clock = PLL
  *             @arg RCC_SAI4BCLKSOURCE_PLL2: SAI4B clock = PLL2
  *             @arg RCC_SAI4BCLKSOURCE_PLL3: SAI4B clock = PLL3
  *             @arg RCC_SAI4BCLKSOURCE_CLKP: SAI4B clock  = CLKP
  *             @arg RCC_SAI4BCLKSOURCE_PIN: SAI4B clock = External Clock
  */
#define __HAL_RCC_GET_SAI4B_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_SAI4BSEL)))
#endif /* SAI4_Block_B */

/** @brief macro to configure the I2C1/2/3 clock (I2C123CLK).
  *
  * @param  __I2C123CLKSource__ specifies the I2C1/2/3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C123CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C1/2/3 clock
  *            @arg RCC_I2C123CLKSOURCE_PLL3: PLL3 selected as I2C1/2/3 clock
  *            @arg RCC_I2C123CLKSOURCE_HSI: HSI selected as I2C1/2/3 clock
  *            @arg RCC_I2C123CLKSOURCE_CSI: CSI selected as I2C1/2/3 clock
  */
#if defined(RCC_D2CCIP2R_I2C123SEL)
#define __HAL_RCC_I2C123_CONFIG(__I2C123CLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_I2C123SEL, (uint32_t)(__I2C123CLKSource__))
#else
#define __HAL_RCC_I2C123_CONFIG(__I2C123CLKSource__) \
                  MODIFY_REG(RCC->CDCCIP2R, RCC_CDCCIP2R_I2C123SEL, (uint32_t)(__I2C123CLKSource__))
#endif /* RCC_D2CCIP2R_I2C123SEL */

/** @brief  macro to get the I2C1/2/3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C123CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C1/2/3 clock
  *            @arg RCC_I2C123CLKSOURCE_PLL3: PLL3 selected as I2C1/2/3 clock
  *            @arg RCC_I2C123CLKSOURCE_HSI: HSI selected as I2C1/2/3 clock
  *            @arg RCC_I2C123CLKSOURCE_CSI: CSI selected as I2C1/2/3 clock
  */
#if defined(RCC_D2CCIP2R_I2C123SEL)
#define __HAL_RCC_GET_I2C123_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_I2C123SEL)))
#else
#define __HAL_RCC_GET_I2C123_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_I2C123SEL)))
#endif /* RCC_D2CCIP2R_I2C123SEL */

/** @brief macro to configure the I2C1 clock (I2C1CLK).
  *
  * @param  __I2C1CLKSource__ specifies the I2C1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C1CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_PLL3: PLL3 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_HSI: HSI selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_CSI: CSI selected as I2C1 clock
  */
#define __HAL_RCC_I2C1_CONFIG  __HAL_RCC_I2C123_CONFIG

/** @brief  macro to get the I2C1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C1CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_PLL3: PLL3 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_HSI: HSI selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_CSI: CSI selected as I2C1 clock
  */
#define __HAL_RCC_GET_I2C1_SOURCE __HAL_RCC_GET_I2C123_SOURCE

/** @brief macro to configure the I2C2 clock (I2C2CLK).
  *
  * @param  __I2C2CLKSource__ specifies the I2C2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C2CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_PLL3: PLL3 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_HSI: HSI selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_CSI: CSI selected as I2C2 clock
  */
#define __HAL_RCC_I2C2_CONFIG __HAL_RCC_I2C123_CONFIG

/** @brief  macro to get the I2C2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C2CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_PLL3: PLL3 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_HSI: HSI selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_CSI: CSI selected as I2C2 clock
  */
#define __HAL_RCC_GET_I2C2_SOURCE __HAL_RCC_GET_I2C123_SOURCE

/** @brief macro to configure the I2C3 clock (I2C3CLK).
  *
  * @param  __I2C3CLKSource__ specifies the I2C3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C3CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_PLL3: PLL3 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_HSI: HSI selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_CSI: CSI selected as I2C3 clock
  */
#define __HAL_RCC_I2C3_CONFIG __HAL_RCC_I2C123_CONFIG

/** @brief  macro to get the I2C3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C3CLKSOURCE_D2PCLK1: D2PCLK1 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_PLL3: PLL3 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_HSI: HSI selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_CSI: CSI selected as I2C3 clock
  */
#define __HAL_RCC_GET_I2C3_SOURCE __HAL_RCC_GET_I2C123_SOURCE

/** @brief macro to configure the I2C4 clock (I2C4CLK).
  *
  * @param  __I2C4CLKSource__ specifies the I2C4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C4CLKSOURCE_D3PCLK1: D3PCLK1 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_PLL3: PLL3 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_HSI: HSI selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_CSI: CSI selected as I2C4 clock
  */
#if defined(RCC_D3CCIPR_I2C4SEL)
#define __HAL_RCC_I2C4_CONFIG(__I2C4CLKSource__) \
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_I2C4SEL, (uint32_t)(__I2C4CLKSource__))
#else
#define __HAL_RCC_I2C4_CONFIG(__I2C4CLKSource__) \
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_I2C4SEL, (uint32_t)(__I2C4CLKSource__))
#endif /* RCC_D3CCIPR_I2C4SEL */

/** @brief  macro to get the I2C4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C4CLKSOURCE_D3PCLK1: D3PCLK1 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_PLL3: PLL3 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_HSI: HSI selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_CSI: CSI selected as I2C4 clock
  */
#define __HAL_RCC_GET_I2C4_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_I2C4SEL)))

/** @brief macro to configure the USART1/6/9* /10* clock (USART16CLK).
  *
  * @param  __USART16CLKSource__ specifies the USART1/6/9* /10* clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART16CLKSOURCE_D2PCLK2: APB2 Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_PLL2: PLL2_Q Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_PLL3: PLL3_Q Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_HSI: HSI selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_CSI: CSI Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_LSE: LSE selected as USART1/6/9* /10* clock
  *
  * (*) : Available on some STM32H7 lines only.
  */
#if defined(RCC_D2CCIP2R_USART16SEL)
#define __HAL_RCC_USART16_CONFIG(__USART16CLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_USART16SEL, (uint32_t)(__USART16CLKSource__))
#else
#define __HAL_RCC_USART16910_CONFIG(__USART16910CLKSource__) \
                  MODIFY_REG(RCC->CDCCIP2R, RCC_CDCCIP2R_USART16910SEL, (uint32_t)(__USART16910CLKSource__))
/* alias */
#define __HAL_RCC_USART16_CONFIG  __HAL_RCC_USART16910_CONFIG
#endif /* RCC_D2CCIP2R_USART16SEL */

/** @brief  macro to get the USART1/6/9* /10* clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART16CLKSOURCE_D2PCLK2: APB2 Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_PLL2: PLL2_Q Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_PLL3: PLL3_Q Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_HSI: HSI selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_CSI: CSI Clock selected as USART1/6/9* /10* clock
  *            @arg RCC_USART16CLKSOURCE_LSE: LSE selected as USART1/6/9* /10* clock
  *
  * (*) : Available on some STM32H7 lines only.
  */
#if defined(RCC_D2CCIP2R_USART16SEL)
#define __HAL_RCC_GET_USART16_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_USART16SEL)))
#else
#define __HAL_RCC_GET_USART16910_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_USART16910SEL)))
/* alias*/
#define  __HAL_RCC_GET_USART16_SOURCE  __HAL_RCC_GET_USART16910_SOURCE
#endif /* RCC_D2CCIP2R_USART16SEL */

/** @brief macro to configure the USART234578 clock (USART234578CLK).
  *
  * @param  __USART234578CLKSource__ specifies the USART2/3/4/5/7/8 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART234578CLKSOURCE_D2PCLK1: APB1 Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_PLL2: PLL2_Q Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_PLL3: PLL3_Q Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_HSI: HSI selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_CSI: CSI Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_LSE: LSE selected as USART2/3/4/5/7/8 clock
  */
#if defined(RCC_D2CCIP2R_USART28SEL)
#define __HAL_RCC_USART234578_CONFIG(__USART234578CLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_USART28SEL, (uint32_t)(__USART234578CLKSource__))
#else
#define __HAL_RCC_USART234578_CONFIG(__USART234578CLKSource__) \
                  MODIFY_REG(RCC->CDCCIP2R, RCC_CDCCIP2R_USART234578SEL, (uint32_t)(__USART234578CLKSource__))
#endif /* RCC_D2CCIP2R_USART28SEL */

/** @brief  macro to get the USART2/3/4/5/7/8 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART234578CLKSOURCE_D2PCLK1: APB1 Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_PLL2: PLL2_Q Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_PLL3: PLL3_Q Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_HSI: HSI selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_CSI: CSI Clock selected as USART2/3/4/5/7/8 clock
  *            @arg RCC_USART234578CLKSOURCE_LSE: LSE selected as USART2/3/4/5/7/8 clock
  */
#if defined(RCC_D2CCIP2R_USART28SEL)
#define __HAL_RCC_GET_USART234578_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_USART28SEL)))
#else
#define __HAL_RCC_GET_USART234578_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_USART234578SEL)))
#endif /* RCC_D2CCIP2R_USART28SEL */

/** @brief macro to configure the USART1 clock (USART1CLK).
  *
  * @param  __USART1CLKSource__ specifies the USART1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART1CLKSOURCE_D2PCLK2: APB2 Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_PLL2: PLL2_Q Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_PLL3: PLL3_Q Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_HSI: HSI selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_CSI: CSI Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_LSE: LSE selected as USART1 clock
  */
#define __HAL_RCC_USART1_CONFIG  __HAL_RCC_USART16_CONFIG

/** @brief  macro to get the USART1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART1CLKSOURCE_D2PCLK2: APB2 Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_PLL2: PLL2_Q Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_PLL3: PLL3_Q Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_HSI: HSI selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_CSI: CSI Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_LSE: LSE selected as USART1 clock
  */
#define __HAL_RCC_GET_USART1_SOURCE  __HAL_RCC_GET_USART16_SOURCE

/** @brief macro to configure the USART2 clock (USART2CLK).
  *
  * @param  __USART2CLKSource__ specifies the USART2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_D2PCLK1: APB1 Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_PLL2: PLL2_Q Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_PLL3: PLL3_Q Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_HSI: HSI selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_CSI: CSI Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_LSE: LSE selected as USART2 clock
  */
#define __HAL_RCC_USART2_CONFIG  __HAL_RCC_USART234578_CONFIG

/** @brief  macro to get the USART2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_D2PCLK1: APB1 Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_PLL2: PLL2_Q Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_PLL3: PLL3_Q Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_HSI: HSI selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_CSI: CSI Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_LSE: LSE selected as USART2 clock
  */
#define __HAL_RCC_GET_USART2_SOURCE __HAL_RCC_GET_USART234578_SOURCE

/** @brief macro to configure the USART3 clock (USART3CLK).
  *
  * @param  __USART3CLKSource__ specifies the USART3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART3CLKSOURCE_D2PCLK1: APB1 Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_PLL2: PLL2_Q Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_PLL3: PLL3_Q Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_HSI: HSI selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_CSI: CSI Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_LSE: LSE selected as USART3 clock
  */
#define __HAL_RCC_USART3_CONFIG  __HAL_RCC_USART234578_CONFIG

/** @brief  macro to get the USART3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_D2PCLK1: APB1 Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_PLL2: PLL2_Q Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_PLL3: PLL3_Q Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_HSI: HSI selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_CSI: CSI Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_LSE: LSE selected as USART3 clock
  */
#define __HAL_RCC_GET_USART3_SOURCE  __HAL_RCC_GET_USART234578_SOURCE

/** @brief macro to configure the UART4 clock (UART4CLK).
  *
  * @param  __UART4CLKSource__ specifies the UART4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART4CLKSOURCE_D2PCLK1: APB1 Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_PLL2: PLL2_Q Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_PLL3: PLL3_Q Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_HSI: HSI selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_CSI: CSI Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_LSE: LSE selected as UART4 clock
  */
#define __HAL_RCC_UART4_CONFIG  __HAL_RCC_USART234578_CONFIG

/** @brief  macro to get the UART4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART4CLKSOURCE_D2PCLK1: APB1 Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_PLL2: PLL2_Q Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_PLL3: PLL3_Q Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_HSI: HSI selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_CSI: CSI Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_LSE: LSE selected as UART4 clock
  */
#define __HAL_RCC_GET_UART4_SOURCE  __HAL_RCC_GET_USART234578_SOURCE

/** @brief macro to configure the UART5 clock (UART5CLK).
  *
  * @param  __UART5CLKSource__ specifies the UART5 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART5CLKSOURCE_D2PCLK1: APB1 Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_PLL2: PLL2_Q Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_PLL3: PLL3_Q Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_HSI: HSI selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_CSI: CSI Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_LSE: LSE selected as UART5 clock
  */
#define __HAL_RCC_UART5_CONFIG  __HAL_RCC_USART234578_CONFIG

/** @brief  macro to get the UART5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART5CLKSOURCE_D2PCLK1: APB1 Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_PLL2: PLL2_Q Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_PLL3: PLL3_Q Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_HSI: HSI selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_CSI: CSI Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_LSE: LSE selected as UART5 clock
  */
#define __HAL_RCC_GET_UART5_SOURCE  __HAL_RCC_GET_USART234578_SOURCE

/** @brief macro to configure the USART6 clock (USART6CLK).
  *
  * @param  __USART6CLKSource__ specifies the USART6 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART6CLKSOURCE_D2PCLK2: APB2 Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_PLL2: PLL2_Q Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_PLL3: PLL3_Q Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_HSI: HSI selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_CSI: CSI Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_LSE: LSE selected as USART6 clock
  */
#define __HAL_RCC_USART6_CONFIG  __HAL_RCC_USART16_CONFIG

/** @brief  macro to get the USART6 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART6CLKSOURCE_D2PCLK2: APB2 Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_PLL2: PLL2_Q Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_PLL3: PLL3_Q Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_HSI: HSI selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_CSI: CSI Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_LSE: LSE selected as USART6 clock
  */
#define __HAL_RCC_GET_USART6_SOURCE  __HAL_RCC_GET_USART16_SOURCE

/** @brief macro to configure the UART5 clock (UART7CLK).
  *
  * @param  __UART7CLKSource__ specifies the UART7 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART7CLKSOURCE_D2PCLK1: APB1 Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_PLL2: PLL2_Q Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_PLL3: PLL3_Q Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_HSI: HSI selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_CSI: CSI Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_LSE: LSE selected as UART7 clock
  */
#define __HAL_RCC_UART7_CONFIG  __HAL_RCC_USART234578_CONFIG

/** @brief  macro to get the UART7 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART7CLKSOURCE_D2PCLK1: APB1 Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_PLL2: PLL2_Q Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_PLL3: PLL3_Q Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_HSI: HSI selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_CSI: CSI Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_LSE: LSE selected as UART7 clock
  */
#define __HAL_RCC_GET_UART7_SOURCE  __HAL_RCC_GET_USART234578_SOURCE

/** @brief macro to configure the UART8 clock (UART8CLK).
  *
  * @param  __UART8CLKSource__ specifies the UART8 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART8CLKSOURCE_D2PCLK1: APB1 Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_PLL2: PLL2_Q Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_PLL3: PLL3_Q Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_HSI: HSI selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_CSI: CSI Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_LSE: LSE selected as UART8 clock
  */
#define __HAL_RCC_UART8_CONFIG  __HAL_RCC_USART234578_CONFIG

/** @brief  macro to get the UART8 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART8CLKSOURCE_D2PCLK1: APB1 Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_PLL2: PLL2_Q Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_PLL3: PLL3_Q Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_HSI: HSI selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_CSI: CSI Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_LSE: LSE selected as UART8 clock
  */
#define __HAL_RCC_GET_UART8_SOURCE  __HAL_RCC_GET_USART234578_SOURCE

#if defined(UART9)
/** @brief macro to configure the UART9 clock (UART9CLK).
  *
  * @param  __UART8CLKSource__ specifies the UART8 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART9CLKSOURCE_D2PCLK1: APB1 Clock selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_PLL2: PLL2_Q Clock selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_PLL3: PLL3_Q Clock selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_HSI: HSI selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_CSI: CSI Clock selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_LSE: LSE selected as UART9 clock
  */
#define __HAL_RCC_UART9_CONFIG  __HAL_RCC_USART16_CONFIG

/** @brief  macro to get the UART9 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART9CLKSOURCE_D2PCLK1: APB1 Clock selected as UART99 clock
  *            @arg RCC_UART9CLKSOURCE_PLL2: PLL2_Q Clock selected as UART99 clock
  *            @arg RCC_UART9CLKSOURCE_PLL3: PLL3_Q Clock selected as UART99 clock
  *            @arg RCC_UART9CLKSOURCE_HSI: HSI selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_CSI: CSI Clock selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_LSE: LSE selected as UART9 clock
  */
#define __HAL_RCC_GET_UART9_SOURCE  __HAL_RCC_GET_USART16_SOURCE
#endif /* UART9 */

#if defined(USART10)
/** @brief macro to configure the USART10 clock (USART10CLK).
  *
  * @param  __UART8CLKSource__ specifies the UART8 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART10CLKSOURCE_D2PCLK1: APB1 Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_PLL2: PLL2_Q Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_PLL3: PLL3_Q Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_HSI: HSI selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_CSI: CSI Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_LSE: LSE selected as USART10 clock
  */
#define __HAL_RCC_USART10_CONFIG  __HAL_RCC_USART16_CONFIG

/** @brief  macro to get the USART10 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART10CLKSOURCE_D2PCLK1: APB1 Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_PLL2: PLL2_Q Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_PLL3: PLL3_Q Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_HSI: HSI selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_CSI: CSI Clock selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_LSE: LSE selected as USART10 clock
  */
#define __HAL_RCC_GET_USART10_SOURCE  __HAL_RCC_GET_USART16_SOURCE
#endif /* USART10 */

/** @brief macro to configure the LPUART1 clock (LPUART1CLK).
  *
  * @param  __LPUART1CLKSource__ specifies the LPUART1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPUART1CLKSOURCE_D3PCLK1: APB4 Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_PLL2: PLL2_Q Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_PLL3: PLL3_Q Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_HSI: HSI selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_CSI: CSI Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_LSE: LSE selected as LPUART1 clock
  */
#if defined (RCC_D3CCIPR_LPUART1SEL)
#define __HAL_RCC_LPUART1_CONFIG(__LPUART1CLKSource__) \
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_LPUART1SEL, (uint32_t)(__LPUART1CLKSource__))
#else
#define __HAL_RCC_LPUART1_CONFIG(__LPUART1CLKSource__) \
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_LPUART1SEL, (uint32_t)(__LPUART1CLKSource__))
#endif /* RCC_D3CCIPR_LPUART1SEL */

/** @brief  macro to get the LPUART1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPUART1CLKSOURCE_D3PCLK1: APB4 Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_PLL2: PLL2_Q Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_PLL3: PLL3_Q Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_HSI: HSI selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_CSI: CSI Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_LSE: LSE selected as LPUART1 clock
  */
#if defined (RCC_D3CCIPR_LPUART1SEL)
#define __HAL_RCC_GET_LPUART1_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_LPUART1SEL)))
#else
#define __HAL_RCC_GET_LPUART1_SOURCE() ((uint32_t)(READ_BIT(RCC->SRDCCIPR, RCC_SRDCCIPR_LPUART1SEL)))
#endif /* RCC_D3CCIPR_LPUART1SEL */

/** @brief  macro to configure the LPTIM1 clock source.
  *
  * @param  __LPTIM1CLKSource__ specifies the LPTIM1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM1CLKSOURCE_D2PCLK1: APB1 Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSE: LSE selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSI: LSI Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_CLKP: CLKP selected as LPTIM1 clock
  */
#if defined(RCC_D2CCIP2R_LPTIM1SEL)
#define __HAL_RCC_LPTIM1_CONFIG(__LPTIM1CLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_LPTIM1SEL, (uint32_t)(__LPTIM1CLKSource__))
#else
#define __HAL_RCC_LPTIM1_CONFIG(__LPTIM1CLKSource__) \
                  MODIFY_REG(RCC->CDCCIP2R, RCC_CDCCIP2R_LPTIM1SEL, (uint32_t)(__LPTIM1CLKSource__))
#endif /* RCC_D2CCIP2R_LPTIM1SEL */

/** @brief  macro to get the LPTIM1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM1CLKSOURCE_D2PCLK1: APB1 Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSE: LSE selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSI: LSI Clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_CLKP: CLKP selected as LPTIM1 clock
  */
#if defined(RCC_D2CCIP2R_LPTIM1SEL)
#define __HAL_RCC_GET_LPTIM1_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_LPTIM1SEL)))
#else
#define __HAL_RCC_GET_LPTIM1_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_LPTIM1SEL)))
#endif /* RCC_D2CCIP2R_LPTIM1SEL */

/** @brief  macro to configure the LPTIM2 clock source.
  *
  * @param  __LPTIM2CLKSource__ specifies the LPTIM2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM2CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSE: LSE selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSI: LSI Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_CLKP: CLKP selected as LPTIM2 clock
  */
#if defined(RCC_D3CCIPR_LPTIM2SEL)
#define __HAL_RCC_LPTIM2_CONFIG(__LPTIM2CLKSource__) \
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_LPTIM2SEL, (uint32_t)(__LPTIM2CLKSource__))
#else
#define __HAL_RCC_LPTIM2_CONFIG(__LPTIM2CLKSource__) \
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_LPTIM2SEL, (uint32_t)(__LPTIM2CLKSource__))
#endif /* RCC_D3CCIPR_LPTIM2SEL */

/** @brief  macro to get the LPTIM2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM2CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSE: LSE selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSI: LSI Clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_CLKP: CLKP selected as LPTIM2 clock
  */
#if defined(RCC_D3CCIPR_LPTIM2SEL)
#define __HAL_RCC_GET_LPTIM2_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_LPTIM2SEL)))
#else
#define __HAL_RCC_GET_LPTIM2_SOURCE() ((uint32_t)(READ_BIT(RCC->SRDCCIPR, RCC_SRDCCIPR_LPTIM2SEL)))
#endif /* RCC_D3CCIPR_LPTIM2SEL */

/** @brief  macro to configure the LPTIM3/4/5 clock source.
  *
  * @param  __LPTIM345CLKSource__ specifies the LPTIM3/4/5 clock source.
  *            @arg RCC_LPTIM345CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_LSE: LSE selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_LSI: LSI Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_CLKP: CLKP selected as LPTIM3/4/5 clock
  */
#if defined(RCC_D3CCIPR_LPTIM345SEL)
#define __HAL_RCC_LPTIM345_CONFIG(__LPTIM345CLKSource__) \
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_LPTIM345SEL, (uint32_t)(__LPTIM345CLKSource__))
#else
#define __HAL_RCC_LPTIM345_CONFIG(__LPTIM3CLKSource__) \
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_LPTIM3SEL, (uint32_t)(__LPTIM3CLKSource__))
#endif /* RCC_D3CCIPR_LPTIM345SEL */

/** @brief  macro to get the LPTIM3/4/5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM345CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_LSE: LSE selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_LSI: LSI Clock selected as LPTIM3/4/5 clock
  *            @arg RCC_LPTIM345CLKSOURCE_CLKP: CLKP selected as LPTIM3/4/5 clock
  */
#if defined(RCC_D3CCIPR_LPTIM345SEL)
#define __HAL_RCC_GET_LPTIM345_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_LPTIM345SEL)))
#else
#define __HAL_RCC_GET_LPTIM345_SOURCE() ((uint32_t)(READ_BIT(RCC->SRDCCIPR, RCC_SRDCCIPR_LPTIM3SEL)))
#endif /* RCC_D3CCIPR_LPTIM345SEL */

/** @brief  macro to configure the LPTIM3 clock source.
  *
  * @param  __LPTIM3CLKSource__ specifies the LPTIM3 clock source.
  *            @arg RCC_LPTIM3CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSE: LSE selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSI: LSI Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_CLKP: CLKP selected as LPTIM3 clock
  */
#define __HAL_RCC_LPTIM3_CONFIG  __HAL_RCC_LPTIM345_CONFIG

/** @brief  macro to get the LPTIM3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM3CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSE: LSE selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSI: LSI Clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_CLKP: CLKP selected as LPTIM3 clock
  */
#define __HAL_RCC_GET_LPTIM3_SOURCE  __HAL_RCC_GET_LPTIM345_SOURCE

#if defined(LPTIM4)
/** @brief  macro to configure the LPTIM4 clock source.
  *
  * @param  __LPTIM4CLKSource__ specifies the LPTIM4 clock source.
  *            @arg RCC_LPTIM4CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSE: LSE selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSI: LSI Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_CLKP: CLKP selected as LPTIM4 clock
  */
#define __HAL_RCC_LPTIM4_CONFIG  __HAL_RCC_LPTIM345_CONFIG


/** @brief  macro to get the LPTIM4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM4CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSE: LSE selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSI: LSI Clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_CLKP: CLKP selected as LPTIM4 clock
  */
#define __HAL_RCC_GET_LPTIM4_SOURCE  __HAL_RCC_GET_LPTIM345_SOURCE
#endif /* LPTIM4 */

#if defined(LPTIM5)
/** @brief  macro to configure the LPTIM5 clock source.
  *
  * @param  __LPTIM5CLKSource__ specifies the LPTIM5 clock source.
  *            @arg RCC_LPTIM5CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSE: LSE selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSI: LSI Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_CLKP: CLKP selected as LPTIM5 clock
  */
#define __HAL_RCC_LPTIM5_CONFIG  __HAL_RCC_LPTIM345_CONFIG


/** @brief  macro to get the LPTIM5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM5CLKSOURCE_D3PCLK1: APB4 Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_PLL2: PLL2_P Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_PLL3: PLL3_R Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSE: LSE selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSI: LSI Clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_CLKP: CLKP selected as LPTIM5 clock
  */
#define __HAL_RCC_GET_LPTIM5_SOURCE  __HAL_RCC_GET_LPTIM345_SOURCE
#endif /* LPTIM5 */

#if defined(QUADSPI)
/** @brief  macro to configure the QSPI clock source.
  *
  * @param  __QSPICLKSource__ specifies the QSPI clock source.
  *            @arg RCC_RCC_QSPICLKSOURCE_D1HCLK: Domain1 HCLK Clock selected as QSPI clock
  *            @arg RCC_RCC_QSPICLKSOURCE_PLL   : PLL1_Q Clock selected as QSPI clock
  *            @arg RCC_RCC_QSPICLKSOURCE_PLL2  : PLL2_R Clock selected as QSPI clock
  *            @arg RCC_RCC_QSPICLKSOURCE_CLKP    CLKP selected as QSPI clock
  */
#define __HAL_RCC_QSPI_CONFIG(__QSPICLKSource__) \
                  MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_QSPISEL, (uint32_t)(__QSPICLKSource__))


/** @brief  macro to get the QSPI clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_RCC_QSPICLKSOURCE_D1HCLK: Domain1 HCLK Clock selected as QSPI clock
  *            @arg RCC_RCC_QSPICLKSOURCE_PLL   : PLL1_Q Clock selected as QSPI clock
  *            @arg RCC_RCC_QSPICLKSOURCE_PLL2  : PLL2_R Clock selected as QSPI clock
  *            @arg RCC_RCC_QSPICLKSOURCE_CLKP    CLKP selected as QSPI clock
  */
#define __HAL_RCC_GET_QSPI_SOURCE() ((uint32_t)(READ_BIT(RCC->D1CCIPR, RCC_D1CCIPR_QSPISEL)))
#endif /* QUADSPI */

#if defined(OCTOSPI1) || defined(OCTOSPI2)
/** @brief  macro to configure the OSPI clock source.
  *
  * @param  __OSPICLKSource__ specifies the OSPI clock source.
  *            @arg RCC_RCC_OSPICLKSOURCE_CDHCLK: Domain1 HCLK Clock selected as OSPI clock
  *            @arg RCC_RCC_OSPICLKSOURCE_PLL   : PLL1_Q Clock selected as OSPI clock
  *            @arg RCC_RCC_OSPICLKSOURCE_PLL2  : PLL2_R Clock selected as OSPI clock
  *            @arg RCC_RCC_OSPICLKSOURCE_CLKP    CLKP selected as OSPI clock
  */
#define __HAL_RCC_OSPI_CONFIG(__OSPICLKSource__) \
                  MODIFY_REG(RCC->CDCCIPR, RCC_CDCCIPR_OCTOSPISEL, (uint32_t)(__OSPICLKSource__))


/** @brief  macro to get the OSPI clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_RCC_OSPICLKSOURCE_D1HCLK: Domain1 HCLK Clock selected as OSPI clock
  *            @arg RCC_RCC_OSPICLKSOURCE_PLL   : PLL1_Q Clock selected as OSPI clock
  *            @arg RCC_RCC_OSPICLKSOURCE_PLL2  : PLL2_R Clock selected as OSPI clock
  *            @arg RCC_RCC_OSPICLKSOURCE_CLKP    CLKP selected as OSPI clock
  */
#define __HAL_RCC_GET_OSPI_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIPR, RCC_CDCCIPR_OCTOSPISEL)))
#endif /* defined(OCTOSPI1) || defined(OCTOSPI2) */


#if defined(DSI)
/** @brief  macro to configure the DSI clock source.
  *
  * @param  __DSICLKSource__ specifies the DSI clock source.
  *            @arg RCC_RCC_DSICLKSOURCE_PHY:DSI clock from PHY is selected as DSI byte lane clock
  *            @arg RCC_RCC_DSICLKSOURCE_PLL2   : PLL2_Q Clock clock is selected as DSI byte lane clock
  */
#define __HAL_RCC_DSI_CONFIG(__DSICLKSource__) \
                  MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_DSISEL, (uint32_t)(__DSICLKSource__))


/** @brief  macro to get the DSI clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_RCC_DSICLKSOURCE_PHY: DSI clock from PHY is selected as DSI byte lane clock
  *            @arg RCC_RCC_DSICLKSOURCE_PLL2: PLL2_Q Clock clock is selected as DSI byte lane clock
  */
#define __HAL_RCC_GET_DSI_SOURCE() ((uint32_t)(READ_BIT(RCC->D1CCIPR, RCC_D1CCIPR_DSISEL)))
#endif /*DSI*/

/** @brief  macro to configure the FMC clock source.
  *
  * @param  __FMCCLKSource__ specifies the FMC clock source.
  *            @arg RCC_RCC_FMCCLKSOURCE_D1HCLK: Domain1 HCLK Clock selected as FMC clock
  *            @arg RCC_RCC_FMCCLKSOURCE_PLL   : PLL1_Q Clock selected as FMC clock
  *            @arg RCC_RCC_FMCCLKSOURCE_PLL2  : PLL2_R Clock selected as FMC clock
  *            @arg RCC_RCC_FMCCLKSOURCE_CLKP    CLKP selected as FMC clock
  */
#if defined(RCC_D1CCIPR_FMCSEL)
#define __HAL_RCC_FMC_CONFIG(__FMCCLKSource__) \
                  MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_FMCSEL, (uint32_t)(__FMCCLKSource__))
#else
#define __HAL_RCC_FMC_CONFIG(__FMCCLKSource__) \
                  MODIFY_REG(RCC->CDCCIPR, RCC_CDCCIPR_FMCSEL, (uint32_t)(__FMCCLKSource__))
#endif /* RCC_D1CCIPR_FMCSEL */

/** @brief  macro to get the FMC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_RCC_FMCCLKSOURCE_D1HCLK: Domain1 HCLK Clock selected as FMC clock
  *            @arg RCC_RCC_FMCCLKSOURCE_PLL   : PLL1_Q Clock selected as FMC clock
  *            @arg RCC_RCC_FMCCLKSOURCE_PLL2  : PLL2_R Clock selected as FMC clock
  *            @arg RCC_RCC_FMCCLKSOURCE_CLKP    CLKP selected as FMC clock
  */
#if defined(RCC_D1CCIPR_FMCSEL)
#define __HAL_RCC_GET_FMC_SOURCE() ((uint32_t)(READ_BIT(RCC->D1CCIPR, RCC_D1CCIPR_FMCSEL)))
#else
#define __HAL_RCC_GET_FMC_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIPR, RCC_CDCCIPR_FMCSEL)))
#endif /* RCC_D1CCIPR_FMCSEL */

/** @brief  Macro to configure the USB clock (USBCLK).
  * @param  __USBCLKSource__ specifies the USB clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_USBCLKSOURCE_PLL:   PLL1Q selected as USB clock
  *            @arg RCC_USBCLKSOURCE_PLL3:  PLL3Q Clock selected as USB clock
  *            @arg RCC_USBCLKSOURCE_HSI48: RC48 MHZ Clock selected as USB clock
  */
#if defined(RCC_D2CCIP2R_USBSEL)
#define __HAL_RCC_USB_CONFIG(__USBCLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_USBSEL, (uint32_t)(__USBCLKSource__))
#else
#define __HAL_RCC_USB_CONFIG(__USBCLKSource__) \
                  MODIFY_REG(RCC->CDCCIP2R, RCC_CDCCIP2R_USBSEL, (uint32_t)(__USBCLKSource__))
#endif /* RCC_D2CCIP2R_USBSEL */

/** @brief  Macro to get the USB clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USBCLKSOURCE_PLL:   PLL1Q selected as USB clock
  *            @arg RCC_USBCLKSOURCE_PLL3:  PLL3Q Clock selected as USB clock
  *            @arg RCC_USBCLKSOURCE_HSI48: RC48 MHZ Clock selected as USB clock
  */
#if defined(RCC_D2CCIP2R_USBSEL)
#define __HAL_RCC_GET_USB_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_USBSEL)))
#else
#define __HAL_RCC_GET_USB_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_USBSEL)))
#endif /* RCC_D2CCIP2R_USBSEL */

/** @brief  Macro to configure the ADC clock
  * @param  __ADCCLKSource__ specifies the ADC digital interface clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_ADCCLKSOURCE_PLL2: PLL2_P Clock selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_PLL3: PLL3_R Clock selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_CLKP: CLKP Clock selected as ADC clock
  */
#if defined(RCC_D3CCIPR_ADCSEL)
#define __HAL_RCC_ADC_CONFIG(__ADCCLKSource__) \
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_ADCSEL, (uint32_t)(__ADCCLKSource__))
#else
#define __HAL_RCC_ADC_CONFIG(__ADCCLKSource__) \
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_ADCSEL, (uint32_t)(__ADCCLKSource__))
#endif /* RCC_D3CCIPR_ADCSEL */

/** @brief  Macro to get the ADC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ADCCLKSOURCE_PLL2: PLL2_P Clock selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_PLL3: PLL3_R Clock selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_CLKP: CLKP Clock selected as ADC clock
  */
#if defined(RCC_D3CCIPR_ADCSEL)
#define __HAL_RCC_GET_ADC_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_ADCSEL)))
#else
#define __HAL_RCC_GET_ADC_SOURCE() ((uint32_t)(READ_BIT(RCC->SRDCCIPR, RCC_SRDCCIPR_ADCSEL)))
#endif /* RCC_D3CCIPR_ADCSEL */

 /** @brief  Macro to configure the SWPMI1 clock
  * @param  __SWPMI1CLKSource__ specifies the SWPMI1  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SWPMI1CLKSOURCE_D2PCLK1:  D2PCLK1 Clock selected as SWPMI1 clock
  *            @arg RCC_SWPMI1CLKSOURCE_HSI: HSI Clock selected as SWPMI1 clock
  */
#if defined(RCC_D2CCIP1R_SWPSEL)
#define __HAL_RCC_SWPMI1_CONFIG(__SWPMI1CLKSource__) \
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_SWPSEL, (uint32_t)(__SWPMI1CLKSource__))
#else
#define __HAL_RCC_SWPMI1_CONFIG(__SWPMI1CLKSource__) \
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SWPSEL, (uint32_t)(__SWPMI1CLKSource__))
#endif /* RCC_D2CCIP1R_SWPSEL */

/** @brief  Macro to get the SWPMI1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SWPMI1CLKSOURCE_D2PCLK1:  D2PCLK1 Clock selected as SWPMI1 clock
  *            @arg RCC_SWPMI1CLKSOURCE_HSI: HSI Clock selected as SWPMI1 clock
  */
#if defined(RCC_D2CCIP1R_SWPSEL)
#define __HAL_RCC_GET_SWPMI1_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_SWPSEL)))
#else
#define __HAL_RCC_GET_SWPMI1_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SWPSEL)))
#endif /* RCC_D2CCIP1R_SWPSEL */

 /** @brief  Macro to configure the DFSDM1 clock
  * @param  __DFSDM1CLKSource__ specifies the DFSDM1  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_DFSDM1CLKSOURCE_D2PCLK:  D2PCLK Clock selected as DFSDM1 clock
  *            @arg RCC_DFSDM1CLKSOURCE_SYS:     System Clock selected as DFSDM1 clock
  */
#if defined(RCC_D2CCIP1R_DFSDM1SEL)
#define __HAL_RCC_DFSDM1_CONFIG(__DFSDM1CLKSource__) \
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_DFSDM1SEL, (uint32_t)(__DFSDM1CLKSource__))
#else
#define __HAL_RCC_DFSDM1_CONFIG(__DFSDM1CLKSource__) \
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_DFSDM1SEL, (uint32_t)(__DFSDM1CLKSource__))
#endif /* RCC_D2CCIP1R_DFSDM1SEL */

/** @brief  Macro to get the DFSDM1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_DFSDM1CLKSOURCE_D2PCLK:  D2PCLK Clock selected as DFSDM1 clock
  *            @arg RCC_DFSDM1CLKSOURCE_SYS:   System Clock selected as DFSDM1 clock
  */
#if defined (RCC_D2CCIP1R_DFSDM1SEL)
#define __HAL_RCC_GET_DFSDM1_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_DFSDM1SEL)))
#else
#define __HAL_RCC_GET_DFSDM1_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_DFSDM1SEL)))
#endif /* RCC_D2CCIP1R_DFSDM1SEL */

#if defined(DFSDM2_BASE)
 /** @brief  Macro to configure the DFSDM2 clock
  * @param  __DFSDM2CLKSource__ specifies the DFSDM2  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_DFSDM2CLKSOURCE_SRDPCLK1:  SRDPCLK1 (APB4) selected as DFSDM2 clock
  *            @arg RCC_DFSDM2CLKSOURCE_SYS:   System Clock selected as DFSDM2 clock
  */
#define __HAL_RCC_DFSDM2_CONFIG(__DFSDM2CLKSource__) \
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_DFSDM2SEL, (uint32_t)(__DFSDM2CLKSource__))

/** @brief  Macro to get the DFSDM2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_DFSDM2CLKSOURCE_SRDPCLK1:  SRDPCLK1 (APB4) Clock selected as DFSDM2 clock
  *            @arg RCC_DFSDM2CLKSOURCE_SYS:   System Clock selected as DFSDM2 clock
  */
#define __HAL_RCC_GET_DFSDM2_SOURCE() ((uint32_t)(READ_BIT(RCC->SRDCCIPR, RCC_SRDCCIPR_DFSDM2SEL)))
#endif /* DFSDM2 */

/** @brief macro to configure the CEC clock (CECCLK).
  *
  * @param  __CECCLKSource__ specifies the CEC clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_CECCLKSOURCE_LSE: LSE selected as CEC clock
  *            @arg RCC_CECCLKSOURCE_LSI: LSI selected as CEC clock
  *            @arg RCC_CECCLKSOURCE_CSI: CSI Clock selected as CEC clock
  */
#if defined(RCC_D2CCIP2R_CECSEL)
#define __HAL_RCC_CEC_CONFIG(__CECCLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_CECSEL, (uint32_t)(__CECCLKSource__))
#else
#define __HAL_RCC_CEC_CONFIG(__CECCLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_CDCCIP2R_CECSEL, (uint32_t)(__CECCLKSource__))
#endif /* RCC_D2CCIP2R_CECSEL */

/** @brief  macro to get the CEC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_CECCLKSOURCE_LSE: LSE selected as CEC clock
  *            @arg RCC_CECCLKSOURCE_LSI: LSI selected as CEC clock
  *            @arg RCC_CECCLKSOURCE_CSI: CSI Clock selected as CEC clock
  */
#if defined(RCC_D2CCIP2R_CECSEL)
#define __HAL_RCC_GET_CEC_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_CECSEL)))
#else
#define __HAL_RCC_GET_CEC_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_CECSEL)))
#endif /* RCC_D2CCIP2R_CECSEL */

/** @brief  Macro to configure the CLKP : Oscillator clock for peripheral
  * @param  __CLKPSource__ specifies Oscillator clock for peripheral
  *         This parameter can be one of the following values:
  *            @arg RCC_CLKPSOURCE_HSI: HSI selected Oscillator clock for peripheral
  *            @arg RCC_CLKPSOURCE_CSI: CSI selected Oscillator clock for peripheral
  *            @arg RCC_CLKPSOURCE_HSE: HSE selected Oscillator clock for peripheral
  */
#if defined(RCC_D1CCIPR_CKPERSEL)
#define __HAL_RCC_CLKP_CONFIG(__CLKPSource__) \
                  MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_CKPERSEL, (uint32_t)(__CLKPSource__))
#else
#define __HAL_RCC_CLKP_CONFIG(__CLKPSource__) \
                  MODIFY_REG(RCC->CDCCIPR, RCC_CDCCIPR_CKPERSEL, (uint32_t)(__CLKPSource__))
#endif /* RCC_D1CCIPR_CKPERSEL */

/** @brief  Macro to get the Oscillator clock for peripheral  source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_CLKPSOURCE_HSI: HSI selected Oscillator clock for peripheral
  *            @arg RCC_CLKPSOURCE_CSI: CSI selected Oscillator clock for peripheral
  *            @arg RCC_CLKPSOURCE_HSE: HSE selected Oscillator clock for peripheral
  */
#if defined(RCC_D1CCIPR_CKPERSEL)
#define __HAL_RCC_GET_CLKP_SOURCE() ((uint32_t)(READ_BIT(RCC->D1CCIPR, RCC_D1CCIPR_CKPERSEL)))
#else
#define __HAL_RCC_GET_CLKP_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIPR, RCC_CDCCIPR_CKPERSEL)))
#endif /* RCC_D1CCIPR_CKPERSEL */

#if defined(FDCAN1) || defined(FDCAN2)
/** @brief  Macro to configure the FDCAN clock
  * @param  __FDCANCLKSource__ specifies  clock source  for FDCAN
  *         This parameter can be one of the following values:
  *            @arg RCC_FDCANCLKSOURCE_HSE: HSE selected as FDCAN clock
  *            @arg RCC_FDCANCLKSOURCE_PLL: PLL selected as FDCAN clock
  *            @arg RCC_FDCANCLKSOURCE_PLL2: PLL2 selected as FDCAN clock
  */
#if defined(RCC_D2CCIP1R_FDCANSEL)
#define __HAL_RCC_FDCAN_CONFIG(__FDCANCLKSource__) \
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_FDCANSEL, (uint32_t)(__FDCANCLKSource__))
#else
#define __HAL_RCC_FDCAN_CONFIG(__FDCANCLKSource__) \
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_FDCANSEL, (uint32_t)(__FDCANCLKSource__))
#endif /* RCC_D2CCIP1R_FDCANSEL */

/** @brief  Macro to get the FDCAN clock
  * @retval The clock source can be one of the following values:
  *            @arg RCC_FDCANCLKSOURCE_HSE: HSE selected as FDCAN clock
  *            @arg RCC_FDCANCLKSOURCE_PLL: PLL selected as FDCAN clock
  *            @arg RCC_FDCANCLKSOURCE_PLL2: PLL2 selected as FDCAN clock
  */
#if defined(RCC_D2CCIP1R_FDCANSEL)
#define __HAL_RCC_GET_FDCAN_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_FDCANSEL)))
#else
#define __HAL_RCC_GET_FDCAN_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_FDCANSEL)))
#endif /* RCC_D2CCIP1R_FDCANSEL */

#endif /*FDCAN1 || FDCAN2*/

/**
  * @brief  Macro to Configure the SPI1/2/3 clock source.
  * @param  __RCC_SPI123CLKSource__ defines the SPI1/2/3 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI123CLKSOURCE_PLL: SPI1/2/3 clock = PLL
  *             @arg RCC_SPI123CLKSOURCE_PLL2: SPI1/2/3 clock = PLL2
  *             @arg RCC_SPI123CLKSOURCE_PLL3: SPI1/2/3 clock = PLL3
  *             @arg RCC_SPI123CLKSOURCE_CLKP: SPI1/2/3 clock  = CLKP
  *             @arg RCC_SPI123CLKSOURCE_PIN: SPI1/2/3 clock = External Clock
  * @retval None
  */
#if defined(RCC_D2CCIP1R_SPI123SEL)
#define __HAL_RCC_SPI123_CONFIG(__RCC_SPI123CLKSource__ )\
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_SPI123SEL, (__RCC_SPI123CLKSource__))
#else
#define __HAL_RCC_SPI123_CONFIG(__RCC_SPI123CLKSource__ )\
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SPI123SEL, (__RCC_SPI123CLKSource__))
#endif /* RCC_D2CCIP1R_SPI123SEL */

/** @brief  Macro to get the SPI1/2/3 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI123CLKSOURCE_PLL: SPI1/2/3 clock = PLL
  *             @arg RCC_SPI123CLKSOURCE_PLL2: SPI1/2/3 clock = PLL2
  *             @arg RCC_SPI123CLKSOURCE_PLL3: SPI1/2/3 clock = PLL3
  *             @arg RCC_SPI123CLKSOURCE_CLKP: SPI1/2/3 clock  = CLKP
  *             @arg RCC_SPI123CLKSOURCE_PIN: SPI1/2/3 clock = External Clock
  */
#if defined(RCC_D2CCIP1R_SPI123SEL)
#define __HAL_RCC_GET_SPI123_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_SPI123SEL)))
#else
#define __HAL_RCC_GET_SPI123_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SPI123SEL)))
#endif /* RCC_D2CCIP1R_SPI123SEL */

/**
  * @brief  Macro to Configure the SPI1 clock source.
  * @param  __RCC_SPI1CLKSource__ defines the SPI1 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI1CLKSOURCE_PLL: SPI1 clock = PLL
  *             @arg RCC_SPI1CLKSOURCE_PLL2: SPI1 clock = PLL2
  *             @arg RCC_SPI1CLKSOURCE_PLL3: SPI1 clock = PLL3
  *             @arg RCC_SPI1CLKSOURCE_CLKP: SPI1 clock  = CLKP
  *             @arg RCC_SPI1CLKSOURCE_PIN: SPI1 clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SPI1_CONFIG  __HAL_RCC_SPI123_CONFIG

/** @brief  Macro to get the SPI1 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI1CLKSOURCE_PLL: SPI1 clock = PLL
  *             @arg RCC_SPI1CLKSOURCE_PLL2: SPI1 clock = PLL2
  *             @arg RCC_SPI1CLKSOURCE_PLL3: SPI1 clock = PLL3
  *             @arg RCC_SPI1CLKSOURCE_CLKP: SPI1 clock  = CLKP
  *             @arg RCC_SPI1CLKSOURCE_PIN: SPI1 clock = External Clock
  */
#define __HAL_RCC_GET_SPI1_SOURCE  __HAL_RCC_GET_SPI123_SOURCE

/**
  * @brief  Macro to Configure the SPI2 clock source.
  * @param  __RCC_SPI2CLKSource__ defines the SPI2 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI2CLKSOURCE_PLL: SPI2 clock = PLL
  *             @arg RCC_SPI2CLKSOURCE_PLL2: SPI2 clock = PLL2
  *             @arg RCC_SPI2CLKSOURCE_PLL3: SPI2 clock = PLL3
  *             @arg RCC_SPI2CLKSOURCE_CLKP: SPI2 clock  = CLKP
  *             @arg RCC_SPI2CLKSOURCE_PIN: SPI2 clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SPI2_CONFIG  __HAL_RCC_SPI123_CONFIG

/** @brief  Macro to get the SPI2 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI2CLKSOURCE_PLL: SPI2 clock = PLL
  *             @arg RCC_SPI2CLKSOURCE_PLL2: SPI2 clock = PLL2
  *             @arg RCC_SPI2CLKSOURCE_PLL3: SPI2 clock = PLL3
  *             @arg RCC_SPI2CLKSOURCE_CLKP: SPI2 clock  = CLKP
  *             @arg RCC_SPI2CLKSOURCE_PIN: SPI2 clock = External Clock
  */
#define __HAL_RCC_GET_SPI2_SOURCE  __HAL_RCC_GET_SPI123_SOURCE

/**
  * @brief  Macro to Configure the SPI3 clock source.
  * @param  __RCC_SPI3CLKSource__ defines the SPI3 clock source. This clock is derived
  *         from system PLL, PLL2, PLL3, OSC or external clock (through a dedicated PIN)
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI3CLKSOURCE_PLL: SPI3 clock = PLL
  *             @arg RCC_SPI3CLKSOURCE_PLL2: SPI3 clock = PLL2
  *             @arg RCC_SPI3CLKSOURCE_PLL3: SPI3 clock = PLL3
  *             @arg RCC_SPI3CLKSOURCE_CLKP: SPI3 clock  = CLKP
  *             @arg RCC_SPI3CLKSOURCE_PIN: SPI3 clock = External Clock
  * @retval None
  */
#define __HAL_RCC_SPI3_CONFIG  __HAL_RCC_SPI123_CONFIG

/** @brief  Macro to get the SPI3 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI3CLKSOURCE_PLL: SPI3 clock = PLL
  *             @arg RCC_SPI3CLKSOURCE_PLL2: SPI3 clock = PLL2
  *             @arg RCC_SPI3CLKSOURCE_PLL3: SPI3 clock = PLL3
  *             @arg RCC_SPI3CLKSOURCE_CLKP: SPI3 clock  = CLKP
  *             @arg RCC_SPI3CLKSOURCE_PIN: SPI3 clock = External Clock
  */
#define __HAL_RCC_GET_SPI3_SOURCE  __HAL_RCC_GET_SPI123_SOURCE

/**
  * @brief  Macro to Configure the SPI4/5 clock source.
  * @param  __RCC_SPI45CLKSource__ defines the SPI4/5 clock source. This clock is derived
  *         from system PCLK, PLL2, PLL3, OSC
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI45CLKSOURCE_D2PCLK1:SPI4/5 clock = D2PCLK1
  *             @arg RCC_SPI45CLKSOURCE_PLL2:   SPI4/5 clock = PLL2
  *             @arg RCC_SPI45CLKSOURCE_PLL3:   SPI4/5 clock = PLL3
  *             @arg RCC_SPI45CLKSOURCE_HSI:    SPI4/5 clock = HSI
  *             @arg RCC_SPI45CLKSOURCE_CSI:    SPI4/5 clock = CSI
  *             @arg RCC_SPI45CLKSOURCE_HSE:    SPI4/5 clock = HSE
  * @retval None
  */
#if defined(RCC_D2CCIP1R_SPI45SEL)
#define __HAL_RCC_SPI45_CONFIG(__RCC_SPI45CLKSource__ )\
                  MODIFY_REG(RCC->D2CCIP1R, RCC_D2CCIP1R_SPI45SEL, (__RCC_SPI45CLKSource__))
#else
#define __HAL_RCC_SPI45_CONFIG(__RCC_SPI45CLKSource__ )\
                  MODIFY_REG(RCC->CDCCIP1R, RCC_CDCCIP1R_SPI45SEL, (__RCC_SPI45CLKSource__))
#endif /* RCC_D2CCIP1R_SPI45SEL */

/** @brief  Macro to get the SPI4/5 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI45CLKSOURCE_D2PCLK1:SPI4/5 clock = D2PCLK1
  *             @arg RCC_SPI45CLKSOURCE_PLL2:   SPI4/5 clock = PLL2
  *             @arg RCC_SPI45CLKSOURCE_PLL3:   SPI4/5 clock = PLL3
  *             @arg RCC_SPI45CLKSOURCE_HSI:    SPI4/5 clock = HSI
  *             @arg RCC_SPI45CLKSOURCE_CSI:    SPI4/5 clock = CSI
  *             @arg RCC_SPI45CLKSOURCE_HSE:    SPI4/5 clock = HSE
*/
#if defined(RCC_D2CCIP1R_SPI45SEL)
#define __HAL_RCC_GET_SPI45_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP1R, RCC_D2CCIP1R_SPI45SEL)))
#else
#define __HAL_RCC_GET_SPI45_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP1R, RCC_CDCCIP1R_SPI45SEL)))
#endif /* RCC_D2CCIP1R_SPI45SEL */

/**
  * @brief  Macro to Configure the SPI4 clock source.
  * @param  __RCC_SPI4CLKSource__ defines the SPI4 clock source. This clock is derived
  *         from system PCLK, PLL2, PLL3, OSC
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI4CLKSOURCE_D2PCLK1:SPI4 clock = D2PCLK1
  *             @arg RCC_SPI4CLKSOURCE_PLL2:   SPI4 clock = PLL2
  *             @arg RCC_SPI4CLKSOURCE_PLL3:   SPI4 clock = PLL3
  *             @arg RCC_SPI4CLKSOURCE_HSI:    SPI4 clock = HSI
  *             @arg RCC_SPI4CLKSOURCE_CSI:    SPI4 clock = CSI
  *             @arg RCC_SPI4CLKSOURCE_HSE:    SPI4 clock = HSE
  * @retval None
  */
#define __HAL_RCC_SPI4_CONFIG  __HAL_RCC_SPI45_CONFIG

/** @brief  Macro to get the SPI4 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI4CLKSOURCE_D2PCLK1:SPI4 clock = D2PCLK1
  *             @arg RCC_SPI4CLKSOURCE_PLL2:   SPI4 clock = PLL2
  *             @arg RCC_SPI4CLKSOURCE_PLL3:   SPI4 clock = PLL3
  *             @arg RCC_SPI4CLKSOURCE_HSI:    SPI4 clock = HSI
  *             @arg RCC_SPI4CLKSOURCE_CSI:    SPI4 clock = CSI
  *             @arg RCC_SPI4CLKSOURCE_HSE:    SPI4 clock = HSE
*/
#define __HAL_RCC_GET_SPI4_SOURCE  __HAL_RCC_GET_SPI45_SOURCE

/**
  * @brief  Macro to Configure the SPI5 clock source.
  * @param  __RCC_SPI5CLKSource__ defines the SPI5 clock source. This clock is derived
  *         from system PCLK, PLL2, PLL3, OSC
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI5CLKSOURCE_D2PCLK1:SPI5 clock = D2PCLK1
  *             @arg RCC_SPI5CLKSOURCE_PLL2:   SPI5 clock = PLL2
  *             @arg RCC_SPI5CLKSOURCE_PLL3:   SPI5 clock = PLL3
  *             @arg RCC_SPI5CLKSOURCE_HSI:    SPI5 clock = HSI
  *             @arg RCC_SPI5CLKSOURCE_CSI:    SPI5 clock = CSI
  *             @arg RCC_SPI5CLKSOURCE_HSE:    SPI5 clock = HSE
  * @retval None
  */
#define __HAL_RCC_SPI5_CONFIG  __HAL_RCC_SPI45_CONFIG

/** @brief  Macro to get the SPI5 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI5CLKSOURCE_D2PCLK1:SPI5 clock = D2PCLK1
  *             @arg RCC_SPI5CLKSOURCE_PLL2:   SPI5 clock = PLL2
  *             @arg RCC_SPI5CLKSOURCE_PLL3:   SPI5 clock = PLL3
  *             @arg RCC_SPI5CLKSOURCE_HSI:    SPI5 clock = HSI
  *             @arg RCC_SPI5CLKSOURCE_CSI:    SPI5 clock = CSI
  *             @arg RCC_SPI5CLKSOURCE_HSE:    SPI5 clock = HSE
*/
#define __HAL_RCC_GET_SPI5_SOURCE  __HAL_RCC_GET_SPI45_SOURCE

/**
  * @brief  Macro to Configure the SPI6 clock source.
  * @param  __RCC_SPI6CLKSource__ defines the SPI6 clock source. This clock is derived
  *         from system PCLK, PLL2, PLL3, OSC
  *          This parameter can be one of the following values:
  *             @arg RCC_SPI6CLKSOURCE_D3PCLK1:SPI6 clock = D2PCLK1
  *             @arg RCC_SPI6CLKSOURCE_PLL2:   SPI6 clock = PLL2
  *             @arg RCC_SPI6CLKSOURCE_PLL3:   SPI6 clock = PLL3
  *             @arg RCC_SPI6CLKSOURCE_HSI:    SPI6 clock = HSI
  *             @arg RCC_SPI6CLKSOURCE_CSI:    SPI6 clock = CSI
  *             @arg RCC_SPI6CLKSOURCE_HSE:    SPI6 clock = HSE
  *             @arg RCC_SPI6CLKSOURCE_PIN:    SPI6 clock = I2S_CKIN (*)
  *
  * @retval None
  *
  * (*) : Available on stm32h7a3xx and stm32h7b3xx family lines.
  *
  */
#if defined(RCC_D3CCIPR_SPI6SEL)
#define __HAL_RCC_SPI6_CONFIG(__RCC_SPI6CLKSource__ )\
                  MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_SPI6SEL, (__RCC_SPI6CLKSource__))
#else
#define __HAL_RCC_SPI6_CONFIG(__RCC_SPI6CLKSource__ )\
                  MODIFY_REG(RCC->SRDCCIPR, RCC_SRDCCIPR_SPI6SEL, (__RCC_SPI6CLKSource__))
#endif /* RCC_D3CCIPR_SPI6SEL */

/** @brief  Macro to get the SPI6 clock source.
  * @retval The clock source can be one of the following values:
  *             @arg RCC_SPI6CLKSOURCE_D3PCLK1:SPI6 clock = D2PCLK1
  *             @arg RCC_SPI6CLKSOURCE_PLL2:   SPI6 clock = PLL2
  *             @arg RCC_SPI6CLKSOURCE_PLL3:   SPI6 clock = PLL3
  *             @arg RCC_SPI6CLKSOURCE_HSI:    SPI6 clock = HSI
  *             @arg RCC_SPI6CLKSOURCE_CSI:    SPI6 clock = CSI
  *             @arg RCC_SPI6CLKSOURCE_HSE:    SPI6 clock = HSE
  *                @arg RCC_SPI6CLKSOURCE_PIN:    SPI6 clock = I2S_CKIN
*/
#if defined(RCC_D3CCIPR_SPI6SEL)
#define __HAL_RCC_GET_SPI6_SOURCE() ((uint32_t)(READ_BIT(RCC->D3CCIPR, RCC_D3CCIPR_SPI6SEL)))
#else
#define __HAL_RCC_GET_SPI6_SOURCE() ((uint32_t)(READ_BIT(RCC->SRDCCIPR, RCC_SRDCCIPR_SPI6SEL)))
#endif /* RCC_D3CCIPR_SPI6SEL */

/** @brief  Macro to configure the SDMMC clock
  * @param  __SDMMCCLKSource__ specifies  clock source  for SDMMC
  *         This parameter can be one of the following values:
  *            @arg RCC_SDMMCCLKSOURCE_PLL:  PLLQ selected as SDMMC clock
  *            @arg RCC_SDMMCCLKSOURCE_PLL2: PLL2R selected as SDMMC clock
  */
#if defined(RCC_D1CCIPR_SDMMCSEL)
#define __HAL_RCC_SDMMC_CONFIG(__SDMMCCLKSource__) \
                  MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_SDMMCSEL, (uint32_t)(__SDMMCCLKSource__))
#else
#define __HAL_RCC_SDMMC_CONFIG(__SDMMCCLKSource__) \
                  MODIFY_REG(RCC->CDCCIPR, RCC_CDCCIPR_SDMMCSEL, (uint32_t)(__SDMMCCLKSource__))
#endif /* RCC_D1CCIPR_SDMMCSEL */

/** @brief  Macro to get the SDMMC clock
  */
#if defined(RCC_D1CCIPR_SDMMCSEL)
#define __HAL_RCC_GET_SDMMC_SOURCE() ((uint32_t)(READ_BIT(RCC->D1CCIPR, RCC_D1CCIPR_SDMMCSEL)))
#else
#define __HAL_RCC_GET_SDMMC_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIPR, RCC_CDCCIPR_SDMMCSEL)))
#endif /* RCC_D1CCIPR_SDMMCSEL */

/** @brief macro to configure the RNG clock (RNGCLK).
  *
  * @param  __RNGCLKSource__ specifies the RNG clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_RNGCLKSOURCE_HSI48: HSI48 selected as RNG clock
  *            @arg RCC_RNGCLKSOURCE_PLL: PLL1Q selected as RNG clock
  *            @arg RCC_RNGCLKSOURCE_LSE: LSE selected as RNG clock
  *            @arg RCC_RNGCLKSOURCE_LSI: LSI selected as RNG clock
  */
#if defined(RCC_D2CCIP2R_RNGSEL)
#define __HAL_RCC_RNG_CONFIG(__RNGCLKSource__) \
                  MODIFY_REG(RCC->D2CCIP2R, RCC_D2CCIP2R_RNGSEL, (uint32_t)(__RNGCLKSource__))
#else
#define __HAL_RCC_RNG_CONFIG(__RNGCLKSource__) \
                  MODIFY_REG(RCC->CDCCIP2R, RCC_CDCCIP2R_RNGSEL, (uint32_t)(__RNGCLKSource__))
#endif /* RCC_D2CCIP2R_RNGSEL */

/** @brief  macro to get the RNG clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_RNGCLKSOURCE_HSI48: HSI48 selected as RNG clock
  *            @arg RCC_RNGCLKSOURCE_PLL: PLL1Q selected as RNG clock
  *            @arg RCC_RNGCLKSOURCE_LSE: LSE selected as RNG clock
  *            @arg RCC_RNGCLKSOURCE_LSI: LSI selected as RNG clock
  */
#if defined(RCC_D2CCIP2R_RNGSEL)
#define __HAL_RCC_GET_RNG_SOURCE() ((uint32_t)(READ_BIT(RCC->D2CCIP2R, RCC_D2CCIP2R_RNGSEL)))
#else
#define __HAL_RCC_GET_RNG_SOURCE() ((uint32_t)(READ_BIT(RCC->CDCCIP2R, RCC_CDCCIP2R_RNGSEL)))
#endif /* RCC_D2CCIP2R_RNGSEL */

#if defined(HRTIM1)
/** @defgroup RCCEx_HRTIMx_Clock_Config RCC Extended HRTIMx Clock Config
  * @{
  */
/** @brief  Macro to configure the HRTIM1 prescaler clock source.
  * @param  __HRTIM1CLKSource__ specifies the HRTIM1 prescaler clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_HRTIM1CLK_TIMCLK    Timers  clock  selected as HRTIM1 prescaler clock
  *            @arg @ref RCC_HRTIM1CLK_CPUCLK CPU Clock selected as HRTIM1 clock
  */
#define __HAL_RCC_HRTIM1_CONFIG(__HRTIM1CLKSource__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_HRTIMSEL, (uint32_t)(__HRTIM1CLKSource__))

/** @brief  Macro to get the HRTIM1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg @ref RCC_HRTIM1CLK_TIMCLK   Timers  clock  selected as HRTIM1 prescaler clock
  *            @arg @ref RCC_HRTIM1CLK_CPUCLK CPU Clock selected as HRTIM1 clock
  */
#define __HAL_RCC_GET_HRTIM1_SOURCE() ((uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_HRTIMSEL)))
#endif /* HRTIM1 */

/** @brief  Macro to configure the Timers clocks prescalers
  * @param  __PRESC__  specifies the Timers clocks prescalers selection
  *         This parameter can be one of the following values:
  *            @arg RCC_TIMPRES_DESACTIVATED: The Timers kernels clocks prescaler is
  *                 equal to rcc_hclk1 if D2PPREx is corresponding to division by 1 or 2,
  *                 else it is equal to 2 x Frcc_pclkx_d2 (default after reset)
  *            @arg RCC_TIMPRES_ACTIVATED: The Timers kernels clocks prescaler is
  *                 equal to rcc_hclk1 if D2PPREx is corresponding to division by 1, 2 or 4,
  *                 else it is equal to 4 x Frcc_pclkx_d2
  */
#define __HAL_RCC_TIMCLKPRESCALER(__PRESC__) do {RCC->CFGR &= ~(RCC_CFGR_TIMPRE);\
                                                 RCC->CFGR |= (__PRESC__);       \
                                                }while(0)

/**
  * @}
  */
    
/**
  * @brief Enable the RCC LSE CSS Extended Interrupt Line.
  * @retval None
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_IT()      SET_BIT(EXTI->IMR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Extended Interrupt Line.
  * @retval None
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_IT()     CLEAR_BIT(EXTI->IMR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Enable the RCC LSE CSS Event Line.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_EVENT()   SET_BIT(EXTI->EMR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Event Line.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR1, RCC_EXTI_LINE_LSECSS)

#if defined(DUAL_CORE)
/**
  * @brief Enable the RCC LSE CSS Extended Interrupt Line for CM4.
  * @retval None
  */
#define __HAL_RCC_C2_LSECSS_EXTI_ENABLE_IT()       SET_BIT(EXTI->C2IMR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Extended Interrupt Line for CM4.
  * @retval None
  */
#define __HAL_RCC_C2_LSECSS_EXTI_DISABLE_IT()      CLEAR_BIT(EXTI->C2IMR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Enable the RCC LSE CSS Event Line for CM4.
  * @retval None.
  */
#define __HAL_RCC_C2_LSECSS_EXTI_ENABLE_EVENT()    SET_BIT(EXTI->C2EMR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Event Line for CM4.
  * @retval None.
  */
#define __HAL_RCC_C2_LSECSS_EXTI_DISABLE_EVENT()   CLEAR_BIT(EXTI->C2EMR1, RCC_EXTI_LINE_LSECSS)
#endif /* DUAL_CORE */

/**
  * @brief  Enable the RCC LSE CSS Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_FALLING_EDGE()  SET_BIT(EXTI->FTSR1, RCC_EXTI_LINE_LSECSS)


/**
  * @brief Disable the RCC LSE CSS Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR1, RCC_EXTI_LINE_LSECSS)


/**
  * @brief  Enable the RCC LSE CSS Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_RISING_EDGE()   SET_BIT(EXTI->RTSR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_RISING_EDGE()  CLEAR_BIT(EXTI->RTSR1, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Enable the RCC LSE CSS Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_RISING_FALLING_EDGE()  \
  do {                                                      \
    __HAL_RCC_LSECSS_EXTI_ENABLE_RISING_EDGE();             \
    __HAL_RCC_LSECSS_EXTI_ENABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief Disable the RCC LSE CSS Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_RISING_FALLING_EDGE()  \
  do {                                                       \
    __HAL_RCC_LSECSS_EXTI_DISABLE_RISING_EDGE();             \
    __HAL_RCC_LSECSS_EXTI_DISABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief Check whether the specified RCC LSE CSS EXTI interrupt flag is set or not.
  * @retval EXTI RCC LSE CSS Line Status.
  */
#define __HAL_RCC_LSECSS_EXTI_GET_FLAG()       (READ_BIT(EXTI->PR1, RCC_EXTI_LINE_LSECSS) == RCC_EXTI_LINE_LSECSS)

/**
  * @brief Clear the RCC LSE CSS EXTI flag.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_CLEAR_FLAG()     WRITE_REG(EXTI->PR1, RCC_EXTI_LINE_LSECSS)

#if defined(DUAL_CORE)
/**
  * @brief Check whether the specified RCC LSE CSS EXTI interrupt flag is set or not for CM4.
  * @retval EXTI RCC LSE CSS Line Status.
  */
#define __HAL_RCC_C2_LSECSS_EXTI_GET_FLAG()       (READ_BIT(EXTI->C2PR1, RCC_EXTI_LINE_LSECSS) == RCC_EXTI_LINE_LSECSS)

/**
  * @brief Clear the RCC LSE CSS EXTI flag or not for CM4.
  * @retval None.
  */
#define __HAL_RCC_C2_LSECSS_EXTI_CLEAR_FLAG()     WRITE_REG(EXTI->C2PR1, RCC_EXTI_LINE_LSECSS)
#endif /* DUAL_CORE */
/**
  * @brief Generate a Software interrupt on the RCC LSE CSS EXTI line.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_GENERATE_SWIT()  SET_BIT(EXTI->SWIER1, RCC_EXTI_LINE_LSECSS)
 
/**
  * @brief  Enable the specified CRS interrupts.
  * @param  __INTERRUPT__ specifies the CRS interrupt sources to be enabled.
  *          This parameter can be any combination of the following values:
  *              @arg @ref RCC_CRS_IT_SYNCOK  SYNC event OK interrupt
  *              @arg @ref RCC_CRS_IT_SYNCWARN  SYNC warning interrupt
  *              @arg @ref RCC_CRS_IT_ERR  Synchronization or trimming error interrupt
  *              @arg @ref RCC_CRS_IT_ESYNC  Expected SYNC interrupt
  * @retval None
  */
#define __HAL_RCC_CRS_ENABLE_IT(__INTERRUPT__)   SET_BIT(CRS->CR, (__INTERRUPT__))

/**
  * @brief  Disable the specified CRS interrupts.
  * @param  __INTERRUPT__ specifies the CRS interrupt sources to be disabled.
  *          This parameter can be any combination of the following values:
  *              @arg @ref RCC_CRS_IT_SYNCOK  SYNC event OK interrupt
  *              @arg @ref RCC_CRS_IT_SYNCWARN  SYNC warning interrupt
  *              @arg @ref RCC_CRS_IT_ERR  Synchronization or trimming error interrupt
  *              @arg @ref RCC_CRS_IT_ESYNC  Expected SYNC interrupt
  * @retval None
  */
#define __HAL_RCC_CRS_DISABLE_IT(__INTERRUPT__)  CLEAR_BIT(CRS->CR, (__INTERRUPT__))

/** @brief  Check whether the CRS interrupt has occurred or not.
  * @param  __INTERRUPT__ specifies the CRS interrupt source to check.
  *         This parameter can be one of the following values:
  *              @arg @ref RCC_CRS_IT_SYNCOK  SYNC event OK interrupt
  *              @arg @ref RCC_CRS_IT_SYNCWARN  SYNC warning interrupt
  *              @arg @ref RCC_CRS_IT_ERR  Synchronization or trimming error interrupt
  *              @arg @ref RCC_CRS_IT_ESYNC  Expected SYNC interrupt
  * @retval The new state of __INTERRUPT__ (SET or RESET).
  */
#define __HAL_RCC_CRS_GET_IT_SOURCE(__INTERRUPT__)  ((READ_BIT(CRS->CR, (__INTERRUPT__)) != 0U) ? SET : RESET)

/** @brief  Clear the CRS interrupt pending bits
  * @param  __INTERRUPT__ specifies the interrupt pending bit to clear.
  *         This parameter can be any combination of the following values:
  *              @arg @ref RCC_CRS_IT_SYNCOK  SYNC event OK interrupt
  *              @arg @ref RCC_CRS_IT_SYNCWARN  SYNC warning interrupt
  *              @arg @ref RCC_CRS_IT_ERR  Synchronization or trimming error interrupt
  *              @arg @ref RCC_CRS_IT_ESYNC  Expected SYNC interrupt
  *              @arg @ref RCC_CRS_IT_TRIMOVF  Trimming overflow or underflow interrupt
  *              @arg @ref RCC_CRS_IT_SYNCERR  SYNC error interrupt
  *              @arg @ref RCC_CRS_IT_SYNCMISS  SYNC missed interrupt
  */
/* CRS IT Error Mask */
#define  RCC_CRS_IT_ERROR_MASK                 ((uint32_t)(RCC_CRS_IT_TRIMOVF | RCC_CRS_IT_SYNCERR | RCC_CRS_IT_SYNCMISS))

#define __HAL_RCC_CRS_CLEAR_IT(__INTERRUPT__)  do { \
                                                 if(((__INTERRUPT__) & RCC_CRS_IT_ERROR_MASK) != 0U) \
                                                 { \
                                                   WRITE_REG(CRS->ICR, CRS_ICR_ERRC | ((__INTERRUPT__) & ~RCC_CRS_IT_ERROR_MASK)); \
                                                 } \
                                                 else \
                                                 { \
                                                   WRITE_REG(CRS->ICR, (__INTERRUPT__)); \
                                                 } \
                                               } while(0)

/**
  * @brief  Check whether the specified CRS flag is set or not.
  * @param  __FLAG__ specifies the flag to check.
  *          This parameter can be one of the following values:
  *              @arg @ref RCC_CRS_FLAG_SYNCOK  SYNC event OK
  *              @arg @ref RCC_CRS_FLAG_SYNCWARN  SYNC warning
  *              @arg @ref RCC_CRS_FLAG_ERR  Error
  *              @arg @ref RCC_CRS_FLAG_ESYNC  Expected SYNC
  *              @arg @ref RCC_CRS_FLAG_TRIMOVF  Trimming overflow or underflow
  *              @arg @ref RCC_CRS_FLAG_SYNCERR  SYNC error
  *              @arg @ref RCC_CRS_FLAG_SYNCMISS  SYNC missed
  * @retval The new state of _FLAG_ (TRUE or FALSE).
  */
#define __HAL_RCC_CRS_GET_FLAG(__FLAG__)  (READ_BIT(CRS->ISR, (__FLAG__)) == (__FLAG__))

/**
  * @brief  Clear the CRS specified FLAG.
  * @param __FLAG__ specifies the flag to clear.
  *          This parameter can be one of the following values:
  *              @arg @ref RCC_CRS_FLAG_SYNCOK  SYNC event OK
  *              @arg @ref RCC_CRS_FLAG_SYNCWARN  SYNC warning
  *              @arg @ref RCC_CRS_FLAG_ERR  Error
  *              @arg @ref RCC_CRS_FLAG_ESYNC  Expected SYNC
  *              @arg @ref RCC_CRS_FLAG_TRIMOVF  Trimming overflow or underflow
  *              @arg @ref RCC_CRS_FLAG_SYNCERR  SYNC error
  *              @arg @ref RCC_CRS_FLAG_SYNCMISS  SYNC missed
  * @note RCC_CRS_FLAG_ERR clears RCC_CRS_FLAG_TRIMOVF, RCC_CRS_FLAG_SYNCERR, RCC_CRS_FLAG_SYNCMISS and consequently RCC_CRS_FLAG_ERR
  * @retval None
  */

/* CRS Flag Error Mask */
#define RCC_CRS_FLAG_ERROR_MASK                ((uint32_t)(RCC_CRS_FLAG_TRIMOVF | RCC_CRS_FLAG_SYNCERR | RCC_CRS_FLAG_SYNCMISS))

#define __HAL_RCC_CRS_CLEAR_FLAG(__FLAG__)     do { \
                                                 if(((__FLAG__) & RCC_CRS_FLAG_ERROR_MASK) != 0U) \
                                                 { \
                                                   WRITE_REG(CRS->ICR, CRS_ICR_ERRC | ((__FLAG__) & ~RCC_CRS_FLAG_ERROR_MASK)); \
                                                 } \
                                                 else \
                                                 { \
                                                   WRITE_REG(CRS->ICR, (__FLAG__)); \
                                                 } \
                                               } while(0)

 /** @defgroup RCCEx_CRS_Extended_Features RCCEx CRS Extended Features
  * @{
  */
/**
  * @brief  Enable the oscillator clock for frequency error counter.
  * @note   when the CEN bit is set the CRS_CFGR register becomes write-protected.
  * @retval None
  */
#define __HAL_RCC_CRS_FREQ_ERROR_COUNTER_ENABLE()  SET_BIT(CRS->CR, CRS_CR_CEN)

/**
  * @brief  Disable the oscillator clock for frequency error counter.
  * @retval None
  */
#define __HAL_RCC_CRS_FREQ_ERROR_COUNTER_DISABLE() CLEAR_BIT(CRS->CR, CRS_CR_CEN)

/**
  * @brief  Enable the automatic hardware adjustment of TRIM bits.
  * @note   When the AUTOTRIMEN bit is set the CRS_CFGR register becomes write-protected.
  * @retval None
  */
#define __HAL_RCC_CRS_AUTOMATIC_CALIB_ENABLE()     SET_BIT(CRS->CR, CRS_CR_AUTOTRIMEN)

/**
  * @brief  Enable or disable the automatic hardware adjustment of TRIM bits.
  * @retval None
  */
#define __HAL_RCC_CRS_AUTOMATIC_CALIB_DISABLE()    CLEAR_BIT(CRS->CR, CRS_CR_AUTOTRIMEN)

/**
  * @brief  Macro to calculate reload value to be set in CRS register according to target and sync frequencies
  * @note   The RELOAD value should be selected according to the ratio between the target frequency and the frequency
  *             of the synchronization source after pre-scaling. It is then decreased by one in order to
  *             reach the expected synchronization on the zero value. The formula is the following:
  *             RELOAD = (fTARGET / fSYNC) -1
  * @param  __FTARGET__ Target frequency (value in Hz)
  * @param  __FSYNC__ Synchronization signal frequency (value in Hz)
  * @retval None
  */
#define __HAL_RCC_CRS_RELOADVALUE_CALCULATE(__FTARGET__, __FSYNC__)  (((__FTARGET__) / (__FSYNC__)) - 1U)


/**
  * @}
  */


/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup RCCEx_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);
uint32_t HAL_RCCEx_GetD1PCLK1Freq(void);
uint32_t HAL_RCCEx_GetD3PCLK1Freq(void);
uint32_t HAL_RCCEx_GetD1SysClockFreq(void);
void     HAL_RCCEx_GetPLL1ClockFreq(PLL1_ClocksTypeDef* PLL1_Clocks);
void     HAL_RCCEx_GetPLL2ClockFreq(PLL2_ClocksTypeDef* PLL2_Clocks);
void     HAL_RCCEx_GetPLL3ClockFreq(PLL3_ClocksTypeDef* PLL3_Clocks);
/**
  * @}
  */

/** @addtogroup RCCEx_Exported_Functions_Group2
  * @{
  */
void HAL_RCCEx_WakeUpStopCLKConfig(uint32_t WakeUpClk);
void HAL_RCCEx_KerWakeUpStopCLKConfig(uint32_t WakeUpClk);
void HAL_RCCEx_EnableLSECSS(void);
void HAL_RCCEx_DisableLSECSS(void);
void HAL_RCCEx_EnableLSECSS_IT(void);
void HAL_RCCEx_LSECSS_IRQHandler(void);
void HAL_RCCEx_LSECSS_Callback(void);
#if defined(DUAL_CORE)
void HAL_RCCEx_EnableBootCore(uint32_t RCC_BootCx);
#endif /*DUAL_CORE*/
#if defined(RCC_GCR_WW1RSC)
void HAL_RCCEx_WWDGxSysResetConfig(uint32_t RCC_WWDGx);
#endif /*RCC_GCR_WW1RSC*/
/**
  * @}
  */


/** @addtogroup RCCEx_Exported_Functions_Group3
  * @{
  */

void     HAL_RCCEx_CRSConfig(RCC_CRSInitTypeDef *pInit);
void     HAL_RCCEx_CRSSoftwareSynchronizationGenerate(void);
void     HAL_RCCEx_CRSGetSynchronizationInfo(RCC_CRSSynchroInfoTypeDef *pSynchroInfo);
uint32_t HAL_RCCEx_CRSWaitSynchronization(uint32_t Timeout);
void     HAL_RCCEx_CRS_IRQHandler(void);
void     HAL_RCCEx_CRS_SyncOkCallback(void);
void     HAL_RCCEx_CRS_SyncWarnCallback(void);
void     HAL_RCCEx_CRS_ExpectedSyncCallback(void);
void     HAL_RCCEx_CRS_ErrorCallback(uint32_t Error);

/**
  * @}
  */

 /* Private macros ------------------------------------------------------------*/
/** @addtogroup RCCEx_Private_Macros RCCEx Private Macros
  * @{
  */
/** @defgroup RCCEx_IS_RCC_Definitions RCC Private macros to check input parameters
  * @{
  */

#define IS_RCC_PLL2CLOCKOUT_VALUE(VALUE) (((VALUE) == RCC_PLL2_DIVP) || \
                                         ((VALUE) == RCC_PLL2_DIVQ)  || \
                                         ((VALUE) == RCC_PLL2_DIVR))

#define IS_RCC_PLL3CLOCKOUT_VALUE(VALUE) (((VALUE) == RCC_PLL3_DIVP) || \
                                          ((VALUE) == RCC_PLL3_DIVQ) || \
                                          ((VALUE) == RCC_PLL3_DIVR))

#if defined(RCC_D2CCIP2R_USART16SEL)
#define IS_RCC_USART16CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART16CLKSOURCE_D2PCLK2)|| \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_PLL2)   || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_PLL3)   || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_CSI)    || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_LSE)    || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_HSI))
#else
#define IS_RCC_USART16CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART16CLKSOURCE_D2PCLK2)|| \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_CDPCLK2)|| \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_PLL2)   || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_PLL3)   || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_CSI)    || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_LSE)    || \
                                         ((SOURCE) == RCC_USART16CLKSOURCE_HSI))
/* alias*/
#define IS_RCC_USART16910CLKSOURCE    IS_RCC_USART16CLKSOURCE
#endif /* RCC_D2CCIP2R_USART16SEL */

#if defined(RCC_D2CCIP2R_USART28SEL)
#define IS_RCC_USART234578CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART234578CLKSOURCE_D2PCLK1)|| \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_PLL2)   || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_PLL3)   || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_CSI)    || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_LSE)    || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_HSI))
#else
#define IS_RCC_USART234578CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART234578CLKSOURCE_D2PCLK1)|| \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_CDPCLK1)|| \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_PLL2)   || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_PLL3)   || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_CSI)    || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_LSE)    || \
                                             ((SOURCE) == RCC_USART234578CLKSOURCE_HSI))
#endif /* RCC_D2CCIP2R_USART28SEL */

#define IS_RCC_USART1CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART1CLKSOURCE_D2PCLK2)|| \
                                        ((SOURCE) == RCC_USART1CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_USART1CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_USART1CLKSOURCE_CSI)    || \
                                        ((SOURCE) == RCC_USART1CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_USART1CLKSOURCE_HSI))

#define IS_RCC_USART2CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART2CLKSOURCE_D2PCLK1)|| \
                                        ((SOURCE) == RCC_USART2CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_USART2CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_USART2CLKSOURCE_CSI)    || \
                                        ((SOURCE) == RCC_USART2CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_USART2CLKSOURCE_HSI))

#define IS_RCC_USART3CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART3CLKSOURCE_D2PCLK1)|| \
                                        ((SOURCE) == RCC_USART3CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_USART3CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_USART3CLKSOURCE_CSI)    || \
                                        ((SOURCE) == RCC_USART3CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_USART3CLKSOURCE_HSI))

#define IS_RCC_UART4CLKSOURCE(SOURCE)  (((SOURCE) == RCC_UART4CLKSOURCE_D2PCLK1) || \
                                        ((SOURCE) == RCC_UART4CLKSOURCE_PLL2)    || \
                                        ((SOURCE) == RCC_UART4CLKSOURCE_PLL3)    || \
                                        ((SOURCE) == RCC_UART4CLKSOURCE_CSI)     || \
                                        ((SOURCE) == RCC_UART4CLKSOURCE_LSE)     || \
                                        ((SOURCE) == RCC_UART4CLKSOURCE_HSI))

#define IS_RCC_UART5CLKSOURCE(SOURCE)  (((SOURCE) == RCC_UART5CLKSOURCE_D2PCLK1) || \
                                        ((SOURCE) == RCC_UART5CLKSOURCE_PLL2)    || \
                                        ((SOURCE) == RCC_UART5CLKSOURCE_PLL3)    || \
                                        ((SOURCE) == RCC_UART5CLKSOURCE_CSI)     || \
                                        ((SOURCE) == RCC_UART5CLKSOURCE_LSE)     || \
                                        ((SOURCE) == RCC_UART5CLKSOURCE_HSI))

#define IS_RCC_USART6CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART6CLKSOURCE_D2PCLK2)|| \
                                        ((SOURCE) == RCC_USART6CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_USART6CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_USART6CLKSOURCE_CSI)    || \
                                        ((SOURCE) == RCC_USART6CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_USART6CLKSOURCE_HSI))

#define IS_RCC_UART7CLKSOURCE(SOURCE)  (((SOURCE) == RCC_UART7CLKSOURCE_D2PCLK1) || \
                                        ((SOURCE) == RCC_UART7CLKSOURCE_PLL2)    || \
                                        ((SOURCE) == RCC_UART7CLKSOURCE_PLL3)    || \
                                        ((SOURCE) == RCC_UART7CLKSOURCE_CSI)     || \
                                        ((SOURCE) == RCC_UART7CLKSOURCE_LSE)     || \
                                        ((SOURCE) == RCC_UART7CLKSOURCE_HSI))

#define IS_RCC_UART8CLKSOURCE(SOURCE)  (((SOURCE) == RCC_UART8CLKSOURCE_D2PCLK1) || \
                                        ((SOURCE) == RCC_UART8CLKSOURCE_PLL2)    || \
                                        ((SOURCE) == RCC_UART8CLKSOURCE_PLL3)    || \
                                        ((SOURCE) == RCC_UART8CLKSOURCE_CSI)     || \
                                        ((SOURCE) == RCC_UART8CLKSOURCE_LSE)     || \
                                        ((SOURCE) == RCC_UART8CLKSOURCE_HSI))

#if defined(UART9)
#define IS_RCC_UART9CLKSOURCE(SOURCE) (((SOURCE) == RCC_UART9CLKSOURCE_D2PCLK2)|| \
                                        ((SOURCE) == RCC_UART9CLKSOURCE_PLL2)  || \
                                        ((SOURCE) == RCC_UART9CLKSOURCE_PLL3)  || \
                                        ((SOURCE) == RCC_UART9CLKSOURCE_CSI)   || \
                                        ((SOURCE) == RCC_UART9CLKSOURCE_LSE)   || \
                                        ((SOURCE) == RCC_UART9CLKSOURCE_HSI))
#endif

#if defined(USART10)
#define IS_RCC_USART10CLKSOURCE(SOURCE) (((SOURCE) == RCC_USART10CLKSOURCE_D2PCLK2)|| \
                                        ((SOURCE) == RCC_USART10CLKSOURCE_PLL2)    || \
                                        ((SOURCE) == RCC_USART10CLKSOURCE_PLL3)    || \
                                        ((SOURCE) == RCC_USART10CLKSOURCE_CSI)     || \
                                        ((SOURCE) == RCC_USART10CLKSOURCE_LSE)     || \
                                        ((SOURCE) == RCC_USART10CLKSOURCE_HSI))
#endif

#define IS_RCC_LPUART1CLKSOURCE(SOURCE) (((SOURCE) == RCC_LPUART1CLKSOURCE_D3PCLK1) || \
                                         ((SOURCE) == RCC_LPUART1CLKSOURCE_PLL2)    || \
                                         ((SOURCE) == RCC_LPUART1CLKSOURCE_PLL3)    || \
                                         ((SOURCE) == RCC_LPUART1CLKSOURCE_CSI)     || \
                                         ((SOURCE) == RCC_LPUART1CLKSOURCE_LSE)     || \
                                         ((SOURCE) == RCC_LPUART1CLKSOURCE_HSI))

#define IS_RCC_I2C123CLKSOURCE(SOURCE)   (((SOURCE) == RCC_I2C123CLKSOURCE_PLL3)   || \
                                          ((SOURCE) == RCC_I2C123CLKSOURCE_HSI)    || \
                                          ((SOURCE) == RCC_I2C123CLKSOURCE_D2PCLK1)|| \
                                          ((SOURCE) == RCC_I2C123CLKSOURCE_CSI))

#define IS_RCC_I2C1CLKSOURCE(SOURCE)   (((SOURCE) == RCC_I2C1CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_I2C1CLKSOURCE_HSI)    || \
                                        ((SOURCE) == RCC_I2C1CLKSOURCE_D2PCLK1)|| \
                                        ((SOURCE) == RCC_I2C1CLKSOURCE_CSI))

#define IS_RCC_I2C2CLKSOURCE(SOURCE)   (((SOURCE) == RCC_I2C2CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_I2C2CLKSOURCE_HSI)    || \
                                        ((SOURCE) == RCC_I2C2CLKSOURCE_D2PCLK1)|| \
                                        ((SOURCE) == RCC_I2C2CLKSOURCE_CSI))

#define IS_RCC_I2C3CLKSOURCE(SOURCE)   (((SOURCE) == RCC_I2C3CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_I2C3CLKSOURCE_HSI)    || \
                                        ((SOURCE) == RCC_I2C3CLKSOURCE_D2PCLK1)|| \
                                        ((SOURCE) == RCC_I2C3CLKSOURCE_CSI))

#define IS_RCC_I2C4CLKSOURCE(SOURCE)   (((SOURCE) == RCC_I2C4CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_I2C4CLKSOURCE_HSI)    || \
                                        ((SOURCE) == RCC_I2C4CLKSOURCE_D3PCLK1)|| \
                                        ((SOURCE) == RCC_I2C3CLKSOURCE_CSI))

#define IS_RCC_RNGCLKSOURCE(SOURCE)    (((SOURCE) == RCC_RNGCLKSOURCE_HSI48)|| \
                                        ((SOURCE) == RCC_RNGCLKSOURCE_PLL)  || \
                                        ((SOURCE) == RCC_RNGCLKSOURCE_LSE)  || \
                                        ((SOURCE) == RCC_RNGCLKSOURCE_LSI))

#if defined(HRTIM1)
#define IS_RCC_HRTIM1CLKSOURCE(SOURCE) (((SOURCE) == RCC_HRTIM1CLK_TIMCLK) || \
                                        ((SOURCE) == RCC_HRTIM1CLK_CPUCLK))
#endif

#define IS_RCC_USBCLKSOURCE(SOURCE)    (((SOURCE) == RCC_USBCLKSOURCE_PLL)  || \
                                        ((SOURCE) == RCC_USBCLKSOURCE_PLL3) || \
                                        ((SOURCE) == RCC_USBCLKSOURCE_HSI48))

#define IS_RCC_SAI1CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI1CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI1CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI1CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI1CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI1CLKSOURCE_PIN))

#if defined(SAI3)
#define IS_RCC_SAI23CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI23CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI23CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI23CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI23CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI23CLKSOURCE_PIN))

#define IS_RCC_SAI2CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI2CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI2CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI2CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI2CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI2CLKSOURCE_PIN))


#define IS_RCC_SAI3CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI3CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI3CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI3CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI3CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI3CLKSOURCE_PIN))
#endif

#if defined(RCC_CDCCIP1R_SAI2ASEL)
#define IS_RCC_SAI2ACLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI2ACLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI2ACLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI2ACLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI2ACLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI2ACLKSOURCE_PIN) || \
                ((__SOURCE__) == RCC_SAI2ACLKSOURCE_SPDIF))
#endif

#if defined(RCC_CDCCIP1R_SAI2BSEL)
#define IS_RCC_SAI2BCLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI2BCLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI2BCLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI2BCLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI2BCLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI2BCLKSOURCE_PIN) || \
                ((__SOURCE__) == RCC_SAI2BCLKSOURCE_SPDIF))
#endif

#define IS_RCC_SPI123CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI123CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SPI123CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SPI123CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SPI123CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SPI123CLKSOURCE_PIN))

#define IS_RCC_SPI1CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI1CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SPI1CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SPI1CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SPI1CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SPI1CLKSOURCE_PIN))

#define IS_RCC_SPI2CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI2CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SPI2CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SPI2CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SPI2CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SPI2CLKSOURCE_PIN))

#define IS_RCC_SPI3CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI3CLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SPI3CLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SPI3CLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SPI3CLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SPI3CLKSOURCE_PIN))

#define IS_RCC_SPI45CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI45CLKSOURCE_D2PCLK1)  || \
                ((__SOURCE__) == RCC_SPI45CLKSOURCE_PLL2)     || \
                ((__SOURCE__) == RCC_SPI45CLKSOURCE_PLL3)     || \
                ((__SOURCE__) == RCC_SPI45CLKSOURCE_HSI)      || \
                ((__SOURCE__) == RCC_SPI45CLKSOURCE_CSI)      || \
                ((__SOURCE__) == RCC_SPI45CLKSOURCE_HSE))

#define IS_RCC_SPI4CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI4CLKSOURCE_D2PCLK1)  || \
                ((__SOURCE__) == RCC_SPI4CLKSOURCE_PLL2)     || \
                ((__SOURCE__) == RCC_SPI4CLKSOURCE_PLL3)     || \
                ((__SOURCE__) == RCC_SPI4CLKSOURCE_HSI)      || \
                ((__SOURCE__) == RCC_SPI4CLKSOURCE_CSI)      || \
                ((__SOURCE__) == RCC_SPI4CLKSOURCE_HSE))

#define IS_RCC_SPI5CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI5CLKSOURCE_D2PCLK1)|| \
                ((__SOURCE__) == RCC_SPI5CLKSOURCE_PLL2)   || \
                ((__SOURCE__) == RCC_SPI5CLKSOURCE_PLL3)   || \
                ((__SOURCE__) == RCC_SPI5CLKSOURCE_HSI)    || \
                ((__SOURCE__) == RCC_SPI5CLKSOURCE_CSI)    || \
                ((__SOURCE__) == RCC_SPI5CLKSOURCE_HSE))

#if defined(RCC_D3CCIPR_SPI6SEL)
#define IS_RCC_SPI6CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI6CLKSOURCE_D3PCLK1) || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_PLL2)    || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_PLL3)    || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_HSI)     || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_CSI)     || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_HSE))
#else
#define IS_RCC_SPI6CLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SPI6CLKSOURCE_D3PCLK1) || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_PLL2)    || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_PLL3)    || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_HSI)     || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_CSI)     || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_HSE)     || \
                ((__SOURCE__) == RCC_SPI6CLKSOURCE_PIN))
#endif /* RCC_D3CCIPR_SPI6SEL */

#if defined(SAI4)
#define IS_RCC_SAI4ACLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI4ACLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI4ACLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI4ACLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI4ACLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI4ACLKSOURCE_PIN))

#define IS_RCC_SAI4BCLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_SAI4BCLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SAI4BCLKSOURCE_PLL2) || \
                ((__SOURCE__) == RCC_SAI4BCLKSOURCE_PLL3) || \
                ((__SOURCE__) == RCC_SAI4BCLKSOURCE_CLKP) || \
                ((__SOURCE__) == RCC_SAI4BCLKSOURCE_PIN))
#endif /*SAI4*/

#define IS_RCC_PLL3M_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 63U))
#define IS_RCC_PLL3N_VALUE(VALUE) ((4U <= (VALUE)) && ((VALUE) <= 512U))
#define IS_RCC_PLL3P_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))
#define IS_RCC_PLL3Q_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))
#define IS_RCC_PLL3R_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))

#define IS_RCC_PLL2M_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 63U))
#define IS_RCC_PLL2N_VALUE(VALUE) ((4U <= (VALUE)) && ((VALUE) <= 512U))
#define IS_RCC_PLL2P_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))
#define IS_RCC_PLL2Q_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))
#define IS_RCC_PLL2R_VALUE(VALUE) ((1U <= (VALUE)) && ((VALUE) <= 128U))

#define IS_RCC_PLL2RGE_VALUE(VALUE) (((VALUE) == RCC_PLL2VCIRANGE_0)  || \
                                    ((VALUE) == RCC_PLL2VCIRANGE_1)   || \
                                    ((VALUE) == RCC_PLL2VCIRANGE_2)   || \
                                    ((VALUE) == RCC_PLL2VCIRANGE_3))

#define IS_RCC_PLL3RGE_VALUE(VALUE) (((VALUE) == RCC_PLL3VCIRANGE_0)  || \
                                    ((VALUE) == RCC_PLL3VCIRANGE_1)   || \
                                    ((VALUE) == RCC_PLL3VCIRANGE_2)   || \
                                    ((VALUE) == RCC_PLL3VCIRANGE_3))

#define IS_RCC_PLL2VCO_VALUE(VALUE) (((VALUE) == RCC_PLL2VCOWIDE)  || \
                                    ((VALUE) == RCC_PLL2VCOMEDIUM))

#define IS_RCC_PLL3VCO_VALUE(VALUE) (((VALUE) == RCC_PLL3VCOWIDE)  || \
                                    ((VALUE) == RCC_PLL3VCOMEDIUM))

#define IS_RCC_PLLFRACN_VALUE(VALUE) ((VALUE) <=8191U)

#define IS_RCC_LPTIM1CLK(SOURCE)       (((SOURCE) == RCC_LPTIM1CLKSOURCE_D2PCLK1)|| \
                                        ((SOURCE) == RCC_LPTIM1CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_LPTIM1CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_LPTIM1CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_LPTIM1CLKSOURCE_LSI)    || \
                                        ((SOURCE) == RCC_LPTIM1CLKSOURCE_CLKP))

#define IS_RCC_LPTIM2CLK(SOURCE)       (((SOURCE) == RCC_LPTIM2CLKSOURCE_D3PCLK1)|| \
                                        ((SOURCE) == RCC_LPTIM2CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_LPTIM2CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_LPTIM2CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_LPTIM2CLKSOURCE_LSI)    || \
                                        ((SOURCE) == RCC_LPTIM2CLKSOURCE_CLKP))

#define IS_RCC_LPTIM345CLK(SOURCE)     (((SOURCE) == RCC_LPTIM345CLKSOURCE_D3PCLK1)|| \
                                        ((SOURCE) == RCC_LPTIM345CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_LPTIM345CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_LPTIM345CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_LPTIM345CLKSOURCE_LSI)    || \
                                        ((SOURCE) == RCC_LPTIM345CLKSOURCE_CLKP))

#define IS_RCC_LPTIM3CLK(SOURCE)       (((SOURCE) == RCC_LPTIM3CLKSOURCE_D3PCLK1)  || \
                                        ((SOURCE) == RCC_LPTIM3CLKSOURCE_PLL2)     || \
                                        ((SOURCE) == RCC_LPTIM3CLKSOURCE_PLL3)     || \
                                        ((SOURCE) == RCC_LPTIM3CLKSOURCE_LSE)      || \
                                        ((SOURCE) == RCC_LPTIM3CLKSOURCE_LSI)      || \
                                        ((SOURCE) == RCC_LPTIM3CLKSOURCE_CLKP))

#if defined(LPTIM4)
#define IS_RCC_LPTIM4CLK(SOURCE)       (((SOURCE) == RCC_LPTIM4CLKSOURCE_D3PCLK1)|| \
                                        ((SOURCE) == RCC_LPTIM4CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_LPTIM4CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_LPTIM4CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_LPTIM4CLKSOURCE_LSI)    || \
                                        ((SOURCE) == RCC_LPTIM4CLKSOURCE_CLKP))
#endif /* LPTIM4*/

#if defined(LPTIM5)
#define IS_RCC_LPTIM5CLK(SOURCE)       (((SOURCE) == RCC_LPTIM5CLKSOURCE_D3PCLK1)|| \
                                        ((SOURCE) == RCC_LPTIM5CLKSOURCE_PLL2)   || \
                                        ((SOURCE) == RCC_LPTIM5CLKSOURCE_PLL3)   || \
                                        ((SOURCE) == RCC_LPTIM5CLKSOURCE_LSE)    || \
                                        ((SOURCE) == RCC_LPTIM5CLKSOURCE_LSI)    || \
                                        ((SOURCE) == RCC_LPTIM5CLKSOURCE_CLKP))
#endif /*LPTIM5*/

#if defined(QUADSPI)
#define IS_RCC_QSPICLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_QSPICLKSOURCE_D1HCLK)  || \
                ((__SOURCE__) == RCC_QSPICLKSOURCE_PLL)     || \
                ((__SOURCE__) == RCC_QSPICLKSOURCE_PLL2)    || \
                ((__SOURCE__) == RCC_QSPICLKSOURCE_CLKP))
#endif /*QUADSPI*/

#if defined(OCTOSPI1) || defined(OCTOSPI1)
#define IS_RCC_OSPICLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_OSPICLKSOURCE_D1HCLK)  || \
                ((__SOURCE__) == RCC_OSPICLKSOURCE_PLL)     || \
                ((__SOURCE__) == RCC_OSPICLKSOURCE_PLL2)    || \
                ((__SOURCE__) == RCC_OSPICLKSOURCE_CLKP))
#endif /*OCTOSPI1 || OCTOSPI1*/

#if defined(DSI)
#define IS_RCC_DSICLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_DSICLKSOURCE_PHY)  || \
                ((__SOURCE__) == RCC_DSICLKSOURCE_PLL2))
#endif /*DSI*/

#define IS_RCC_FMCCLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_FMCCLKSOURCE_D1HCLK)  || \
                ((__SOURCE__) == RCC_FMCCLKSOURCE_PLL)     || \
                ((__SOURCE__) == RCC_FMCCLKSOURCE_PLL2)    || \
                ((__SOURCE__) == RCC_FMCCLKSOURCE_CLKP))

#if defined(FDCAN1) || defined(FDCAN2)
#define IS_RCC_FDCANCLK(__SOURCE__)   \
               (((__SOURCE__) == RCC_FDCANCLKSOURCE_HSE)  || \
                ((__SOURCE__) == RCC_FDCANCLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_FDCANCLKSOURCE_PLL2))
#endif /*FDCAN1 || FDCAN2*/

#define IS_RCC_SDMMC(__SOURCE__)   \
                (((__SOURCE__) == RCC_SDMMCCLKSOURCE_PLL)  || \
                ((__SOURCE__) == RCC_SDMMCCLKSOURCE_PLL2))

#define IS_RCC_ADCCLKSOURCE(SOURCE)    (((SOURCE) == RCC_ADCCLKSOURCE_PLL2) || \
                                        ((SOURCE) == RCC_ADCCLKSOURCE_PLL3) || \
                                        ((SOURCE) == RCC_ADCCLKSOURCE_CLKP))

#define IS_RCC_SWPMI1CLKSOURCE(SOURCE) (((SOURCE) == RCC_SWPMI1CLKSOURCE_D2PCLK1) || \
                                        ((SOURCE) == RCC_SWPMI1CLKSOURCE_HSI))

#define IS_RCC_DFSDM1CLKSOURCE(SOURCE)  (((SOURCE) == RCC_DFSDM1CLKSOURCE_D2PCLK1) || \
                                         ((SOURCE) == RCC_DFSDM1CLKSOURCE_SYS))

#if defined(DFSDM2_BASE)
#define IS_RCC_DFSDM2CLKSOURCE(SOURCE)  (((SOURCE) == RCC_DFSDM2CLKSOURCE_SRDPCLK1) || \
                                        ((SOURCE) == RCC_DFSDM2CLKSOURCE_SYS))
#endif /*DFSDM2*/

#define IS_RCC_SPDIFRXCLKSOURCE(SOURCE)(((SOURCE) == RCC_SPDIFRXCLKSOURCE_PLL)  || \
                                        ((SOURCE) == RCC_SPDIFRXCLKSOURCE_PLL2) || \
                                        ((SOURCE) == RCC_SPDIFRXCLKSOURCE_PLL3) || \
                                        ((SOURCE) == RCC_SPDIFRXCLKSOURCE_HSI))

#define IS_RCC_CECCLKSOURCE(SOURCE)  (((SOURCE) == RCC_CECCLKSOURCE_LSE) || \
                                      ((SOURCE) == RCC_CECCLKSOURCE_LSI) || \
                                      ((SOURCE) == RCC_CECCLKSOURCE_CSI))

#define IS_RCC_CLKPSOURCE(SOURCE)   (((SOURCE) == RCC_CLKPSOURCE_HSI)  || \
                                      ((SOURCE) == RCC_CLKPSOURCE_CSI) || \
                                      ((SOURCE) == RCC_CLKPSOURCE_HSE))
#define IS_RCC_TIMPRES(VALUE)  \
               (((VALUE) == RCC_TIMPRES_DESACTIVATED) || \
                ((VALUE) == RCC_TIMPRES_ACTIVATED))

#if defined(DUAL_CORE)
#define IS_RCC_BOOT_CORE(CORE)   (((CORE) == RCC_BOOT_C1)  || \
                                  ((CORE) == RCC_BOOT_C2))
#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
#define IS_RCC_SCOPE_WWDG(WWDG)   (((WWDG) == RCC_WWDG1)  || \
                                  ((WWDG) == RCC_WWDG2))
#else
#define IS_RCC_SCOPE_WWDG(WWDG)   ((WWDG) == RCC_WWDG1)

#endif /*DUAL_CORE*/

#define IS_RCC_CRS_SYNC_SOURCE(__SOURCE__) (((__SOURCE__) == RCC_CRS_SYNC_SOURCE_USB2) || \
                                            ((__SOURCE__) == RCC_CRS_SYNC_SOURCE_LSE)  || \
                                            ((__SOURCE__) == RCC_CRS_SYNC_SOURCE_USB1) || \
                                            ((__SOURCE__) == RCC_CRS_SYNC_SOURCE_PIN))

#define IS_RCC_CRS_SYNC_DIV(__DIV__)       (((__DIV__) == RCC_CRS_SYNC_DIV1)  || ((__DIV__) == RCC_CRS_SYNC_DIV2)  || \
                                            ((__DIV__) == RCC_CRS_SYNC_DIV4)  || ((__DIV__) == RCC_CRS_SYNC_DIV8)  || \
                                            ((__DIV__) == RCC_CRS_SYNC_DIV16) || ((__DIV__) == RCC_CRS_SYNC_DIV32) || \
                                            ((__DIV__) == RCC_CRS_SYNC_DIV64) || ((__DIV__) == RCC_CRS_SYNC_DIV128))

#define IS_RCC_CRS_SYNC_POLARITY(__POLARITY__) (((__POLARITY__) == RCC_CRS_SYNC_POLARITY_RISING) || \
                                                ((__POLARITY__) == RCC_CRS_SYNC_POLARITY_FALLING))

#define IS_RCC_CRS_RELOADVALUE(__VALUE__)  (((__VALUE__) <= 0xFFFFU))

#define IS_RCC_CRS_ERRORLIMIT(__VALUE__)   (((__VALUE__) <= 0xFFU))

#define IS_RCC_CRS_HSI48CALIBRATION(__VALUE__) (((__VALUE__) <= 0x3FU))

#define IS_RCC_CRS_FREQERRORDIR(__DIR__)   (((__DIR__) == RCC_CRS_FREQERRORDIR_UP) || \
                                            ((__DIR__) == RCC_CRS_FREQERRORDIR_DOWN))
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

#endif /* STM32H7xx_HAL_RCC_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
