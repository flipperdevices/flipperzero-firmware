/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rcc_ex.c
  * @author  MCD Application Team
  * @brief   Extended RCC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities RCC extension peripheral:
  *           + Extended Peripheral Control functions
  *
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup RCCEx  RCCEx
  * @brief RCC HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup RCCEx_Private_defines Private Defines
 * @{
 */
#define PLL2_TIMEOUT_VALUE         PLL_TIMEOUT_VALUE    /* 2 ms */
#define PLL3_TIMEOUT_VALUE         PLL_TIMEOUT_VALUE    /* 2 ms */

#define DIVIDER_P_UPDATE          0U
#define DIVIDER_Q_UPDATE          1U
#define DIVIDER_R_UPDATE          2U
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef RCCEx_PLL2_Config(RCC_PLL2InitTypeDef *pll2, uint32_t Divider);
static HAL_StatusTypeDef RCCEx_PLL3_Config(RCC_PLL3InitTypeDef *pll3, uint32_t Divider);

/* Exported functions --------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup RCCEx_Exported_Functions_Group1 Extended Peripheral Control functions
 *  @brief  Extended Peripheral Control functions
 *
@verbatim
 ===============================================================================
                ##### Extended Peripheral Control functions  #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the RCC Clocks
    frequencies.
    [..]
    (@) Important note: Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to
        select the RTC clock source; in this case the Backup domain will be reset in
        order to modify the RTC Clock source, as consequence RTC registers (including
        the backup registers) and RCC_BDCR register are set to their reset values.

@endverbatim
  * @{
  */
/**
  * @brief  Initializes the RCC extended peripherals clocks according to the specified
  *         parameters in the RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains the configuration information for the Extended Peripherals
  *         clocks (SDMMC, CKPER, FMC, QSPI*, OSPI*, DSI, SPI45, SPDIF, DFSDM1, DFSDM2*, FDCAN, SWPMI, SAI23*,SAI2A*, SAI2B*, SAI1, SPI123,
  *         USART234578, USART16 (USART16910*), RNG, HRTIM1*, I2C123, USB, CEC, LPTIM1, LPUART1, I2C4, LPTIM2, LPTIM345, ADC,
  *         SAI4A*, SAI4B*, SPI6, RTC).
  * @note   Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *         the RTC clock source; in this case the Backup domain will be reset in
  *         order to modify the RTC Clock source, as consequence RTC registers (including
  *         the backup registers) are set to their reset values.
  *
  * (*) : Available on some STM32H7 lines only.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  uint32_t tmpreg;
  uint32_t tickstart;
  HAL_StatusTypeDef ret = HAL_OK;      /* Intermediate status */
  HAL_StatusTypeDef status = HAL_OK;   /* Final status */

  /*---------------------------- SPDIFRX configuration -------------------------------*/

  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPDIFRX) == RCC_PERIPHCLK_SPDIFRX)
  {

    switch(PeriphClkInit->SpdifrxClockSelection)
    {
    case RCC_SPDIFRXCLKSOURCE_PLL:      /* PLL is used as clock source for SPDIFRX*/
      /* Enable PLL1Q Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SPDIFRX clock source configuration done later after clock selection check */
      break;

    case RCC_SPDIFRXCLKSOURCE_PLL2: /* PLL2 is used as clock source for SPDIFRX*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_R_UPDATE);

      /* SPDIFRX clock source configuration done later after clock selection check */
      break;

    case RCC_SPDIFRXCLKSOURCE_PLL3:  /* PLL3 is used as clock source for SPDIFRX*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE);

      /* SPDIFRX clock source configuration done later after clock selection check */
      break;

    case RCC_SPDIFRXCLKSOURCE_HSI:
      /* Internal OSC clock is used as source of SPDIFRX clock*/
      /* SPDIFRX clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SPDIFRX clock*/
      __HAL_RCC_SPDIFRX_CONFIG(PeriphClkInit->SpdifrxClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- SAI1 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI1) == RCC_PERIPHCLK_SAI1)
  {
    switch(PeriphClkInit->Sai1ClockSelection)
    {
    case RCC_SAI1CLKSOURCE_PLL:      /* PLL is used as clock source for SAI1*/
      /* Enable SAI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI1CLKSOURCE_PLL2: /* PLL2 is used as clock source for SAI1*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI1CLKSOURCE_PLL3:  /* PLL3 is used as clock source for SAI1*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_P_UPDATE);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI1CLKSOURCE_PIN:
      /* External clock is used as source of SAI1 clock*/
      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI1CLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SAI1 clock */
      /* SAI1 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SAI1 clock*/
      __HAL_RCC_SAI1_CONFIG(PeriphClkInit->Sai1ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

#if defined(SAI3)
  /*---------------------------- SAI2/3 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI23) == RCC_PERIPHCLK_SAI23)
  {
    switch(PeriphClkInit->Sai23ClockSelection)
    {
    case RCC_SAI23CLKSOURCE_PLL:      /* PLL is used as clock source for SAI2/3 */
      /* Enable SAI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SAI2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI23CLKSOURCE_PLL2: /* PLL2 is used as clock source for SAI2/3 */

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SAI2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI23CLKSOURCE_PLL3:  /* PLL3 is used as clock source for SAI2/3 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_P_UPDATE);

      /* SAI2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI23CLKSOURCE_PIN:
      /* External clock is used as source of SAI2/3 clock*/
      /* SAI2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI23CLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SAI2/3 clock */
      /* SAI2/3 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SAI2/3 clock*/
      __HAL_RCC_SAI23_CONFIG(PeriphClkInit->Sai23ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

#endif /* SAI3 */

#if defined(RCC_CDCCIP1R_SAI2ASEL)
  /*---------------------------- SAI2A configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2A) == RCC_PERIPHCLK_SAI2A)
  {
    switch(PeriphClkInit->Sai2AClockSelection)
    {
    case RCC_SAI2ACLKSOURCE_PLL:      /* PLL is used as clock source for SAI2A */
      /* Enable SAI2A Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SAI2A clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2ACLKSOURCE_PLL2: /* PLL2 is used as clock source for SAI2A */

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SAI2A clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2ACLKSOURCE_PLL3:  /* PLL3 is used as clock source for SAI2A */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_P_UPDATE);

      /* SAI2A clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2ACLKSOURCE_PIN:
      /* External clock is used as source of SAI2A clock*/
      /* SAI2A clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2ACLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SAI2A clock */
      /* SAI2A clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2ACLKSOURCE_SPDIF:
      /* SPDIF clock is used as source of SAI2A clock */
      /* SAI2A clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SAI2A clock*/
      __HAL_RCC_SAI2A_CONFIG(PeriphClkInit->Sai2AClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif  /*SAI2A*/

#if defined(RCC_CDCCIP1R_SAI2BSEL)

  /*---------------------------- SAI2B configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2B) == RCC_PERIPHCLK_SAI2B)
  {
    switch(PeriphClkInit->Sai2BClockSelection)
    {
    case RCC_SAI2BCLKSOURCE_PLL:      /* PLL is used as clock source for SAI2B */
      /* Enable SAI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SAI2B clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2BCLKSOURCE_PLL2: /* PLL2 is used as clock source for SAI2B */

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SAI2B clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2BCLKSOURCE_PLL3:  /* PLL3 is used as clock source for SAI2B */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_P_UPDATE);

      /* SAI2B clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2BCLKSOURCE_PIN:
      /* External clock is used as source of SAI2B clock*/
      /* SAI2B clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2BCLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SAI2B clock */
      /* SAI2B clock source configuration done later after clock selection check */
      break;

    case RCC_SAI2BCLKSOURCE_SPDIF:
      /* SPDIF clock is used as source of SAI2B clock */
      /* SAI2B clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SAI2B clock*/
      __HAL_RCC_SAI2B_CONFIG(PeriphClkInit->Sai2BClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif  /*SAI2B*/

#if defined(SAI4)
  /*---------------------------- SAI4A configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI4A) == RCC_PERIPHCLK_SAI4A)
  {
    switch(PeriphClkInit->Sai4AClockSelection)
    {
    case RCC_SAI4ACLKSOURCE_PLL:      /* PLL is used as clock source for SAI2*/
      /* Enable SAI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4ACLKSOURCE_PLL2: /* PLL2 is used as clock source for SAI2*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SAI2 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4ACLKSOURCE_PLL3:  /* PLL3 is used as clock source for SAI2*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_P_UPDATE);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4ACLKSOURCE_PIN:
      /* External clock is used as source of SAI2 clock*/
      /* SAI2 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4ACLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SAI2 clock */
      /* SAI1 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SAI4A clock*/
      __HAL_RCC_SAI4A_CONFIG(PeriphClkInit->Sai4AClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
  /*---------------------------- SAI4B configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI4B) == RCC_PERIPHCLK_SAI4B)
  {
    switch(PeriphClkInit->Sai4BClockSelection)
    {
    case RCC_SAI4BCLKSOURCE_PLL:      /* PLL is used as clock source for SAI2*/
      /* Enable SAI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4BCLKSOURCE_PLL2: /* PLL2 is used as clock source for SAI2*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SAI2 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4BCLKSOURCE_PLL3:  /* PLL3 is used as clock source for SAI2*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3), DIVIDER_P_UPDATE);

      /* SAI1 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4BCLKSOURCE_PIN:
      /* External clock is used as source of SAI2 clock*/
      /* SAI2 clock source configuration done later after clock selection check */
      break;

    case RCC_SAI4BCLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SAI2 clock */
      /* SAI1 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SAI4B clock*/
      __HAL_RCC_SAI4B_CONFIG(PeriphClkInit->Sai4BClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif  /*SAI4*/

#if defined(QUADSPI)
  /*---------------------------- QSPI configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_QSPI) == RCC_PERIPHCLK_QSPI)
  {
    switch(PeriphClkInit->QspiClockSelection)
    {
    case RCC_QSPICLKSOURCE_PLL:      /* PLL is used as clock source for QSPI*/
      /* Enable QSPI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* QSPI clock source configuration done later after clock selection check */
      break;

    case RCC_QSPICLKSOURCE_PLL2: /* PLL2 is used as clock source for QSPI*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_R_UPDATE);

      /* QSPI clock source configuration done later after clock selection check */
      break;


    case RCC_QSPICLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of QSPI clock */
      /* QSPI clock source configuration done later after clock selection check */
      break;

    case RCC_QSPICLKSOURCE_D1HCLK:
      /* Domain1 HCLK  clock selected as QSPI kernel peripheral clock */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of QSPI clock*/
      __HAL_RCC_QSPI_CONFIG(PeriphClkInit->QspiClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif  /*QUADSPI*/

#if defined(OCTOSPI1) || defined(OCTOSPI2)
  /*---------------------------- OCTOSPI configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_OSPI) == RCC_PERIPHCLK_OSPI)
  {
    switch(PeriphClkInit->OspiClockSelection)
    {
    case RCC_OSPICLKSOURCE_PLL:      /* PLL is used as clock source for OSPI*/
      /* Enable OSPI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* OSPI clock source configuration done later after clock selection check */
      break;

    case RCC_OSPICLKSOURCE_PLL2: /* PLL2 is used as clock source for OSPI*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_R_UPDATE);

      /* OSPI clock source configuration done later after clock selection check */
      break;


    case RCC_OSPICLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of OSPI clock */
      /* OSPI clock source configuration done later after clock selection check */
      break;

    case RCC_OSPICLKSOURCE_HCLK:
      /* HCLK clock selected as OSPI kernel peripheral clock */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of OSPI clock*/
      __HAL_RCC_OSPI_CONFIG(PeriphClkInit->OspiClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif  /*OCTOSPI*/

  /*---------------------------- SPI1/2/3 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPI123) == RCC_PERIPHCLK_SPI123)
  {
    switch(PeriphClkInit->Spi123ClockSelection)
    {
    case RCC_SPI123CLKSOURCE_PLL:      /* PLL is used as clock source for SPI1/2/3 */
      /* Enable SPI Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SPI1/2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI123CLKSOURCE_PLL2: /* PLL2 is used as clock source for SPI1/2/3 */
      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* SPI1/2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI123CLKSOURCE_PLL3:  /* PLL3 is used as clock source for SPI1/2/3 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_P_UPDATE);

      /* SPI1/2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI123CLKSOURCE_PIN:
      /* External clock is used as source of SPI1/2/3 clock*/
      /* SPI1/2/3 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI123CLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of SPI1/2/3 clock */
      /* SPI1/2/3 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SPI1/2/3 clock*/
      __HAL_RCC_SPI123_CONFIG(PeriphClkInit->Spi123ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- SPI4/5 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPI45) == RCC_PERIPHCLK_SPI45)
  {
    switch(PeriphClkInit->Spi45ClockSelection)
    {
    case RCC_SPI45CLKSOURCE_PCLK1:      /* CD/D2 PCLK1 as clock source for SPI4/5 */
      /* SPI4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI45CLKSOURCE_PLL2: /* PLL2 is used as clock source for SPI4/5 */

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);

      /* SPI4/5 clock source configuration done later after clock selection check */
      break;
    case RCC_SPI45CLKSOURCE_PLL3:  /* PLL3 is used as clock source for SPI4/5 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_Q_UPDATE);
      /* SPI4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI45CLKSOURCE_HSI:
      /* HSI oscillator clock is used as source of SPI4/5 clock*/
      /* SPI4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI45CLKSOURCE_CSI:
      /*  CSI oscillator clock is used as source of SPI4/5 clock */
      /* SPI4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI45CLKSOURCE_HSE:
      /* HSE,  oscillator is used as source of SPI4/5 clock */
      /* SPI4/5 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SPI4/5 clock*/
      __HAL_RCC_SPI45_CONFIG(PeriphClkInit->Spi45ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- SPI6 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPI6) == RCC_PERIPHCLK_SPI6)
  {
    switch(PeriphClkInit->Spi6ClockSelection)
    {
    case RCC_SPI6CLKSOURCE_PCLK4:      /* SRD/D3 PCLK1 (PCLK4) as clock source for SPI6*/
      /* SPI6 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI6CLKSOURCE_PLL2: /* PLL2 is used as clock source for SPI6*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);

      /* SPI6 clock source configuration done later after clock selection check */
      break;
    case RCC_SPI6CLKSOURCE_PLL3:  /* PLL3 is used as clock source for SPI6*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_Q_UPDATE);
      /* SPI6 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI6CLKSOURCE_HSI:
      /* HSI oscillator clock is used as source of SPI6 clock*/
      /* SPI6 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI6CLKSOURCE_CSI:
      /*  CSI oscillator clock is used as source of SPI6 clock */
      /* SPI6 clock source configuration done later after clock selection check */
      break;

    case RCC_SPI6CLKSOURCE_HSE:
      /* HSE,  oscillator is used as source of SPI6 clock */
      /* SPI6 clock source configuration done later after clock selection check */
      break;
#if defined(RCC_SPI6CLKSOURCE_PIN)
    case RCC_SPI6CLKSOURCE_PIN:
      /* 2S_CKIN is used as source of SPI6 clock */
      /* SPI6 clock source configuration done later after clock selection check */
      break;
#endif

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SPI6 clock*/
      __HAL_RCC_SPI6_CONFIG(PeriphClkInit->Spi6ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

#if defined(DSI)
  /*---------------------------- DSI configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_DSI) == RCC_PERIPHCLK_DSI)
  {
    switch(PeriphClkInit->DsiClockSelection)
    {

    case RCC_DSICLKSOURCE_PLL2: /* PLL2 is used as clock source for DSI*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);

      /* DSI clock source configuration done later after clock selection check */
      break;

    case RCC_DSICLKSOURCE_PHY:
      /* PHY is used as clock source for DSI*/
      /* DSI clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of DSI clock*/
      __HAL_RCC_DSI_CONFIG(PeriphClkInit->DsiClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif /*DSI*/

#if defined(FDCAN1) || defined(FDCAN2)
  /*---------------------------- FDCAN configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_FDCAN) == RCC_PERIPHCLK_FDCAN)
  {
    switch(PeriphClkInit->FdcanClockSelection)
    {
    case RCC_FDCANCLKSOURCE_PLL:      /* PLL is used as clock source for FDCAN*/
      /* Enable FDCAN Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* FDCAN clock source configuration done later after clock selection check */
      break;

    case RCC_FDCANCLKSOURCE_PLL2: /* PLL2 is used as clock source for FDCAN*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);

      /* FDCAN clock source configuration done later after clock selection check */
      break;

    case RCC_FDCANCLKSOURCE_HSE:
      /* HSE is used as clock source for FDCAN*/
      /* FDCAN clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of FDCAN clock*/
      __HAL_RCC_FDCAN_CONFIG(PeriphClkInit->FdcanClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
#endif /*FDCAN1 || FDCAN2*/

  /*---------------------------- FMC configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_FMC) == RCC_PERIPHCLK_FMC)
  {
    switch(PeriphClkInit->FmcClockSelection)
    {
    case RCC_FMCCLKSOURCE_PLL:      /* PLL is used as clock source for FMC*/
      /* Enable FMC Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* FMC clock source configuration done later after clock selection check */
      break;

    case RCC_FMCCLKSOURCE_PLL2: /* PLL2 is used as clock source for FMC*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_R_UPDATE);

      /* FMC clock source configuration done later after clock selection check */
      break;


    case RCC_FMCCLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of FMC clock */
      /* FMC clock source configuration done later after clock selection check */
      break;

    case RCC_FMCCLKSOURCE_HCLK:
      /* D1/CD HCLK  clock selected as FMC kernel peripheral clock */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of FMC clock*/
      __HAL_RCC_FMC_CONFIG(PeriphClkInit->FmcClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- RTC configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == RCC_PERIPHCLK_RTC)
  {
    /* check for RTC Parameters used to output RTCCLK */
    assert_param(IS_RCC_RTCCLKSOURCE(PeriphClkInit->RTCClockSelection));

    /* Enable write access to Backup domain */
    SET_BIT(PWR->CR1, PWR_CR1_DBP);

    /* Wait for Backup domain Write protection disable */
    tickstart = HAL_GetTick();

    while((PWR->CR1 & PWR_CR1_DBP) == 0U)
    {
      if((HAL_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
      {
        ret = HAL_TIMEOUT;
        break;
      }
    }

    if(ret == HAL_OK)
    {
      /* Reset the Backup domain only if the RTC Clock source selection is modified */
      if((RCC->BDCR & RCC_BDCR_RTCSEL) != (PeriphClkInit->RTCClockSelection & RCC_BDCR_RTCSEL))
      {
        /* Store the content of BDCR register before the reset of Backup Domain */
        tmpreg = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));
        /* RTC Clock selection can be changed only if the Backup Domain is reset */
        __HAL_RCC_BACKUPRESET_FORCE();
        __HAL_RCC_BACKUPRESET_RELEASE();
        /* Restore the Content of BDCR register */
        RCC->BDCR = tmpreg;
      }

      /* If LSE is selected as RTC clock source (and enabled prior to Backup Domain reset), wait for LSE reactivation */
      if(PeriphClkInit->RTCClockSelection == RCC_RTCCLKSOURCE_LSE)
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till LSE is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == 0U)
        {
          if((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
          {
            ret = HAL_TIMEOUT;
            break;
          }
        }
      }

      if(ret == HAL_OK)
      {
        __HAL_RCC_RTC_CONFIG(PeriphClkInit->RTCClockSelection);
      }
      else
      {
        /* set overall return value */
        status = ret;
      }
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }


  /*-------------------------- USART1/6 configuration --------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART16) == RCC_PERIPHCLK_USART16)
  {
    switch(PeriphClkInit->Usart16ClockSelection)
    {
    case RCC_USART16CLKSOURCE_PCLK2: /* CD/D2 PCLK2 as clock source for USART1/6 */
      /* USART1/6 clock source configuration done later after clock selection check */
      break;

    case RCC_USART16CLKSOURCE_PLL2: /* PLL2 is used as clock source for USART1/6 */
      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);
      /* USART1/6 clock source configuration done later after clock selection check */
      break;

    case RCC_USART16CLKSOURCE_PLL3: /* PLL3 is used as clock source for USART1/6 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_Q_UPDATE);
      /* USART1/6 clock source configuration done later after clock selection check */
      break;

    case RCC_USART16CLKSOURCE_HSI:
      /* HSI oscillator clock is used as source of USART1/6 clock */
      /* USART1/6 clock source configuration done later after clock selection check */
      break;

    case RCC_USART16CLKSOURCE_CSI:
      /* CSI oscillator clock is used as source of USART1/6 clock */
      /* USART1/6 clock source configuration done later after clock selection check */
      break;

    case RCC_USART16CLKSOURCE_LSE:
      /* LSE,  oscillator is used as source of USART1/6 clock */
      /* USART1/6 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of USART1/6 clock */
      __HAL_RCC_USART16_CONFIG(PeriphClkInit->Usart16ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*-------------------------- USART2/3/4/5/7/8 Configuration --------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USART234578) == RCC_PERIPHCLK_USART234578)
  {
    switch(PeriphClkInit->Usart234578ClockSelection)
    {
    case RCC_USART234578CLKSOURCE_PCLK1: /* CD/D2 PCLK1 as clock source for USART2/3/4/5/7/8 */
      /* USART2/3/4/5/7/8 clock source configuration done later after clock selection check */
      break;

    case RCC_USART234578CLKSOURCE_PLL2: /* PLL2 is used as clock source for USART2/3/4/5/7/8 */
      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);
      /* USART2/3/4/5/7/8 clock source configuration done later after clock selection check */
      break;

    case RCC_USART234578CLKSOURCE_PLL3: /* PLL3 is used as clock source for USART2/3/4/5/7/8 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_Q_UPDATE);
      /* USART2/3/4/5/7/8 clock source configuration done later after clock selection check */
      break;

    case RCC_USART234578CLKSOURCE_HSI:
      /* HSI oscillator clock is used as source of USART2/3/4/5/7/8 clock */
      /* USART2/3/4/5/7/8 clock source configuration done later after clock selection check */
      break;

    case RCC_USART234578CLKSOURCE_CSI:
      /* CSI oscillator clock is used as source of USART2/3/4/5/7/8 clock */
      /* USART2/3/4/5/7/8 clock source configuration done later after clock selection check */
      break;

    case RCC_USART234578CLKSOURCE_LSE:
      /* LSE,  oscillator is used as source of USART2/3/4/5/7/8 clock */
      /* USART2/3/4/5/7/8 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of USART2/3/4/5/7/8 clock */
      __HAL_RCC_USART234578_CONFIG(PeriphClkInit->Usart234578ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*-------------------------- LPUART1 Configuration -------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPUART1) == RCC_PERIPHCLK_LPUART1)
  {
    switch(PeriphClkInit->Lpuart1ClockSelection)
    {
    case RCC_LPUART1CLKSOURCE_PCLK4: /* SRD/D3 PCLK1 (PCLK4) as clock source for LPUART1 */
      /* LPUART1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPUART1CLKSOURCE_PLL2: /* PLL2 is used as clock source for LPUART1 */
      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_Q_UPDATE);
      /* LPUART1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPUART1CLKSOURCE_PLL3: /* PLL3 is used as clock source for LPUART1 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_Q_UPDATE);
      /* LPUART1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPUART1CLKSOURCE_HSI:
      /* HSI oscillator clock is used as source of LPUART1 clock */
      /* LPUART1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPUART1CLKSOURCE_CSI:
      /* CSI oscillator clock is used as source of LPUART1 clock */
      /* LPUART1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPUART1CLKSOURCE_LSE:
      /* LSE,  oscillator is used as source of LPUART1 clock */
      /* LPUART1 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of LPUART1 clock */
      __HAL_RCC_LPUART1_CONFIG(PeriphClkInit->Lpuart1ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- LPTIM1 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPTIM1) == RCC_PERIPHCLK_LPTIM1)
  {
    switch(PeriphClkInit->Lptim1ClockSelection)
    {
    case RCC_LPTIM1CLKSOURCE_PCLK1:      /* CD/D2 PCLK1 as clock source for LPTIM1*/
      /* LPTIM1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM1CLKSOURCE_PLL2: /* PLL2 is used as clock source for LPTIM1*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* LPTIM1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM1CLKSOURCE_PLL3:  /* PLL3 is used as clock source for LPTIM1*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE);

      /* LPTIM1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM1CLKSOURCE_LSE:
      /* External low speed OSC clock is used as source of LPTIM1 clock*/
      /* LPTIM1 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM1CLKSOURCE_LSI:
      /* Internal  low speed OSC clock is used  as source of LPTIM1 clock*/
      /* LPTIM1 clock source configuration done later after clock selection check */
      break;
    case RCC_LPTIM1CLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of LPTIM1 clock */
      /* LPTIM1 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of LPTIM1 clock*/
      __HAL_RCC_LPTIM1_CONFIG(PeriphClkInit->Lptim1ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- LPTIM2 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPTIM2) == RCC_PERIPHCLK_LPTIM2)
  {
    switch(PeriphClkInit->Lptim2ClockSelection)
    {
    case RCC_LPTIM2CLKSOURCE_PCLK4:      /* SRD/D3 PCLK1 (PCLK4) as clock source for LPTIM2*/
      /* LPTIM2 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM2CLKSOURCE_PLL2: /* PLL2 is used as clock source for LPTIM2*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* LPTIM2 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM2CLKSOURCE_PLL3:  /* PLL3 is used as clock source for LPTIM2*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE);

      /* LPTIM2 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM2CLKSOURCE_LSE:
      /* External low speed OSC clock is used as source of LPTIM2 clock*/
      /* LPTIM2 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM2CLKSOURCE_LSI:
      /* Internal  low speed OSC clock is used  as source of LPTIM2 clock*/
      /* LPTIM2 clock source configuration done later after clock selection check */
      break;
    case RCC_LPTIM2CLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of LPTIM2 clock */
      /* LPTIM2 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of LPTIM2 clock*/
      __HAL_RCC_LPTIM2_CONFIG(PeriphClkInit->Lptim2ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*---------------------------- LPTIM345 configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LPTIM345) == RCC_PERIPHCLK_LPTIM345)
  {
    switch(PeriphClkInit->Lptim345ClockSelection)
    {

    case RCC_LPTIM345CLKSOURCE_PCLK4:      /* SRD/D3 PCLK1 (PCLK4) as clock source for LPTIM3/4/5 */
      /* LPTIM3/4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM345CLKSOURCE_PLL2: /* PLL2 is used as clock source for LPTIM3/4/5 */
      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* LPTIM3/4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM345CLKSOURCE_PLL3:  /* PLL3 is used as clock source for LPTIM3/4/5 */
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE);

      /* LPTIM3/4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM345CLKSOURCE_LSE:
      /* External low speed OSC clock is used as source of LPTIM3/4/5 clock */
      /* LPTIM3/4/5 clock source configuration done later after clock selection check */
      break;

    case RCC_LPTIM345CLKSOURCE_LSI:
      /* Internal  low speed OSC clock is used  as source of LPTIM3/4/5 clock */
      /* LPTIM3/4/5 clock source configuration done later after clock selection check */
      break;
    case RCC_LPTIM345CLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of LPTIM3/4/5 clock */
      /* LPTIM3/4/5 clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of LPTIM3/4/5 clock */
      __HAL_RCC_LPTIM345_CONFIG(PeriphClkInit->Lptim345ClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*------------------------------ I2C1/2/3 Configuration ------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C123) == RCC_PERIPHCLK_I2C123)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C123CLKSOURCE(PeriphClkInit->I2c123ClockSelection));

    if ((PeriphClkInit->I2c123ClockSelection )== RCC_I2C123CLKSOURCE_PLL3 )
    {
        if(RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE)!= HAL_OK)
        {
          status = HAL_ERROR;
        }
    }

      __HAL_RCC_I2C123_CONFIG(PeriphClkInit->I2c123ClockSelection);

  }

  /*------------------------------ I2C4 Configuration ------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C4) == RCC_PERIPHCLK_I2C4)
  {
    /* Check the parameters */
    assert_param(IS_RCC_I2C4CLKSOURCE(PeriphClkInit->I2c4ClockSelection));

    if ((PeriphClkInit->I2c4ClockSelection) == RCC_I2C4CLKSOURCE_PLL3 )
    {
      if(RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE)!= HAL_OK)
      {
        status = HAL_ERROR;
      }
    }

      __HAL_RCC_I2C4_CONFIG(PeriphClkInit->I2c4ClockSelection);

  }

  /*---------------------------- ADC configuration -------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_ADC) == RCC_PERIPHCLK_ADC)
  {
    switch(PeriphClkInit->AdcClockSelection)
    {

    case RCC_ADCCLKSOURCE_PLL2: /* PLL2 is used as clock source for ADC*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_P_UPDATE);

      /* ADC clock source configuration done later after clock selection check */
      break;

    case RCC_ADCCLKSOURCE_PLL3:  /* PLL3 is used as clock source for ADC*/
      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE);

      /* ADC clock source configuration done later after clock selection check */
      break;

    case RCC_ADCCLKSOURCE_CLKP:
      /* HSI, HSE, or CSI oscillator is used as source of ADC clock */
      /* ADC clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of ADC clock*/
      __HAL_RCC_ADC_CONFIG(PeriphClkInit->AdcClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

  /*------------------------------ USB Configuration -------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_USB) == RCC_PERIPHCLK_USB)
  {

    switch(PeriphClkInit->UsbClockSelection)
    {
    case RCC_USBCLKSOURCE_PLL:      /* PLL is used as clock source for USB*/
      /* Enable USB Clock output generated form System USB . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* USB clock source configuration done later after clock selection check */
      break;

    case RCC_USBCLKSOURCE_PLL3: /* PLL3 is used as clock source for USB*/

      ret = RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_Q_UPDATE);

      /* USB clock source configuration done later after clock selection check */
      break;

    case RCC_USBCLKSOURCE_HSI48:
      /* HSI48 oscillator is used as source of USB clock */
      /* USB clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of USB clock*/
      __HAL_RCC_USB_CONFIG(PeriphClkInit->UsbClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }

  }

  /*------------------------------------- SDMMC Configuration ------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SDMMC) == RCC_PERIPHCLK_SDMMC)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SDMMC(PeriphClkInit->SdmmcClockSelection));

    switch(PeriphClkInit->SdmmcClockSelection)
    {
    case RCC_SDMMCCLKSOURCE_PLL:      /* PLL is used as clock source for SDMMC*/
      /* Enable SDMMC Clock output generated form System PLL . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* SDMMC clock source configuration done later after clock selection check */
      break;

    case RCC_SDMMCCLKSOURCE_PLL2: /* PLL2 is used as clock source for SDMMC*/

      ret = RCCEx_PLL2_Config(&(PeriphClkInit->PLL2),DIVIDER_R_UPDATE);

      /* SDMMC clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of SDMMC clock*/
      __HAL_RCC_SDMMC_CONFIG(PeriphClkInit->SdmmcClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }

#if defined(LTDC)
  /*-------------------------------------- LTDC Configuration -----------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == RCC_PERIPHCLK_LTDC)
  {
    if(RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE)!=HAL_OK)
    {
      status=HAL_ERROR;
    }
  }
#endif /* LTDC */

  /*------------------------------ RNG Configuration -------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RNG) == RCC_PERIPHCLK_RNG)
  {

    switch(PeriphClkInit->RngClockSelection)
    {
    case RCC_RNGCLKSOURCE_PLL:     /* PLL is used as clock source for RNG*/
      /* Enable RNG Clock output generated form System RNG . */
      __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);

      /* RNG clock source configuration done later after clock selection check */
      break;

    case RCC_RNGCLKSOURCE_LSE: /* LSE is used as clock source for RNG*/

      /* RNG clock source configuration done later after clock selection check */
      break;

    case RCC_RNGCLKSOURCE_LSI: /* LSI is used as clock source for RNG*/

      /* RNG clock source configuration done later after clock selection check */
      break;
    case RCC_RNGCLKSOURCE_HSI48:
      /* HSI48 oscillator is used as source of RNG clock */
      /* RNG clock source configuration done later after clock selection check */
      break;

    default:
      ret = HAL_ERROR;
      break;
    }

    if(ret == HAL_OK)
    {
      /* Set the source of RNG clock*/
      __HAL_RCC_RNG_CONFIG(PeriphClkInit->RngClockSelection);
    }
    else
    {
      /* set overall return value */
      status = ret;
    }

  }

  /*------------------------------ SWPMI1 Configuration ------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SWPMI1) == RCC_PERIPHCLK_SWPMI1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_SWPMI1CLKSOURCE(PeriphClkInit->Swpmi1ClockSelection));

    /* Configure the SWPMI1 interface clock source */
    __HAL_RCC_SWPMI1_CONFIG(PeriphClkInit->Swpmi1ClockSelection);
  }
#if defined(HRTIM)
  /*------------------------------ HRTIM1 clock Configuration ----------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_HRTIM1) == RCC_PERIPHCLK_HRTIM1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HRTIM1CLKSOURCE(PeriphClkInit->Hrtim1ClockSelection));

    /* Configure the HRTIM1 clock source */
    __HAL_RCC_HRTIM1_CONFIG(PeriphClkInit->Hrtim1ClockSelection);
  }
#endif  /*HRTIM*/
  /*------------------------------ DFSDM1 Configuration ------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_DFSDM1) == RCC_PERIPHCLK_DFSDM1)
  {
    /* Check the parameters */
    assert_param(IS_RCC_DFSDM1CLKSOURCE(PeriphClkInit->Dfsdm1ClockSelection));

    /* Configure the DFSDM1 interface clock source */
    __HAL_RCC_DFSDM1_CONFIG(PeriphClkInit->Dfsdm1ClockSelection);
  }

#if defined(DFSDM2_BASE)
  /*------------------------------ DFSDM2 Configuration ------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_DFSDM2) == RCC_PERIPHCLK_DFSDM2)
  {
    /* Check the parameters */
    assert_param(IS_RCC_DFSDM2CLKSOURCE(PeriphClkInit->Dfsdm2ClockSelection));

    /* Configure the DFSDM2 interface clock source */
    __HAL_RCC_DFSDM2_CONFIG(PeriphClkInit->Dfsdm2ClockSelection);
  }
