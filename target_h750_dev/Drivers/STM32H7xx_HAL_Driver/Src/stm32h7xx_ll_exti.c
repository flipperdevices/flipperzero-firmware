/**
  ******************************************************************************
  * @file    stm32h7xx_ll_exti.c
  * @author  MCD Application Team
  * @brief   EXTI LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_ll_exti.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

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
/* Private macros ------------------------------------------------------------*/
/** @addtogroup EXTI_LL_Private_Macros
  * @{
  */

#define IS_LL_EXTI_LINE_0_31(__VALUE__)              (((__VALUE__) & ~LL_EXTI_LINE_ALL_0_31)  == 0x00000000U)
#define IS_LL_EXTI_LINE_32_63(__VALUE__)             (((__VALUE__) & ~LL_EXTI_LINE_ALL_32_63) == 0x00000000U)
#define IS_LL_EXTI_LINE_64_95(__VALUE__)             (((__VALUE__) & ~LL_EXTI_LINE_ALL_64_95) == 0x00000000U)

#define IS_LL_EXTI_MODE(__VALUE__)                   (((__VALUE__) == LL_EXTI_MODE_IT)            \
                                                   || ((__VALUE__) == LL_EXTI_MODE_EVENT)         \
                                                   || ((__VALUE__) == LL_EXTI_MODE_IT_EVENT))


#define IS_LL_EXTI_TRIGGER(__VALUE__)                (((__VALUE__) == LL_EXTI_TRIGGER_NONE)       \
                                                   || ((__VALUE__) == LL_EXTI_TRIGGER_RISING)     \
                                                   || ((__VALUE__) == LL_EXTI_TRIGGER_FALLING)    \
                                                   || ((__VALUE__) == LL_EXTI_TRIGGER_RISING_FALLING))

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup EXTI_LL_Exported_Functions
  * @{
  */

/** @addtogroup EXTI_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the EXTI registers to their default reset values.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: EXTI registers are de-initialized
  *          - ERROR: not applicable
  */
ErrorStatus LL_EXTI_DeInit(void)
{
  /* Rising Trigger selection register set to default reset values */
  LL_EXTI_WriteReg(RTSR1,  0x00000000U);
  LL_EXTI_WriteReg(RTSR2,  0x00000000U);
  LL_EXTI_WriteReg(RTSR3,  0x00000000U);

  /* Falling Trigger selection register set to default reset values */
  LL_EXTI_WriteReg(FTSR1,  0x00000000U);
  LL_EXTI_WriteReg(FTSR2,  0x00000000U);
  LL_EXTI_WriteReg(FTSR3,  0x00000000U);

  /* Software interrupt event register set to default reset values */
  LL_EXTI_WriteReg(SWIER1, 0x00000000U);
  LL_EXTI_WriteReg(SWIER2, 0x00000000U);
  LL_EXTI_WriteReg(SWIER3, 0x00000000U);

  /* D3 Pending register set to default reset values */
  LL_EXTI_WriteReg(D3PMR1, 0x00000000U);
  LL_EXTI_WriteReg(D3PMR2, 0x00000000U);
  LL_EXTI_WriteReg(D3PMR3, 0x00000000U);

  /* D3 Pending clear selection register low to default reset values */
  LL_EXTI_WriteReg(D3PCR1L, 0x00000000U);
  LL_EXTI_WriteReg(D3PCR2L, 0x00000000U);
  LL_EXTI_WriteReg(D3PCR3L, 0x00000000U);

  /* D3 Pending clear selection register high to default reset values */
  LL_EXTI_WriteReg(D3PCR1H, 0x00000000U);
  LL_EXTI_WriteReg(D3PCR2H, 0x00000000U);
  LL_EXTI_WriteReg(D3PCR3H, 0x00000000U);

  /* Interrupt mask register set to default reset values */
  LL_EXTI_WriteReg(IMR1, 0x00000000U);
  LL_EXTI_WriteReg(IMR2, 0x00000000U);
  LL_EXTI_WriteReg(IMR3, 0x00000000U);

  /*  Event mask register set to default reset values */
  LL_EXTI_WriteReg(EMR1, 0x00000000U);
  LL_EXTI_WriteReg(EMR2, 0x00000000U);
  LL_EXTI_WriteReg(EMR3, 0x00000000U);

  /* Clear Pending requests */
  LL_EXTI_WriteReg(PR1, EXTI_PR1_PR_Msk);
  LL_EXTI_WriteReg(PR2, EXTI_PR2_PR_Msk);
  LL_EXTI_WriteReg(PR3, EXTI_PR3_PR_Msk);

#if defined(DUAL_CORE)
  /* Interrupt mask register set to default reset values  for Core 2 (Coretx-M4)*/
  LL_EXTI_WriteReg(C2IMR1, 0x00000000U);
  LL_EXTI_WriteReg(C2IMR2, 0x00000000U);
  LL_EXTI_WriteReg(C2IMR3, 0x00000000U);

  /*  Event mask register set to default reset values */
  LL_EXTI_WriteReg(C2EMR1, 0x00000000U);
  LL_EXTI_WriteReg(C2EMR2, 0x00000000U);
  LL_EXTI_WriteReg(C2EMR3, 0x00000000U);

  /* Clear Pending requests */
  LL_EXTI_WriteReg(C2PR1, EXTI_PR1_PR_Msk);
  LL_EXTI_WriteReg(C2PR2, EXTI_PR2_PR_Msk);
  LL_EXTI_WriteReg(C2PR3, EXTI_PR3_PR_Msk);

#endif /* DUAL_CORE*/
  return SUCCESS;
}

/**
  * @brief  Initialize the EXTI registers according to the specified parameters in EXTI_InitStruct.
  * @param  EXTI_InitStruct pointer to a @ref LL_EXTI_InitTypeDef structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: EXTI registers are initialized
  *          - ERROR: not applicable
  */
ErrorStatus LL_EXTI_Init(LL_EXTI_InitTypeDef *EXTI_InitStruct)
{
  ErrorStatus status = SUCCESS;
  /* Check the parameters */
  assert_param(IS_LL_EXTI_LINE_0_31(EXTI_InitStruct->Line_0_31));
  assert_param(IS_LL_EXTI_LINE_32_63(EXTI_InitStruct->Line_32_63));
  assert_param(IS_LL_EXTI_LINE_64_95(EXTI_InitStruct->Line_64_95));
  assert_param(IS_FUNCTIONAL_STATE(EXTI_InitStruct->LineCommand));
  assert_param(IS_LL_EXTI_MODE(EXTI_InitStruct->Mode));

  /* ENABLE LineCommand */
  if (EXTI_InitStruct->LineCommand != DISABLE)
  {
    assert_param(IS_LL_EXTI_TRIGGER(EXTI_InitStruct->Trigger));

    /* Configure EXTI Lines in range from 0 to 31 */
    if (EXTI_InitStruct->Line_0_31 != LL_EXTI_LINE_NONE)
    {
      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_IT) == LL_EXTI_MODE_IT)
      {
        /* Enable IT on provided Lines for Cortex-M7*/
        LL_EXTI_EnableIT_0_31(EXTI_InitStruct->Line_0_31);
      }
      else
      {
        /* Disable IT on provided Lines for Cortex-M7*/
        LL_EXTI_DisableIT_0_31(EXTI_InitStruct->Line_0_31);
      }

      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_EVENT) == LL_EXTI_MODE_EVENT)
      {
        /* Enable event on provided Lines for Cortex-M7 */
        LL_EXTI_EnableEvent_0_31(EXTI_InitStruct->Line_0_31);
      }
      else
      {
        /* Disable event on provided Lines for Cortex-M7 */
        LL_EXTI_DisableEvent_0_31(EXTI_InitStruct->Line_0_31);
      }