#endif  /* DFSDM2 */

  /*------------------------------------ TIM configuration --------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_TIM) == RCC_PERIPHCLK_TIM)
  {
    /* Check the parameters */
    assert_param(IS_RCC_TIMPRES(PeriphClkInit->TIMPresSelection));

    /* Configure Timer Prescaler */
    __HAL_RCC_TIMCLKPRESCALER(PeriphClkInit->TIMPresSelection);
  }

  /*------------------------------------ CKPER configuration --------------------------------------*/
  if(((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_CKPER) == RCC_PERIPHCLK_CKPER)
  {
    /* Check the parameters */
    assert_param(IS_RCC_CLKPSOURCE(PeriphClkInit->CkperClockSelection));

    /* Configure the CKPER clock source */
    __HAL_RCC_CLKP_CONFIG(PeriphClkInit->CkperClockSelection);
  }

  if (status == HAL_OK)
  {
    return HAL_OK;
  }
  return HAL_ERROR;
}

/**
  * @brief  Get the RCC_ClkInitStruct according to the internal RCC configuration registers.
  * @param  PeriphClkInit: pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         returns the configuration information for the Extended Peripherals clocks :
  *         (SDMMC, CKPER, FMC, QSPI*, OSPI*, DSI*, SPI45, SPDIF, DFSDM1, DFSDM2*, FDCAN, SWPMI, SAI23*, SAI1, SPI123,
  *         USART234578, USART16, RNG, HRTIM1*, I2C123, USB, CEC, LPTIM1, LPUART1, I2C4, LPTIM2, LPTIM345, ADC.
  *         SAI4A*, SAI4B*, SPI6, RTC, TIM).
  * @retval None
  *
  *   (*) : Available on some STM32H7 lines only.
  */
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit)
{
  /* Set all possible values for the extended clock type parameter------------*/
  PeriphClkInit->PeriphClockSelection =
                 RCC_PERIPHCLK_USART16 | RCC_PERIPHCLK_USART234578 | RCC_PERIPHCLK_LPUART1 |
                 RCC_PERIPHCLK_I2C4    | RCC_PERIPHCLK_LPTIM1      | RCC_PERIPHCLK_LPTIM2  | RCC_PERIPHCLK_LPTIM345 |
                 RCC_PERIPHCLK_SAI1    | RCC_PERIPHCLK_SPI123      | RCC_PERIPHCLK_SPI45   | RCC_PERIPHCLK_SPI6     |
	         RCC_PERIPHCLK_FDCAN   | RCC_PERIPHCLK_SDMMC       | RCC_PERIPHCLK_RNG     | RCC_PERIPHCLK_USB      |
	         RCC_PERIPHCLK_ADC     | RCC_PERIPHCLK_SWPMI1      | RCC_PERIPHCLK_DFSDM1  | RCC_PERIPHCLK_RTC      |
	         RCC_PERIPHCLK_CEC     | RCC_PERIPHCLK_FMC         | RCC_PERIPHCLK_SPDIFRX | RCC_PERIPHCLK_TIM      |
	         RCC_PERIPHCLK_CKPER;

PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_I2C123;
#if defined(RCC_CDCCIP1R_SAI2ASEL)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_SAI2A;
#endif /* RCC_CDCCIP1R_SAI2ASEL */
#if defined(RCC_CDCCIP1R_SAI2BSEL)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_SAI2B;
#endif /* RCC_CDCCIP1R_SAI2BSEL */
#if defined(SAI3)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_SAI23;
#endif /* SAI3 */
#if defined(SAI4)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_SAI4A;
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_SAI4B;
#endif /* SAI4 */
#if defined(DFSDM2_BASE)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_DFSDM2;
#endif /* DFSDM2 */
#if defined(QUADSPI)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_QSPI;
#endif /* QUADSPI */
#if defined(OCTOSPI1) || defined(OCTOSPI2)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_OSPI;
#endif /* OCTOSPI1 || OCTOSPI2 */
#if defined(HRTIM)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_HRTIM1;
#endif /* HRTIM */
#if defined(LTDC)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_LTDC;
#endif /* LTDC */
#if defined(DSI)
  PeriphClkInit->PeriphClockSelection |= RCC_PERIPHCLK_DSI;
#endif /* DSI */

  /* Get the PLL3 Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLL3.PLL3M = (uint32_t)((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM3)>> RCC_PLLCKSELR_DIVM3_Pos);
  PeriphClkInit->PLL3.PLL3N = (uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_N3) >> RCC_PLL3DIVR_N3_Pos)+ 1U;
  PeriphClkInit->PLL3.PLL3R = (uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_R3) >> RCC_PLL3DIVR_R3_Pos)+ 1U;
  PeriphClkInit->PLL3.PLL3P = (uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_P3) >> RCC_PLL3DIVR_P3_Pos)+ 1U;
  PeriphClkInit->PLL3.PLL3Q = (uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_Q3) >> RCC_PLL3DIVR_Q3_Pos)+ 1U;
  PeriphClkInit->PLL3.PLL3RGE = (uint32_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLL3RGE) >> RCC_PLLCFGR_PLL3RGE_Pos);
  PeriphClkInit->PLL3.PLL3VCOSEL = (uint32_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLL3VCOSEL) >> RCC_PLLCFGR_PLL3VCOSEL_Pos);

  /* Get the PLL2 Clock configuration -----------------------------------------------*/
  PeriphClkInit->PLL2.PLL2M = (uint32_t)((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM2)>> RCC_PLLCKSELR_DIVM2_Pos);
  PeriphClkInit->PLL2.PLL2N = (uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_N2) >> RCC_PLL2DIVR_N2_Pos)+ 1U;
  PeriphClkInit->PLL2.PLL2R = (uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_R2) >> RCC_PLL2DIVR_R2_Pos)+ 1U;
  PeriphClkInit->PLL2.PLL2P = (uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_P2) >> RCC_PLL2DIVR_P2_Pos)+ 1U;
  PeriphClkInit->PLL2.PLL2Q = (uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_Q2) >> RCC_PLL2DIVR_Q2_Pos)+ 1U;
  PeriphClkInit->PLL2.PLL2RGE = (uint32_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLL2RGE) >> RCC_PLLCFGR_PLL2RGE_Pos);
  PeriphClkInit->PLL2.PLL2VCOSEL = (uint32_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLL2VCOSEL) >> RCC_PLLCFGR_PLL2VCOSEL_Pos);

  /* Get the USART1 configuration --------------------------------------------*/
  PeriphClkInit->Usart16ClockSelection      = __HAL_RCC_GET_USART16_SOURCE();
  /* Get the USART2/3/4/5/7/8 clock source -----------------------------------*/
  PeriphClkInit->Usart234578ClockSelection  = __HAL_RCC_GET_USART234578_SOURCE();
  /* Get the LPUART1 clock source --------------------------------------------*/
  PeriphClkInit->Lpuart1ClockSelection      = __HAL_RCC_GET_LPUART1_SOURCE();
  /* Get the I2C1/2/3 clock source -------------------------------------------*/
  PeriphClkInit->I2c123ClockSelection       = __HAL_RCC_GET_I2C1_SOURCE();
  /* Get the LPTIM1 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim1ClockSelection       = __HAL_RCC_GET_LPTIM1_SOURCE();
  /* Get the LPTIM2 clock source ---------------------------------------------*/
  PeriphClkInit->Lptim2ClockSelection       = __HAL_RCC_GET_LPTIM2_SOURCE();
  /* Get the LPTIM3/4/5 clock source -----------------------------------------*/
  PeriphClkInit->Lptim345ClockSelection     = __HAL_RCC_GET_LPTIM345_SOURCE();
  /* Get the SAI1 clock source -----------------------------------------------*/
  PeriphClkInit->Sai1ClockSelection         = __HAL_RCC_GET_SAI1_SOURCE();
#if defined(SAI3)
  /* Get the SAI2/3 clock source ---------------------------------------------*/
  PeriphClkInit->Sai23ClockSelection        = __HAL_RCC_GET_SAI23_SOURCE();
#endif  /*SAI3*/
#if defined(RCC_CDCCIP1R_SAI2ASEL_0)
  /* Get the SAI2A clock source ---------------------------------------------*/
  PeriphClkInit->Sai2AClockSelection        = __HAL_RCC_GET_SAI2A_SOURCE();
#endif  /*SAI2A*/
#if defined(RCC_CDCCIP1R_SAI2BSEL_0)
  /* Get the SAI2B clock source ---------------------------------------------*/
  PeriphClkInit->Sai2BClockSelection        = __HAL_RCC_GET_SAI2B_SOURCE();
#endif  /*SAI2B*/
#if defined(SAI4)
  /* Get the SAI4A clock source ----------------------------------------------*/
  PeriphClkInit->Sai4AClockSelection        = __HAL_RCC_GET_SAI4A_SOURCE();
  /* Get the SAI4B clock source ----------------------------------------------*/
  PeriphClkInit->Sai4BClockSelection        = __HAL_RCC_GET_SAI4B_SOURCE();
#endif  /*SAI4*/
  /* Get the RTC clock source ------------------------------------------------*/
  PeriphClkInit->RTCClockSelection          = __HAL_RCC_GET_RTC_SOURCE();
  /* Get the USB clock source ------------------------------------------------*/
  PeriphClkInit->UsbClockSelection          = __HAL_RCC_GET_USB_SOURCE();
  /* Get the SDMMC clock source ----------------------------------------------*/
  PeriphClkInit->SdmmcClockSelection        = __HAL_RCC_GET_SDMMC_SOURCE();
  /* Get the RNG clock source ------------------------------------------------*/
  PeriphClkInit->RngClockSelection          = __HAL_RCC_GET_RNG_SOURCE();