#if defined(DUAL_CORE)
      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_C2_IT) == LL_EXTI_MODE_C2_IT)
      {
        /* Enable IT on provided Lines for Cortex-M4 */
        LL_C2_EXTI_EnableIT_0_31 (EXTI_InitStruct->Line_0_31);
      }
      else
      {
        /* Disable IT on provided Lines for Cortex-M4*/
        LL_C2_EXTI_DisableIT_0_31(EXTI_InitStruct->Line_0_31);
      }

      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_C2_EVENT) == LL_EXTI_MODE_C2_EVENT)
      {
        /* Enable event on provided Lines for Cortex-M4 */
        LL_C2_EXTI_EnableEvent_0_31(EXTI_InitStruct->Line_0_31);
      }
      else
      {
        /* Disable event on provided Lines for Cortex-M4*/
        LL_C2_EXTI_DisableEvent_0_31(EXTI_InitStruct->Line_0_31);
      }
#endif /* DUAL_CORE */

      if (EXTI_InitStruct->Trigger != LL_EXTI_TRIGGER_NONE)
      {
        switch (EXTI_InitStruct->Trigger)
        {
          case LL_EXTI_TRIGGER_RISING:
            /* First Disable Falling Trigger on provided Lines */
            LL_EXTI_DisableFallingTrig_0_31(EXTI_InitStruct->Line_0_31);
            /* Then Enable Rising Trigger on provided Lines */
            LL_EXTI_EnableRisingTrig_0_31(EXTI_InitStruct->Line_0_31);
            break;
          case LL_EXTI_TRIGGER_FALLING:
            /* First Disable Rising Trigger on provided Lines */
            LL_EXTI_DisableRisingTrig_0_31(EXTI_InitStruct->Line_0_31);
            /* Then Enable Falling Trigger on provided Lines */
            LL_EXTI_EnableFallingTrig_0_31(EXTI_InitStruct->Line_0_31);
            break;
          case LL_EXTI_TRIGGER_RISING_FALLING:
            LL_EXTI_EnableRisingTrig_0_31(EXTI_InitStruct->Line_0_31);
            LL_EXTI_EnableFallingTrig_0_31(EXTI_InitStruct->Line_0_31);
            break;
          default:
            status = ERROR;
            break;
        }
      }
    }
    /* Configure EXTI Lines in range from 32 to 63 */
    if (EXTI_InitStruct->Line_32_63 != LL_EXTI_LINE_NONE)
    {
      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_IT) == LL_EXTI_MODE_IT)
      {
        /* Enable IT on provided Lines for Cortex-M7*/
        LL_EXTI_EnableIT_32_63(EXTI_InitStruct->Line_32_63);
      }
      else
      {
        /* Disable IT on provided Lines for Cortex-M7*/
        LL_EXTI_DisableIT_32_63(EXTI_InitStruct->Line_32_63);
      }

      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_EVENT) == LL_EXTI_MODE_EVENT)
      {
        /* Enable event on provided Lines for Cortex-M7 */
        LL_EXTI_EnableEvent_32_63(EXTI_InitStruct->Line_32_63);
      }
      else
      {
        /* Disable event on provided Lines for Cortex-M7 */
        LL_EXTI_DisableEvent_32_63(EXTI_InitStruct->Line_32_63);
      }