#if defined(HRTIM1)
  /* Get the HRTIM1 clock source ---------------------------------------------*/
  PeriphClkInit->Hrtim1ClockSelection       = __HAL_RCC_GET_HRTIM1_SOURCE();
#endif /* HRTIM1 */
  /* Get the ADC clock source ------------------------------------------------*/
  PeriphClkInit->AdcClockSelection          = __HAL_RCC_GET_ADC_SOURCE();
  /* Get the SWPMI1 clock source ---------------------------------------------*/
  PeriphClkInit->Swpmi1ClockSelection       = __HAL_RCC_GET_SWPMI1_SOURCE();
  /* Get the DFSDM1 clock source ---------------------------------------------*/
  PeriphClkInit->Dfsdm1ClockSelection       = __HAL_RCC_GET_DFSDM1_SOURCE();
#if defined(DFSDM2_BASE)
  /* Get the DFSDM2 clock source ---------------------------------------------*/
  PeriphClkInit->Dfsdm2ClockSelection       = __HAL_RCC_GET_DFSDM2_SOURCE();
#endif /* DFSDM2 */
  /* Get the SPDIFRX clock source --------------------------------------------*/
  PeriphClkInit->SpdifrxClockSelection      = __HAL_RCC_GET_SPDIFRX_SOURCE();
  /* Get the SPI1/2/3 clock source -------------------------------------------*/
  PeriphClkInit->Spi123ClockSelection       = __HAL_RCC_GET_SPI123_SOURCE();
  /* Get the SPI4/5 clock source ---------------------------------------------*/
  PeriphClkInit->Spi45ClockSelection        = __HAL_RCC_GET_SPI45_SOURCE();
  /* Get the SPI6 clock source -----------------------------------------------*/
  PeriphClkInit->Spi6ClockSelection         = __HAL_RCC_GET_SPI6_SOURCE();
  /* Get the FDCAN clock source ----------------------------------------------*/
  PeriphClkInit->FdcanClockSelection        = __HAL_RCC_GET_FDCAN_SOURCE();
  /* Get the CEC clock source ------------------------------------------------*/
  PeriphClkInit->CecClockSelection          = __HAL_RCC_GET_CEC_SOURCE();
  /* Get the FMC clock source ------------------------------------------------*/
  PeriphClkInit->FmcClockSelection          = __HAL_RCC_GET_FMC_SOURCE();
#if defined(QUADSPI)
  /* Get the QSPI clock source -----------------------------------------------*/
  PeriphClkInit->QspiClockSelection         = __HAL_RCC_GET_QSPI_SOURCE();
#endif /* QUADSPI */
#if defined(OCTOSPI1) || defined(OCTOSPI2)
  /* Get the OSPI clock source -----------------------------------------------*/
  PeriphClkInit->OspiClockSelection         = __HAL_RCC_GET_OSPI_SOURCE();
#endif /* OCTOSPI1 || OCTOSPI2 */

#if defined(DSI)
  /* Get the DSI clock source ------------------------------------------------*/
  PeriphClkInit->DsiClockSelection          = __HAL_RCC_GET_DSI_SOURCE();
#endif /*DSI*/

  /* Get the CKPER clock source ----------------------------------------------*/
  PeriphClkInit->CkperClockSelection        = __HAL_RCC_GET_CLKP_SOURCE();

  /* Get the TIM Prescaler configuration -------------------------------------*/
  if ((RCC->CFGR & RCC_CFGR_TIMPRE) == 0U)
  {
    PeriphClkInit->TIMPresSelection = RCC_TIMPRES_DESACTIVATED;
  }
  else
  {
    PeriphClkInit->TIMPresSelection = RCC_TIMPRES_ACTIVATED;
  }
}

/**
  * @brief  Return the peripheral clock frequency for a given peripheral(SAI..)
  * @note   Return 0 if peripheral clock identifier not managed by this API
  * @param  PeriphClk: Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg RCC_PERIPHCLK_SAI1  : SAI1 peripheral clock
  *            @arg RCC_PERIPHCLK_SAI23 : SAI2/3  peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI2A : SAI2A peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI2B : SAI2B peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI4A : SAI4A peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI4B : SAI4B peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SPI123: SPI1/2/3 peripheral clock
  *            @arg RCC_PERIPHCLK_ADC   : ADC peripheral clock
  *            @arg RCC_PERIPHCLK_SDMMC : SDMMC peripheral clock
  *            @arg RCC_PERIPHCLK_SPI6  : SPI6 peripheral clock
  * @retval Frequency in KHz
  *
  *  (*) : Available on some STM32H7 lines only.
  */
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk)
{
  PLL1_ClocksTypeDef pll1_clocks;
  PLL2_ClocksTypeDef pll2_clocks;
  PLL3_ClocksTypeDef pll3_clocks;

  /* This variable is used to store the SAI clock frequency (value in Hz) */
  uint32_t frequency;
  /* This variable is used to store the SAI and CKP clock source */
  uint32_t saiclocksource;
  uint32_t ckpclocksource;
  uint32_t srcclk;

  if (PeriphClk == RCC_PERIPHCLK_SAI1)
    {

      saiclocksource= __HAL_RCC_GET_SAI1_SOURCE();

      switch (saiclocksource)
      {
      case RCC_SAI1CLKSOURCE_PLL: /* PLL1 is the clock source for SAI1 */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_SAI1CLKSOURCE_PLL2: /* PLL2 is the clock source for SAI1 */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_SAI1CLKSOURCE_PLL3: /* PLL3 is the clock source for SAI1 */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_SAI1CLKSOURCE_CLKP: /* CKPER is the clock source for SAI1*/
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      case (RCC_SAI1CLKSOURCE_PIN): /* External clock is the clock source for SAI1 */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }
      default :
        {
          frequency = 0;
          break;
        }
      }
    }

#if defined(SAI3)
  else if (PeriphClk == RCC_PERIPHCLK_SAI23)
    {

      saiclocksource= __HAL_RCC_GET_SAI23_SOURCE();

      switch (saiclocksource)
      {
      case 0: /* PLL1 is the clock source for SAI2/3 */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_SAI23CLKSOURCE_PLL2: /* PLL2 is the clock source for SAI2/3 */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_SAI23CLKSOURCE_PLL3: /* PLL3 is the clock source for SAI2/3 */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_SAI23CLKSOURCE_CLKP: /* CKPER is the clock source for SAI2/3 */
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      case (RCC_SAI23CLKSOURCE_PIN): /* External clock is the clock source for SAI2/3 */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }
      default :
        {
          frequency = 0;
          break;
        }
      }
    }
#endif /* SAI3 */

#if  defined(RCC_CDCCIP1R_SAI2ASEL)

    else if (PeriphClk == RCC_PERIPHCLK_SAI2A)
    {
      saiclocksource= __HAL_RCC_GET_SAI2A_SOURCE();

      switch (saiclocksource)
      {
      case 0: /* PLL1 is the clock source for SAI2A */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_SAI2ACLKSOURCE_PLL2: /* PLLI2 is the clock source for SAI2A */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_SAI2ACLKSOURCE_PLL3: /* PLLI3 is the clock source for SAI2A  */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_SAI2ACLKSOURCE_CLKP: /* CKPER is the clock source for SAI2A  */
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      case (RCC_SAI2ACLKSOURCE_PIN): /* External clock is the clock source for SAI2A */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }

      default :
        {
          frequency = 0;
          break;
        }
      }

    }
#endif

#if  defined(RCC_CDCCIP1R_SAI2BSEL_0)
  else if (PeriphClk == RCC_PERIPHCLK_SAI2B)
    {

      saiclocksource= __HAL_RCC_GET_SAI2B_SOURCE();

      switch (saiclocksource)
      {
      case 0: /* PLL1 is the clock source for SAI2B */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_SAI2BCLKSOURCE_PLL2: /* PLLI2 is the clock source for SAI2B */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_SAI2BCLKSOURCE_PLL3: /* PLLI3 is the clock source for SAI2B */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_SAI2BCLKSOURCE_CLKP: /* CKPER is the clock source for SAI2B*/
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }
          break;
        }

      case (RCC_SAI2BCLKSOURCE_PIN): /* External clock is the clock source for SAI2B */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }

      default :
        {
          frequency = 0;
          break;
        }
      }
    }
#endif

#if defined(SAI4)
  else if (PeriphClk == RCC_PERIPHCLK_SAI4A)
    {

      saiclocksource= __HAL_RCC_GET_SAI4A_SOURCE();

      switch (saiclocksource)
      {
      case 0: /* PLL1 is the clock source for SAI4A */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_D3CCIPR_SAI4ASEL_0: /* PLLI2 is the clock source for SAI4A */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_D3CCIPR_SAI4ASEL_1: /* PLLI3 is the clock source for SAI4A */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_D3CCIPR_SAI4ASEL_2: /* CKPER is the clock source for SAI4A*/
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      case (RCC_D3CCIPR_SAI4ASEL_0 | RCC_D3CCIPR_SAI4ASEL_1 ): /* External clock is the clock source for SAI4A */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }

      default :
        {
          frequency = 0;
          break;
        }
      }
    }

  else if (PeriphClk == RCC_PERIPHCLK_SAI4B)
    {

      saiclocksource= __HAL_RCC_GET_SAI4B_SOURCE();

      switch (saiclocksource)
      {
      case 0: /* PLL1 is the clock source for SAI4B */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_D3CCIPR_SAI4BSEL_0: /* PLLI2 is the clock source for SAI4B */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_D3CCIPR_SAI4BSEL_1: /* PLLI3 is the clock source for SAI4B */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_D3CCIPR_SAI4BSEL_2: /* CKPER is the clock source for SAI4B*/
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      case (RCC_D3CCIPR_SAI4BSEL_0 | RCC_D3CCIPR_SAI4BSEL_1 ): /* External clock is the clock source for SAI4B */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }

      default :
        {
          frequency = 0;
          break;
        }
      }
    }
#endif /*SAI4*/
  else if (PeriphClk == RCC_PERIPHCLK_SPI123)
    {
      /* Get SPI1/2/3 clock source */
      srcclk= __HAL_RCC_GET_SPI123_SOURCE();

      switch (srcclk)
      {
      case RCC_SPI123CLKSOURCE_PLL: /* PLL1 is the clock source for I2S */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_SPI123CLKSOURCE_PLL2: /* PLL2 is the clock source for I2S */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }

      case RCC_SPI123CLKSOURCE_PLL3: /* PLL3 is the clock source for I2S */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_P_Frequency;
          break;
        }

      case RCC_SPI123CLKSOURCE_CLKP: /* CKPER is the clock source for I2S */
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      case (RCC_SPI123CLKSOURCE_PIN): /* External clock is the clock source for I2S */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }
      default :
        {
          frequency = 0;
          break;
        }
      }
    }
  else if (PeriphClk == RCC_PERIPHCLK_ADC)
    {
      /* Get ADC clock source */
      srcclk= __HAL_RCC_GET_ADC_SOURCE();

      switch (srcclk)
      {
      case RCC_ADCCLKSOURCE_PLL2:
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_P_Frequency;
          break;
        }
      case RCC_ADCCLKSOURCE_PLL3:
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_R_Frequency;
          break;
        }

      case RCC_ADCCLKSOURCE_CLKP:
        {

          ckpclocksource= __HAL_RCC_GET_CLKP_SOURCE();

          if(ckpclocksource== RCC_CLKPSOURCE_HSI)
          {
            /* In Case the CKPER Source is HSI */
            frequency = HSI_VALUE;
          }

          else if(ckpclocksource== RCC_CLKPSOURCE_CSI)
          {
            /* In Case the CKPER Source is CSI */
            frequency = CSI_VALUE;
          }

          else if (ckpclocksource== RCC_CLKPSOURCE_HSE)
          {
            /* In Case the CKPER Source is HSE */
            frequency = HSE_VALUE;
          }

          else
          {
            /* In Case the CKPER is disabled*/
            frequency = 0;
          }

          break;
        }

      default :
        {
          frequency = 0;
          break;
        }
      }
    }
  else if (PeriphClk == RCC_PERIPHCLK_SDMMC)
    {
      /* Get SDMMC clock source */
      srcclk= __HAL_RCC_GET_SDMMC_SOURCE();

      switch (srcclk)
      {
      case RCC_SDMMCCLKSOURCE_PLL: /* PLL1 is the clock source for SDMMC */
        {
          HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
          frequency = pll1_clocks.PLL1_Q_Frequency;
          break;
        }
      case RCC_SDMMCCLKSOURCE_PLL2: /* PLL2 is the clock source for SDMMC */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_R_Frequency;
          break;
        }

      default :
        {
          frequency = 0;
          break;
        }
      }
    }
  else if (PeriphClk == RCC_PERIPHCLK_SPI6)
    {
      /* Get SPI6 clock source */
      srcclk= __HAL_RCC_GET_SPI6_SOURCE();

      switch (srcclk)
      {
      case RCC_SPI6CLKSOURCE_D3PCLK1: /* D3PCLK1 (PCLK4) is the clock source for SPI6 */
        {
          frequency = HAL_RCCEx_GetD3PCLK1Freq();
          break;
        }
      case RCC_SPI6CLKSOURCE_PLL2: /* PLL2 is the clock source for SPI6 */
        {
          HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
          frequency = pll2_clocks.PLL2_Q_Frequency;
          break;
        }
      case RCC_SPI6CLKSOURCE_PLL3: /* PLL3 is the clock source for SPI6 */
        {
          HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
          frequency = pll3_clocks.PLL3_Q_Frequency;
          break;
        }
      case RCC_SPI6CLKSOURCE_HSI: /* HSI is the clock source for SPI6 */
        {
          frequency = HSI_VALUE;
          break;
        }
      case RCC_SPI6CLKSOURCE_CSI: /* CSI is the clock source for SPI6 */
        {
          frequency = CSI_VALUE;
          break;
        }
      case RCC_SPI6CLKSOURCE_HSE: /* HSE is the clock source for SPI6 */
        {
          frequency = HSE_VALUE;
          break;
        }
#if defined(RCC_SPI6CLKSOURCE_PIN)
      case RCC_SPI6CLKSOURCE_PIN: /* External clock is the clock source for SPI6 */
        {
          frequency = EXTERNAL_CLOCK_VALUE;
          break;
        }
#endif /* RCC_SPI6CLKSOURCE_PIN */
      default :
        {
          frequency = 0;
          break;
        }
      }
    }
  else
    {
      frequency = 0;
    }

  return frequency;
}


/**
  * @brief  Returns the D1PCLK1 frequency
  * @note   Each time D1PCLK1 changes, this function must be called to update the
  *         right D1PCLK1 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval D1PCLK1 frequency
  */
uint32_t HAL_RCCEx_GetD1PCLK1Freq(void)
{
#if defined(RCC_D1CFGR_D1PPRE)
  /* Get HCLK source and Compute D1PCLK1 frequency ---------------------------*/
  return (HAL_RCC_GetHCLKFreq() >> (D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1PPRE)>> RCC_D1CFGR_D1PPRE_Pos] & 0x1FU));
#else
/* Get HCLK source and Compute D1PCLK1 frequency ---------------------------*/
  return (HAL_RCC_GetHCLKFreq() >> (D1CorePrescTable[(RCC->CDCFGR1 & RCC_CDCFGR1_CDPPRE)>> RCC_CDCFGR1_CDPPRE_Pos] & 0x1FU));
#endif
}

/**
  * @brief  Returns the D3PCLK1 frequency
  * @note   Each time D3PCLK1 changes, this function must be called to update the
  *         right D3PCLK1 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval D3PCLK1 frequency
  */
uint32_t HAL_RCCEx_GetD3PCLK1Freq(void)
{
#if defined(RCC_D3CFGR_D3PPRE)
  /* Get HCLK source and Compute D3PCLK1 frequency ---------------------------*/
  return (HAL_RCC_GetHCLKFreq() >> (D1CorePrescTable[(RCC->D3CFGR & RCC_D3CFGR_D3PPRE)>> RCC_D3CFGR_D3PPRE_Pos] & 0x1FU));
#else
  /* Get HCLK source and Compute D3PCLK1 frequency ---------------------------*/
  return (HAL_RCC_GetHCLKFreq() >> (D1CorePrescTable[(RCC->SRDCFGR & RCC_SRDCFGR_SRDPPRE)>> RCC_SRDCFGR_SRDPPRE_Pos] & 0x1FU));
#endif
}
/**
* @brief  Returns the PLL2 clock frequencies :PLL2_P_Frequency,PLL2_R_Frequency and PLL2_Q_Frequency
  * @note   The PLL2 clock frequencies computed by this function is not the real
  *         frequency in the chip. It is calculated based on the predefined
  *         constant and the selected clock source:
  * @note     The function returns values based on HSE_VALUE, HSI_VALUE or CSI Value multiplied/divided by the PLL factors.
  * @note   This function can be used by the user application to compute the
  *         baud-rate for the communication peripherals or configure other parameters.
  *
  * @note   Each time PLL2CLK changes, this function must be called to update the
  *         right PLL2CLK value. Otherwise, any configuration based on this function will be incorrect.
  * @param  PLL2_Clocks structure.
  * @retval None
  */
void HAL_RCCEx_GetPLL2ClockFreq(PLL2_ClocksTypeDef* PLL2_Clocks)
{
  uint32_t  pllsource, pll2m,  pll2fracen, hsivalue;
  float_t fracn2, pll2vco;

  /* PLL_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLL2M) * PLL2N
     PLL2xCLK = PLL2_VCO / PLL2x
  */
  pllsource = (RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC);
  pll2m = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM2)>> 12);
  pll2fracen = (RCC->PLLCFGR & RCC_PLLCFGR_PLL2FRACEN) >> RCC_PLLCFGR_PLL2FRACEN_Pos;
  fracn2 =(float_t)(uint32_t)(pll2fracen* ((RCC->PLL2FRACR & RCC_PLL2FRACR_FRACN2)>> 3));

  if (pll2m != 0U)
  {
    switch (pllsource)
    {

    case RCC_PLLSOURCE_HSI:  /* HSI used as PLL clock source */

      if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U)
      {
        hsivalue = (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        pll2vco = ( (float_t)hsivalue / (float_t)pll2m) * ((float_t)(uint32_t)(RCC->PLL2DIVR & RCC_PLL2DIVR_N2) + (fracn2/(float_t)0x2000) +(float_t)1 );
      }
      else
      {
        pll2vco = ((float_t)HSI_VALUE / (float_t)pll2m) * ((float_t)(uint32_t)(RCC->PLL2DIVR & RCC_PLL2DIVR_N2) + (fracn2/(float_t)0x2000) +(float_t)1 );
      }
      break;

    case RCC_PLLSOURCE_CSI:  /* CSI used as PLL clock source */
      pll2vco = ((float_t)CSI_VALUE / (float_t)pll2m) * ((float_t)(uint32_t)(RCC->PLL2DIVR & RCC_PLL2DIVR_N2) + (fracn2/(float_t)0x2000) +(float_t)1 );
      break;

    case RCC_PLLSOURCE_HSE:  /* HSE used as PLL clock source */
      pll2vco = ((float_t)HSE_VALUE / (float_t)pll2m) * ((float_t)(uint32_t)(RCC->PLL2DIVR & RCC_PLL2DIVR_N2) + (fracn2/(float_t)0x2000) +(float_t)1 );
      break;

    default:
      pll2vco = ((float_t)CSI_VALUE / (float_t)pll2m) * ((float_t)(uint32_t)(RCC->PLL2DIVR & RCC_PLL2DIVR_N2) + (fracn2/(float_t)0x2000) +(float_t)1 );
      break;
    }
    PLL2_Clocks->PLL2_P_Frequency = (uint32_t)(float_t)(pll2vco/((float_t)(uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_P2) >>9)  + (float_t)1 )) ;
    PLL2_Clocks->PLL2_Q_Frequency = (uint32_t)(float_t)(pll2vco/((float_t)(uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_Q2) >>16) + (float_t)1 )) ;
    PLL2_Clocks->PLL2_R_Frequency = (uint32_t)(float_t)(pll2vco/((float_t)(uint32_t)((RCC->PLL2DIVR & RCC_PLL2DIVR_R2) >>24) + (float_t)1 )) ;
  }
  else
  {
    PLL2_Clocks->PLL2_P_Frequency = 0U;
    PLL2_Clocks->PLL2_Q_Frequency = 0U;
    PLL2_Clocks->PLL2_R_Frequency = 0U;
  }
}