#if defined(DUAL_CORE)
      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_C2_IT) == LL_EXTI_MODE_C2_IT)
      {
        /* Enable IT on provided Lines for Cortex-M4 */
        LL_C2_EXTI_EnableIT_32_63 (EXTI_InitStruct->Line_32_63);
      }
      else
      {
        /* Disable IT on provided Lines for Cortex-M4 */
        LL_C2_EXTI_DisableIT_32_63 (EXTI_InitStruct->Line_32_63);
      }

      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_C2_EVENT) == LL_EXTI_MODE_C2_EVENT)
      {
        /* Enable event on provided Lines for Cortex-M4 */
        LL_C2_EXTI_EnableEvent_32_63(EXTI_InitStruct->Line_32_63);
      }
      else
      {
        /* Disable event on provided Lines for Cortex-M4 */
        LL_C2_EXTI_DisableEvent_32_63(EXTI_InitStruct->Line_32_63);
      }
#endif /* DUAL_CORE */

      if (EXTI_InitStruct->Trigger != LL_EXTI_TRIGGER_NONE)
      {
        switch (EXTI_InitStruct->Trigger)
        {
          case LL_EXTI_TRIGGER_RISING:
            /* First Disable Falling Trigger on provided Lines */
            LL_EXTI_DisableFallingTrig_32_63(EXTI_InitStruct->Line_32_63);
            /* Then Enable IT on provided Lines */
            LL_EXTI_EnableRisingTrig_32_63(EXTI_InitStruct->Line_32_63);
            break;
          case LL_EXTI_TRIGGER_FALLING:
            /* First Disable Rising Trigger on provided Lines */
            LL_EXTI_DisableRisingTrig_32_63(EXTI_InitStruct->Line_32_63);
            /* Then Enable Falling Trigger on provided Lines */
            LL_EXTI_EnableFallingTrig_32_63(EXTI_InitStruct->Line_32_63);
            break;
          case LL_EXTI_TRIGGER_RISING_FALLING:
            LL_EXTI_EnableRisingTrig_32_63(EXTI_InitStruct->Line_32_63);
            LL_EXTI_EnableFallingTrig_32_63(EXTI_InitStruct->Line_32_63);
            break;
          default:
            status = ERROR;
            break;
        }
      }
    }
    /* Configure EXTI Lines in range from 64 to 95 */
    if (EXTI_InitStruct->Line_64_95 != LL_EXTI_LINE_NONE)
    {
      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_IT) == LL_EXTI_MODE_IT)
      {
        /* Enable IT on provided Lines for Cortex-M7*/
        LL_EXTI_EnableIT_64_95(EXTI_InitStruct->Line_64_95);
      }
      else
      {
        /* Disable IT on provided Lines for Cortex-M7*/
        LL_EXTI_DisableIT_64_95(EXTI_InitStruct->Line_64_95);
      }

      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_EVENT) == LL_EXTI_MODE_EVENT)
      {
        /* Enable event on provided Lines for Cortex-M7 */
        LL_EXTI_EnableEvent_64_95(EXTI_InitStruct->Line_64_95);
      }
      else
      {
        /* Disable event on provided Lines for Cortex-M7 */
        LL_EXTI_DisableEvent_64_95(EXTI_InitStruct->Line_64_95);
      }