/**
* @brief  Returns the PLL3 clock frequencies :PLL3_P_Frequency,PLL3_R_Frequency and PLL3_Q_Frequency
  * @note   The PLL3 clock frequencies computed by this function is not the real
  *         frequency in the chip. It is calculated based on the predefined
  *         constant and the selected clock source:
  * @note     The function returns values based on HSE_VALUE, HSI_VALUE or CSI Value multiplied/divided by the PLL factors.
  * @note   This function can be used by the user application to compute the
  *         baud-rate for the communication peripherals or configure other parameters.
  *
  * @note   Each time PLL3CLK changes, this function must be called to update the
  *         right PLL3CLK value. Otherwise, any configuration based on this function will be incorrect.
  * @param  PLL3_Clocks structure.
  * @retval None
  */
void HAL_RCCEx_GetPLL3ClockFreq(PLL3_ClocksTypeDef* PLL3_Clocks)
{
  uint32_t pllsource, pll3m, pll3fracen, hsivalue;
  float_t fracn3, pll3vco;

  /* PLL3_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLL3M) * PLL3N
     PLL3xCLK = PLL3_VCO / PLLxR
  */
  pllsource = (RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC);
  pll3m = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM3)>> 20)  ;
  pll3fracen = (RCC->PLLCFGR & RCC_PLLCFGR_PLL3FRACEN) >> RCC_PLLCFGR_PLL3FRACEN_Pos;
  fracn3 = (float_t)(uint32_t)(pll3fracen* ((RCC->PLL3FRACR & RCC_PLL3FRACR_FRACN3)>> 3));

  if (pll3m != 0U)
  {
    switch (pllsource)
    {
    case RCC_PLLSOURCE_HSI:  /* HSI used as PLL clock source */

      if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U)
      {
        hsivalue = (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        pll3vco = ((float_t)hsivalue / (float_t)pll3m) * ((float_t)(uint32_t)(RCC->PLL3DIVR & RCC_PLL3DIVR_N3) + (fracn3/(float_t)0x2000) +(float_t)1 );
      }
      else
      {
        pll3vco = ((float_t)HSI_VALUE / (float_t)pll3m) * ((float_t)(uint32_t)(RCC->PLL3DIVR & RCC_PLL3DIVR_N3) + (fracn3/(float_t)0x2000) +(float_t)1 );
      }
      break;
    case RCC_PLLSOURCE_CSI:  /* CSI used as PLL clock source */
      pll3vco = ((float_t)CSI_VALUE / (float_t)pll3m) * ((float_t)(uint32_t)(RCC->PLL3DIVR & RCC_PLL3DIVR_N3) + (fracn3/(float_t)0x2000) +(float_t)1 );
      break;

    case RCC_PLLSOURCE_HSE:  /* HSE used as PLL clock source */
      pll3vco = ((float_t)HSE_VALUE / (float_t)pll3m) * ((float_t)(uint32_t)(RCC->PLL3DIVR & RCC_PLL3DIVR_N3) + (fracn3/(float_t)0x2000) +(float_t)1 );
      break;

    default:
      pll3vco = ((float_t)CSI_VALUE / (float_t)pll3m) * ((float_t)(uint32_t)(RCC->PLL3DIVR & RCC_PLL3DIVR_N3) + (fracn3/(float_t)0x2000) +(float_t)1 );
      break;
    }
    PLL3_Clocks->PLL3_P_Frequency = (uint32_t)(float_t)(pll3vco/((float_t)(uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_P3) >>9)  + (float_t)1 )) ;
    PLL3_Clocks->PLL3_Q_Frequency = (uint32_t)(float_t)(pll3vco/((float_t)(uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_Q3) >>16) + (float_t)1 )) ;
    PLL3_Clocks->PLL3_R_Frequency = (uint32_t)(float_t)(pll3vco/((float_t)(uint32_t)((RCC->PLL3DIVR & RCC_PLL3DIVR_R3) >>24) + (float_t)1 )) ;
  }
  else
  {
    PLL3_Clocks->PLL3_P_Frequency = 0U;
    PLL3_Clocks->PLL3_Q_Frequency = 0U;
    PLL3_Clocks->PLL3_R_Frequency = 0U;
  }

}

/**
* @brief  Returns the PLL1 clock frequencies :PLL1_P_Frequency,PLL1_R_Frequency and PLL1_Q_Frequency
  * @note   The PLL1 clock frequencies computed by this function is not the real
  *         frequency in the chip. It is calculated based on the predefined
  *         constant and the selected clock source:
  * @note     The function returns values based on HSE_VALUE, HSI_VALUE or CSI Value multiplied/divided by the PLL factors.
  * @note   This function can be used by the user application to compute the
  *         baud-rate for the communication peripherals or configure other parameters.
  *
  * @note   Each time PLL1CLK changes, this function must be called to update the
  *         right PLL1CLK value. Otherwise, any configuration based on this function will be incorrect.
  * @param  PLL1_Clocks structure.
  * @retval None
  */
void HAL_RCCEx_GetPLL1ClockFreq(PLL1_ClocksTypeDef* PLL1_Clocks)
{
  uint32_t pllsource, pll1m, pll1fracen, hsivalue;
  float_t fracn1, pll1vco;

  pllsource = (RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC);
  pll1m = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM1)>> 4);
  pll1fracen = RCC->PLLCFGR & RCC_PLLCFGR_PLL1FRACEN;
  fracn1 = (float_t)(uint32_t)(pll1fracen * ((RCC->PLL1FRACR & RCC_PLL1FRACR_FRACN1)>> 3));

  if (pll1m != 0U)
  {
    switch (pllsource)
    {

    case RCC_PLLSOURCE_HSI:  /* HSI used as PLL clock source */

      if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U)
      {
        hsivalue = (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        pll1vco = ((float_t)hsivalue / (float_t)pll1m) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
      }
      else
      {
        pll1vco = ((float_t)HSI_VALUE / (float_t)pll1m) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
      }
      break;
    case RCC_PLLSOURCE_CSI:  /* CSI used as PLL clock source */
      pll1vco = ((float_t)CSI_VALUE / (float_t)pll1m) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
      break;

    case RCC_PLLSOURCE_HSE:  /* HSE used as PLL clock source */
      pll1vco = ((float_t)HSE_VALUE / (float_t)pll1m) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
      break;

    default:
      pll1vco = ((float_t)CSI_VALUE / (float_t)pll1m) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
      break;
    }

    PLL1_Clocks->PLL1_P_Frequency = (uint32_t)(float_t)(pll1vco/((float_t)(uint32_t)((RCC->PLL1DIVR & RCC_PLL1DIVR_P1) >>9)  + (float_t)1 )) ;
    PLL1_Clocks->PLL1_Q_Frequency = (uint32_t)(float_t)(pll1vco/((float_t)(uint32_t)((RCC->PLL1DIVR & RCC_PLL1DIVR_Q1) >>16) + (float_t)1 )) ;
    PLL1_Clocks->PLL1_R_Frequency = (uint32_t)(float_t)(pll1vco/((float_t)(uint32_t)((RCC->PLL1DIVR & RCC_PLL1DIVR_R1) >>24) + (float_t)1 )) ;
  }
  else
  {
    PLL1_Clocks->PLL1_P_Frequency = 0U;
    PLL1_Clocks->PLL1_Q_Frequency = 0U;
    PLL1_Clocks->PLL1_R_Frequency = 0U;
  }

}

/**
  * @brief  Returns the main System frequency
  * @note   Each time System clock changes, this function must be called to update the
  *         right core clock value. Otherwise, any configuration based on this function will be incorrect.
  * @note   The SystemCoreClock CMSIS variable is used to store System current Core Clock Frequency
  *         and updated within this function
  * @retval HCLK frequency
  */
uint32_t HAL_RCCEx_GetD1SysClockFreq(void)
{
uint32_t common_system_clock;

#if defined(RCC_D1CFGR_D1CPRE)
  common_system_clock = HAL_RCC_GetSysClockFreq() >> (D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE)>> RCC_D1CFGR_D1CPRE_Pos] & 0x1FU);
#else
  common_system_clock = HAL_RCC_GetSysClockFreq() >> (D1CorePrescTable[(RCC->CDCFGR1 & RCC_CDCFGR1_CDCPRE)>> RCC_CDCFGR1_CDCPRE_Pos] & 0x1FU);
#endif

  /* Update the SystemD2Clock global variable */
#if defined(RCC_D1CFGR_HPRE)
  SystemD2Clock = (common_system_clock >> ((D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_HPRE)>> RCC_D1CFGR_HPRE_Pos]) & 0x1FU));
#else
  SystemD2Clock = (common_system_clock >> ((D1CorePrescTable[(RCC->CDCFGR1 & RCC_CDCFGR1_HPRE)>> RCC_CDCFGR1_HPRE_Pos]) & 0x1FU));
#endif

#if defined(DUAL_CORE) && defined(CORE_CM4)
  SystemCoreClock = SystemD2Clock;
#else
  SystemCoreClock = common_system_clock;
#endif /* DUAL_CORE && CORE_CM4 */

  return common_system_clock;
}
/**
  * @}
  */

/** @defgroup RCCEx_Exported_Functions_Group2 Extended System Control functions
 *  @brief  Extended Peripheral Control functions
  * @{
  */
/**
  * @brief  Enables the LSE Clock Security System.
  * @note   Prior to enable the LSE Clock Security System, LSE oscillator is to be enabled
  *         with HAL_RCC_OscConfig() and the LSE oscillator clock is to be selected as RTC
  *         clock with HAL_RCCEx_PeriphCLKConfig().
  * @retval None
  */
void HAL_RCCEx_EnableLSECSS(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_LSECSSON) ;
}

/**
  * @brief  Disables the LSE Clock Security System.
  * @note   LSE Clock Security System can only be disabled after a LSE failure detection.
  * @retval None
  */
void HAL_RCCEx_DisableLSECSS(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSECSSON) ;
  /* Disable LSE CSS IT if any */
  __HAL_RCC_DISABLE_IT(RCC_IT_LSECSS);
}

/**
  * @brief  Enable the LSE Clock Security System Interrupt & corresponding EXTI line.
  * @note   LSE Clock Security System Interrupt is mapped on EXTI line 18
  * @retval None
  */
void HAL_RCCEx_EnableLSECSS_IT(void)
{
  /* Enable LSE CSS */
  SET_BIT(RCC->BDCR, RCC_BDCR_LSECSSON) ;

  /* Enable LSE CSS IT */
  __HAL_RCC_ENABLE_IT(RCC_IT_LSECSS);

  /* Enable IT on EXTI Line 18 */
#if defined(DUAL_CORE) && defined(CORE_CM4)
  __HAL_RCC_C2_LSECSS_EXTI_ENABLE_IT();
#else
  __HAL_RCC_LSECSS_EXTI_ENABLE_IT();
#endif /* DUAL_CORE && CORE_CM4 */
  __HAL_RCC_LSECSS_EXTI_ENABLE_RISING_EDGE();
}

/**
  * @brief  Configure the oscillator clock source for wakeup from Stop and CSS backup clock
  * @param  WakeUpClk: Wakeup clock
  *         This parameter can be one of the following values:
  *            @arg RCC_STOP_WAKEUPCLOCK_CSI: CSI oscillator selection
  *            @arg RCC_STOP_WAKEUPCLOCK_HSI: HSI oscillator selection
  * @note   This function shall not be called after the Clock Security System on HSE has been
  *         enabled.
  * @retval None
  */
void HAL_RCCEx_WakeUpStopCLKConfig(uint32_t WakeUpClk)
{
  assert_param(IS_RCC_STOP_WAKEUPCLOCK(WakeUpClk));

  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(WakeUpClk);
}

/**
  * @brief  Configure the oscillator Kernel clock source for wakeup from Stop
  * @param  WakeUpClk: Kernel Wakeup clock
  *         This parameter can be one of the following values:
  *            @arg RCC_STOP_KERWAKEUPCLOCK_CSI: CSI oscillator selection
  *            @arg RCC_STOP_KERWAKEUPCLOCK_HSI: HSI oscillator selection
  * @retval None
  */
void HAL_RCCEx_KerWakeUpStopCLKConfig(uint32_t WakeUpClk)
{
  assert_param(IS_RCC_STOP_KERWAKEUPCLOCK(WakeUpClk));

  __HAL_RCC_KERWAKEUPSTOP_CLK_CONFIG(WakeUpClk);
}

#if defined(DUAL_CORE)
/**
  * @brief  Enable COREx boot independently of CMx_B option byte value
  * @param  RCC_BootCx: Boot Core to be enabled
  *         This parameter can be one of the following values:
  *            @arg RCC_BOOT_C1: CM7 core selection
  *            @arg RCC_BOOT_C2: CM4 core selection
  * @note   This bit can be set by software but is cleared by hardware after a system reset or STANDBY
  *
  * @retval None
  */
void HAL_RCCEx_EnableBootCore(uint32_t RCC_BootCx)
{
  assert_param(IS_RCC_BOOT_CORE(RCC_BootCx));
  SET_BIT(RCC->GCR, RCC_BootCx) ;
}

#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
/**
  * @brief  Configure WWDGx to generate a system reset not only CPUx reset(default) when a time-out occurs
  * @param  RCC_WWDGx: WWDGx to be configured
  *         This parameter can be one of the following values:
  *            @arg RCC_WWDG1: WWDG1 generates system reset
  *            @arg RCC_WWDG2: WWDG2 generates system reset
  * @note   This bit can be set by software but is cleared by hardware during a system reset
  *
  * @retval None
  */
void HAL_RCCEx_WWDGxSysResetConfig(uint32_t RCC_WWDGx)
{
  assert_param(IS_RCC_SCOPE_WWDG(RCC_WWDGx));
  SET_BIT(RCC->GCR, RCC_WWDGx) ;
}

#else
#if defined(RCC_GCR_WW1RSC)
/**
  * @brief  Configure WWDG1 to generate a system reset not only CPU reset(default) when a time-out occurs
  * @param  RCC_WWDGx: WWDGx to be configured
  *         This parameter can be one of the following values:
  *            @arg RCC_WWDG1: WWDG1 generates system reset
  * @note   This bit can be set by software but is cleared by hardware during a system reset
  *
  * @retval None
  */
void HAL_RCCEx_WWDGxSysResetConfig(uint32_t RCC_WWDGx)
{
  assert_param(IS_RCC_SCOPE_WWDG(RCC_WWDGx));
  SET_BIT(RCC->GCR, RCC_WWDGx) ;
}
#endif
#endif /*DUAL_CORE*/

/**
  * @}
  */

/** @defgroup RCCEx_Exported_Functions_Group3 Extended Clock Recovery System Control functions
 *  @brief  Extended Clock Recovery System Control functions
 *
@verbatim
 ===============================================================================
                ##### Extended Clock Recovery System Control functions  #####
 ===============================================================================
    [..]
      For devices with Clock Recovery System feature (CRS), RCC Extension HAL driver can be used as follows:

      (#) In System clock config, HSI48 needs to be enabled

      (#) Enable CRS clock in IP MSP init which will use CRS functions

      (#) Call CRS functions as follows:
          (##) Prepare synchronization configuration necessary for HSI48 calibration
              (+++) Default values can be set for frequency Error Measurement (reload and error limit)
                        and also HSI48 oscillator smooth trimming.
              (+++) Macro __HAL_RCC_CRS_RELOADVALUE_CALCULATE can be also used to calculate
                        directly reload value with target and synchronization frequencies values
          (##) Call function HAL_RCCEx_CRSConfig which
              (+++) Resets CRS registers to their default values.
              (+++) Configures CRS registers with synchronization configuration
              (+++) Enables automatic calibration and frequency error counter feature
           Note: When using USB LPM (Link Power Management) and the device is in Sleep mode, the
           periodic USB SOF will not be generated by the host. No SYNC signal will therefore be
           provided to the CRS to calibrate the HSI48 on the run. To guarantee the required clock
           precision after waking up from Sleep mode, the LSE or reference clock on the GPIOs
           should be used as SYNC signal.

          (##) A polling function is provided to wait for complete synchronization
              (+++) Call function HAL_RCCEx_CRSWaitSynchronization()
              (+++) According to CRS status, user can decide to adjust again the calibration or continue
                        application if synchronization is OK

      (#) User can retrieve information related to synchronization in calling function
            HAL_RCCEx_CRSGetSynchronizationInfo()

      (#) Regarding synchronization status and synchronization information, user can try a new calibration
           in changing synchronization configuration and call again HAL_RCCEx_CRSConfig.
           Note: When the SYNC event is detected during the down-counting phase (before reaching the zero value),
           it means that the actual frequency is lower than the target (and so, that the TRIM value should be
           incremented), while when it is detected during the up-counting phase it means that the actual frequency
           is higher (and that the TRIM value should be decremented).

      (#) In interrupt mode, user can resort to the available macros (__HAL_RCC_CRS_XXX_IT). Interrupts will go
          through CRS Handler (CRS_IRQn/CRS_IRQHandler)
              (++) Call function HAL_RCCEx_CRSConfig()
              (++) Enable CRS_IRQn (thanks to NVIC functions)
              (++) Enable CRS interrupt (__HAL_RCC_CRS_ENABLE_IT)
              (++) Implement CRS status management in the following user callbacks called from
                   HAL_RCCEx_CRS_IRQHandler():
                   (+++) HAL_RCCEx_CRS_SyncOkCallback()
                   (+++) HAL_RCCEx_CRS_SyncWarnCallback()
                   (+++) HAL_RCCEx_CRS_ExpectedSyncCallback()
                   (+++) HAL_RCCEx_CRS_ErrorCallback()

      (#) To force a SYNC EVENT, user can use the function HAL_RCCEx_CRSSoftwareSynchronizationGenerate().
          This function can be called before calling HAL_RCCEx_CRSConfig (for instance in Systick handler)

@endverbatim
 * @{
 */

/**
  * @brief  Start automatic synchronization for polling mode
  * @param  pInit Pointer on RCC_CRSInitTypeDef structure
  * @retval None
  */
void HAL_RCCEx_CRSConfig(RCC_CRSInitTypeDef *pInit)
{
  uint32_t value;

  /* Check the parameters */
  assert_param(IS_RCC_CRS_SYNC_DIV(pInit->Prescaler));
  assert_param(IS_RCC_CRS_SYNC_SOURCE(pInit->Source));
  assert_param(IS_RCC_CRS_SYNC_POLARITY(pInit->Polarity));
  assert_param(IS_RCC_CRS_RELOADVALUE(pInit->ReloadValue));
  assert_param(IS_RCC_CRS_ERRORLIMIT(pInit->ErrorLimitValue));
  assert_param(IS_RCC_CRS_HSI48CALIBRATION(pInit->HSI48CalibrationValue));

  /* CONFIGURATION */

  /* Before configuration, reset CRS registers to their default values*/
  __HAL_RCC_CRS_FORCE_RESET();
  __HAL_RCC_CRS_RELEASE_RESET();

  /* Set the SYNCDIV[2:0] bits according to Pre-scaler value */
  /* Set the SYNCSRC[1:0] bits according to Source value */
  /* Set the SYNCSPOL bit according to Polarity value */
  if ((HAL_GetREVID() <= REV_ID_Y) && (pInit->Source == RCC_CRS_SYNC_SOURCE_USB2))
  {
    /* Use Rev.Y value of USB2 */
    value = (pInit->Prescaler | RCC_CRS_SYNC_SOURCE_PIN | pInit->Polarity);
  }
  else
  {
    value = (pInit->Prescaler | pInit->Source | pInit->Polarity);
  }
  /* Set the RELOAD[15:0] bits according to ReloadValue value */
  value |= pInit->ReloadValue;
  /* Set the FELIM[7:0] bits according to ErrorLimitValue value */
  value |= (pInit->ErrorLimitValue << CRS_CFGR_FELIM_Pos);
  WRITE_REG(CRS->CFGR, value);

  /* Adjust HSI48 oscillator smooth trimming */
  /* Set the TRIM[5:0] bits according to RCC_CRS_HSI48CalibrationValue value */
  MODIFY_REG(CRS->CR, CRS_CR_TRIM, (pInit->HSI48CalibrationValue << CRS_CR_TRIM_Pos));

  /* START AUTOMATIC SYNCHRONIZATION*/

  /* Enable Automatic trimming & Frequency error counter */
  SET_BIT(CRS->CR, CRS_CR_AUTOTRIMEN | CRS_CR_CEN);
}

/**
  * @brief  Generate the software synchronization event
  * @retval None
  */
void HAL_RCCEx_CRSSoftwareSynchronizationGenerate(void)
{
  SET_BIT(CRS->CR, CRS_CR_SWSYNC);
}

/**
  * @brief  Return synchronization info
  * @param  pSynchroInfo Pointer on RCC_CRSSynchroInfoTypeDef structure
  * @retval None
  */
void HAL_RCCEx_CRSGetSynchronizationInfo(RCC_CRSSynchroInfoTypeDef *pSynchroInfo)
{
  /* Check the parameter */
  assert_param(pSynchroInfo != (void *)NULL);

  /* Get the reload value */
  pSynchroInfo->ReloadValue = (uint32_t)(READ_BIT(CRS->CFGR, CRS_CFGR_RELOAD));

  /* Get HSI48 oscillator smooth trimming */
  pSynchroInfo->HSI48CalibrationValue = (uint32_t)(READ_BIT(CRS->CR, CRS_CR_TRIM) >> CRS_CR_TRIM_Pos);

  /* Get Frequency error capture */
  pSynchroInfo->FreqErrorCapture = (uint32_t)(READ_BIT(CRS->ISR, CRS_ISR_FECAP) >> CRS_ISR_FECAP_Pos);

  /* Get Frequency error direction */
  pSynchroInfo->FreqErrorDirection = (uint32_t)(READ_BIT(CRS->ISR, CRS_ISR_FEDIR));
}

/**
* @brief Wait for CRS Synchronization status.
* @param Timeout  Duration of the time-out
* @note  Timeout is based on the maximum time to receive a SYNC event based on synchronization
*        frequency.
* @note    If Time-out set to HAL_MAX_DELAY, HAL_TIMEOUT will be never returned.
* @retval Combination of Synchronization status
*          This parameter can be a combination of the following values:
*            @arg @ref RCC_CRS_TIMEOUT
*            @arg @ref RCC_CRS_SYNCOK
*            @arg @ref RCC_CRS_SYNCWARN
*            @arg @ref RCC_CRS_SYNCERR
*            @arg @ref RCC_CRS_SYNCMISS
*            @arg @ref RCC_CRS_TRIMOVF
*/
uint32_t HAL_RCCEx_CRSWaitSynchronization(uint32_t Timeout)
{
  uint32_t crsstatus = RCC_CRS_NONE;
  uint32_t tickstart;

  /* Get time-out */
  tickstart = HAL_GetTick();

  /* Wait for CRS flag or time-out detection */
  do
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        crsstatus = RCC_CRS_TIMEOUT;
      }
    }
    /* Check CRS SYNCOK flag  */
    if(__HAL_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCOK))
    {
      /* CRS SYNC event OK */
      crsstatus |= RCC_CRS_SYNCOK;

      /* Clear CRS SYNC event OK bit */
      __HAL_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCOK);
    }

    /* Check CRS SYNCWARN flag  */
    if(__HAL_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCWARN))
    {
      /* CRS SYNC warning */
      crsstatus |= RCC_CRS_SYNCWARN;

      /* Clear CRS SYNCWARN bit */
      __HAL_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCWARN);
    }

    /* Check CRS TRIM overflow flag  */
    if(__HAL_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_TRIMOVF))
    {
      /* CRS SYNC Error */
      crsstatus |= RCC_CRS_TRIMOVF;

      /* Clear CRS Error bit */
      __HAL_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_TRIMOVF);
    }

    /* Check CRS Error flag  */
    if(__HAL_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCERR))
    {
      /* CRS SYNC Error */
      crsstatus |= RCC_CRS_SYNCERR;

      /* Clear CRS Error bit */
      __HAL_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCERR);
    }

    /* Check CRS SYNC Missed flag  */
    if(__HAL_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCMISS))
    {
      /* CRS SYNC Missed */
      crsstatus |= RCC_CRS_SYNCMISS;

      /* Clear CRS SYNC Missed bit */
      __HAL_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCMISS);
    }

    /* Check CRS Expected SYNC flag  */
    if(__HAL_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_ESYNC))
    {
      /* frequency error counter reached a zero value */
      __HAL_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_ESYNC);
    }
  } while(RCC_CRS_NONE == crsstatus);

  return crsstatus;
}