#if defined(DUAL_CORE)
      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_C2_IT) == LL_EXTI_MODE_C2_IT)
      {
        /* Enable IT on provided Lines for Cortex-M4 */
        LL_C2_EXTI_EnableIT_64_95 (EXTI_InitStruct->Line_64_95);
      }
      else
      {
        /* Disable IT on provided Lines for Cortex-M4 */
        LL_C2_EXTI_DisableIT_64_95 (EXTI_InitStruct->Line_64_95);
      }

      if((EXTI_InitStruct->Mode & LL_EXTI_MODE_C2_EVENT) == LL_EXTI_MODE_C2_EVENT)
      {
        /* Enable event on provided Lines for Cortex-M4 */
        LL_C2_EXTI_EnableEvent_64_95(EXTI_InitStruct->Line_64_95);
      }
      else
      {
        /* Disable event on provided Lines for Cortex-M4 */
        LL_C2_EXTI_DisableEvent_64_95(EXTI_InitStruct->Line_64_95);
      }
#endif /* DUAL_CORE */

      if (EXTI_InitStruct->Trigger != LL_EXTI_TRIGGER_NONE)
      {
        switch (EXTI_InitStruct->Trigger)
        {
          case LL_EXTI_TRIGGER_RISING:
            /* First Disable Falling Trigger on provided Lines */
            LL_EXTI_DisableFallingTrig_64_95(EXTI_InitStruct->Line_64_95);
            /* Then Enable IT on provided Lines */
            LL_EXTI_EnableRisingTrig_64_95(EXTI_InitStruct->Line_64_95);
            break;
          case LL_EXTI_TRIGGER_FALLING:
            /* First Disable Rising Trigger on provided Lines */
            LL_EXTI_DisableRisingTrig_64_95(EXTI_InitStruct->Line_64_95);
            /* Then Enable Falling Trigger on provided Lines */
            LL_EXTI_EnableFallingTrig_64_95(EXTI_InitStruct->Line_64_95);
            break;
          case LL_EXTI_TRIGGER_RISING_FALLING:
            LL_EXTI_EnableRisingTrig_64_95(EXTI_InitStruct->Line_64_95);
            LL_EXTI_EnableFallingTrig_64_95(EXTI_InitStruct->Line_64_95);
            break;
          default:
            status = ERROR;
            break;
        }
      }
    }
  }
  else /* DISABLE LineCommand */
  {
    /* Disable IT on provided Lines for Cortex-M7*/
    LL_EXTI_DisableIT_0_31(EXTI_InitStruct->Line_0_31);
    LL_EXTI_DisableIT_32_63(EXTI_InitStruct->Line_32_63);
    LL_EXTI_DisableIT_64_95(EXTI_InitStruct->Line_64_95);

    /* Disable event on provided Lines for Cortex-M7 */
    LL_EXTI_DisableEvent_0_31(EXTI_InitStruct->Line_0_31);
    LL_EXTI_DisableEvent_32_63(EXTI_InitStruct->Line_32_63);
    LL_EXTI_DisableEvent_64_95(EXTI_InitStruct->Line_64_95);

#if defined(DUAL_CORE)
    /* Disable IT on provided Lines for Cortex-M4*/
    LL_C2_EXTI_DisableIT_0_31(EXTI_InitStruct->Line_0_31);
    LL_C2_EXTI_DisableIT_32_63(EXTI_InitStruct->Line_32_63);
    LL_C2_EXTI_DisableIT_64_95(EXTI_InitStruct->Line_64_95);

    /* Disable event on provided Lines for Cortex-M4 */
    LL_C2_EXTI_DisableEvent_0_31(EXTI_InitStruct->Line_0_31);
    LL_C2_EXTI_DisableEvent_32_63(EXTI_InitStruct->Line_32_63);
    LL_C2_EXTI_DisableEvent_64_95(EXTI_InitStruct->Line_64_95);
#endif /* DUAL_CORE */
  }

  return status;
}

/**
  * @brief  Set each @ref LL_EXTI_InitTypeDef field to default value.
  * @param  EXTI_InitStruct Pointer to a @ref LL_EXTI_InitTypeDef structure.
  * @retval None
  */
void LL_EXTI_StructInit(LL_EXTI_InitTypeDef *EXTI_InitStruct)
{
  EXTI_InitStruct->Line_0_31      = LL_EXTI_LINE_NONE;
  EXTI_InitStruct->Line_32_63     = LL_EXTI_LINE_NONE;
  EXTI_InitStruct->Line_64_95     = LL_EXTI_LINE_NONE;
  EXTI_InitStruct->LineCommand    = DISABLE;
  EXTI_InitStruct->Mode           = LL_EXTI_MODE_IT;
  EXTI_InitStruct->Trigger        = LL_EXTI_TRIGGER_FALLING;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined (EXTI) */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