/**
  * @brief Handle the Clock Recovery System interrupt request.
  * @retval None
  */
void HAL_RCCEx_CRS_IRQHandler(void)
{
  uint32_t crserror = RCC_CRS_NONE;
  /* Get current IT flags and IT sources values */
  uint32_t itflags = READ_REG(CRS->ISR);
  uint32_t itsources = READ_REG(CRS->CR);

  /* Check CRS SYNCOK flag  */
  if(((itflags & RCC_CRS_FLAG_SYNCOK) != 0U) && ((itsources & RCC_CRS_IT_SYNCOK) != 0U))
  {
    /* Clear CRS SYNC event OK flag */
    WRITE_REG(CRS->ICR, CRS_ICR_SYNCOKC);

    /* user callback */
    HAL_RCCEx_CRS_SyncOkCallback();
  }
  /* Check CRS SYNCWARN flag  */
  else if(((itflags & RCC_CRS_FLAG_SYNCWARN) != 0U) && ((itsources & RCC_CRS_IT_SYNCWARN) != 0U))
  {
    /* Clear CRS SYNCWARN flag */
    WRITE_REG(CRS->ICR, CRS_ICR_SYNCWARNC);

    /* user callback */
    HAL_RCCEx_CRS_SyncWarnCallback();
  }
  /* Check CRS Expected SYNC flag  */
  else if(((itflags & RCC_CRS_FLAG_ESYNC) != 0U) && ((itsources & RCC_CRS_IT_ESYNC) != 0U))
  {
    /* frequency error counter reached a zero value */
    WRITE_REG(CRS->ICR, CRS_ICR_ESYNCC);

    /* user callback */
    HAL_RCCEx_CRS_ExpectedSyncCallback();
  }
  /* Check CRS Error flags  */
  else
  {
    if(((itflags & RCC_CRS_FLAG_ERR) != 0U) && ((itsources & RCC_CRS_IT_ERR) != 0U))
    {
      if((itflags & RCC_CRS_FLAG_SYNCERR) != 0U)
      {
        crserror |= RCC_CRS_SYNCERR;
      }
      if((itflags & RCC_CRS_FLAG_SYNCMISS) != 0U)
      {
        crserror |= RCC_CRS_SYNCMISS;
      }
      if((itflags & RCC_CRS_FLAG_TRIMOVF) != 0U)
      {
        crserror |= RCC_CRS_TRIMOVF;
      }

      /* Clear CRS Error flags */
      WRITE_REG(CRS->ICR, CRS_ICR_ERRC);

      /* user error callback */
      HAL_RCCEx_CRS_ErrorCallback(crserror);
    }
  }
}

/**
  * @brief  RCCEx Clock Recovery System SYNCOK interrupt callback.
  * @retval none
  */
__weak void HAL_RCCEx_CRS_SyncOkCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the @ref HAL_RCCEx_CRS_SyncOkCallback should be implemented in the user file
   */
}

/**
  * @brief  RCCEx Clock Recovery System SYNCWARN interrupt callback.
  * @retval none
  */
__weak void HAL_RCCEx_CRS_SyncWarnCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the @ref HAL_RCCEx_CRS_SyncWarnCallback should be implemented in the user file
   */
}

/**
  * @brief  RCCEx Clock Recovery System Expected SYNC interrupt callback.
  * @retval none
  */
__weak void HAL_RCCEx_CRS_ExpectedSyncCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the @ref HAL_RCCEx_CRS_ExpectedSyncCallback should be implemented in the user file
   */
}

/**
  * @brief  RCCEx Clock Recovery System Error interrupt callback.
  * @param  Error Combination of Error status.
  *         This parameter can be a combination of the following values:
  *           @arg @ref RCC_CRS_SYNCERR
  *           @arg @ref RCC_CRS_SYNCMISS
  *           @arg @ref RCC_CRS_TRIMOVF
  * @retval none
  */
__weak void HAL_RCCEx_CRS_ErrorCallback(uint32_t Error)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Error);

  /* NOTE : This function should not be modified, when the callback is needed,
            the @ref HAL_RCCEx_CRS_ErrorCallback should be implemented in the user file
   */
}


/**
  * @}
  */

/**
  * @}
  */

/** @defgroup RCCEx_Private_functions Private Functions
 * @{
 */
/**
  * @brief  Configure the PLL2 VCI,VCO ranges, multiplication and division factors and enable it
  * @param  pll2: Pointer to an RCC_PLL2InitTypeDef structure that
  *         contains the configuration parameters  as well as VCI, VCO clock ranges.
  * @param  Divider  divider parameter to be updated
  * @note   PLL2 is temporary disabled to apply new parameters
  *
  * @retval HAL status
  */
static HAL_StatusTypeDef RCCEx_PLL2_Config(RCC_PLL2InitTypeDef *pll2, uint32_t Divider)
{

  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;
  assert_param(IS_RCC_PLL2M_VALUE(pll2->PLL2M));
  assert_param(IS_RCC_PLL2N_VALUE(pll2->PLL2N));
  assert_param(IS_RCC_PLL2P_VALUE(pll2->PLL2P));
  assert_param(IS_RCC_PLL2R_VALUE(pll2->PLL2R));
  assert_param(IS_RCC_PLL2Q_VALUE(pll2->PLL2Q));
  assert_param(IS_RCC_PLL2RGE_VALUE(pll2->PLL2RGE));
  assert_param(IS_RCC_PLL2VCO_VALUE(pll2->PLL2VCOSEL));
  assert_param(IS_RCC_PLLFRACN_VALUE(pll2->PLL2FRACN));

  /* Check that PLL2 OSC clock source is already set */
  if(__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_NONE)
  {
    return HAL_ERROR;
  }


  else
  {
    /* Disable  PLL2. */
    __HAL_RCC_PLL2_DISABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLL is disabled */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLL2RDY) != 0U)
    {
      if( (HAL_GetTick() - tickstart ) > PLL2_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

    /* Configure PLL2 multiplication and division factors. */
    __HAL_RCC_PLL2_CONFIG(pll2->PLL2M,
                          pll2->PLL2N,
                          pll2->PLL2P,
                          pll2->PLL2Q,
                          pll2->PLL2R);

    /* Select PLL2 input reference frequency range: VCI */
    __HAL_RCC_PLL2_VCIRANGE(pll2->PLL2RGE) ;

    /* Select PLL2 output frequency range : VCO */
    __HAL_RCC_PLL2_VCORANGE(pll2->PLL2VCOSEL) ;

    /* Disable PLL2FRACN . */
    __HAL_RCC_PLL2FRACN_DISABLE();

    /* Configures PLL2 clock Fractional Part Of The Multiplication Factor */
    __HAL_RCC_PLL2FRACN_CONFIG(pll2->PLL2FRACN);

    /* Enable PLL2FRACN . */
    __HAL_RCC_PLL2FRACN_ENABLE();

    /* Enable the PLL2 clock output */
    if(Divider == DIVIDER_P_UPDATE)
    {
      __HAL_RCC_PLL2CLKOUT_ENABLE(RCC_PLL2_DIVP);
    }
    else if(Divider == DIVIDER_Q_UPDATE)
    {
      __HAL_RCC_PLL2CLKOUT_ENABLE(RCC_PLL2_DIVQ);
    }
    else
    {
      __HAL_RCC_PLL2CLKOUT_ENABLE(RCC_PLL2_DIVR);
    }

    /* Enable  PLL2. */
    __HAL_RCC_PLL2_ENABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLL2 is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLL2RDY) == 0U)
    {
      if( (HAL_GetTick() - tickstart ) > PLL2_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

  }


  return status;
}


/**
  * @brief  Configure the PLL3 VCI,VCO ranges, multiplication and division factors and enable it
  * @param  pll3: Pointer to an RCC_PLL3InitTypeDef structure that
  *         contains the configuration parameters  as well as VCI, VCO clock ranges.
  * @param  Divider  divider parameter to be updated
  * @note   PLL3 is temporary disabled to apply new parameters
  *
  * @retval HAL status
  */
static HAL_StatusTypeDef RCCEx_PLL3_Config(RCC_PLL3InitTypeDef *pll3, uint32_t Divider)
{
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;
  assert_param(IS_RCC_PLL3M_VALUE(pll3->PLL3M));
  assert_param(IS_RCC_PLL3N_VALUE(pll3->PLL3N));
  assert_param(IS_RCC_PLL3P_VALUE(pll3->PLL3P));
  assert_param(IS_RCC_PLL3R_VALUE(pll3->PLL3R));
  assert_param(IS_RCC_PLL3Q_VALUE(pll3->PLL3Q));
  assert_param(IS_RCC_PLL3RGE_VALUE(pll3->PLL3RGE));
  assert_param(IS_RCC_PLL3VCO_VALUE(pll3->PLL3VCOSEL));
  assert_param(IS_RCC_PLLFRACN_VALUE(pll3->PLL3FRACN));

  /* Check that PLL3 OSC clock source is already set */
  if(__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_NONE)
  {
    return HAL_ERROR;
  }


  else
  {
    /* Disable  PLL3. */
    __HAL_RCC_PLL3_DISABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();
    /* Wait till PLL3 is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLL3RDY) != 0U)
    {
      if( (HAL_GetTick() - tickstart ) > PLL3_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

    /* Configure the PLL3  multiplication and division factors. */
    __HAL_RCC_PLL3_CONFIG(pll3->PLL3M,
                          pll3->PLL3N,
                          pll3->PLL3P,
                          pll3->PLL3Q,
                          pll3->PLL3R);

    /* Select PLL3 input reference frequency range: VCI */
    __HAL_RCC_PLL3_VCIRANGE(pll3->PLL3RGE) ;

    /* Select PLL3 output frequency range : VCO */
    __HAL_RCC_PLL3_VCORANGE(pll3->PLL3VCOSEL) ;

    /* Disable PLL3FRACN . */
    __HAL_RCC_PLL3FRACN_DISABLE();

    /* Configures PLL3 clock Fractional Part Of The Multiplication Factor */
    __HAL_RCC_PLL3FRACN_CONFIG(pll3->PLL3FRACN);

    /* Enable PLL3FRACN . */
    __HAL_RCC_PLL3FRACN_ENABLE();

    /* Enable the PLL3 clock output */
    if(Divider == DIVIDER_P_UPDATE)
    {
      __HAL_RCC_PLL3CLKOUT_ENABLE(RCC_PLL3_DIVP);
    }
    else if(Divider == DIVIDER_Q_UPDATE)
    {
      __HAL_RCC_PLL3CLKOUT_ENABLE(RCC_PLL3_DIVQ);
    }
    else
    {
      __HAL_RCC_PLL3CLKOUT_ENABLE(RCC_PLL3_DIVR);
    }

    /* Enable  PLL3. */
    __HAL_RCC_PLL3_ENABLE();

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till PLL3 is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLL3RDY) == 0U)
    {
      if( (HAL_GetTick() - tickstart ) > PLL3_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }

  }


  return status;
}

/**
  * @brief Handle the RCC LSE Clock Security System interrupt request.
  * @retval None
  */
void HAL_RCCEx_LSECSS_IRQHandler(void)
{
  /* Check RCC LSE CSSF flag  */
  if(__HAL_RCC_GET_IT(RCC_IT_LSECSS))
  {

    /* Clear RCC LSE CSS pending bit */
    __HAL_RCC_CLEAR_IT(RCC_IT_LSECSS);

    /* RCC LSE Clock Security System interrupt user callback */
    HAL_RCCEx_LSECSS_Callback();

  }
}

/**
  * @brief  RCCEx LSE Clock Security System interrupt callback.
  * @retval none
  */
__weak void HAL_RCCEx_LSECSS_Callback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the @ref HAL_RCCEx_LSECSS_Callback should be implemented in the user file
   */
}



/**
  * @}
  */

#endif /* HAL_RCC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
