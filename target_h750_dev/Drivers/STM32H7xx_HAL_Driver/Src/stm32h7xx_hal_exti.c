/**
  ******************************************************************************
  * @file    stm32h7xx_hal_exti.c
  * @author  MCD Application Team
  * @brief   EXTI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the General Purpose Input/Output (EXTI) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *
  @verbatim
  ==============================================================================
                    ##### EXTI Peripheral features #####
  ==============================================================================
  [..]
    (+) Each Exti line can be configured within this driver.

    (+) Exti line can be configured in 3 different modes
        (++) Interrupt (CORE1 or CORE2 in case of dual core line )
        (++) Event (CORE1 or CORE2 in case of dual core line )
        (++) a combination of the previous

    (+) Configurable Exti lines can be configured with 3 different triggers
        (++) Rising
        (++) Falling
        (++) Both of them

    (+) When set in interrupt mode, configurable Exti lines have two diffenrents
        interrupt pending registers which allow to distinguish which transition
        occurs:
        (++) Rising edge pending interrupt
        (++) Falling

    (+) Exti lines 0 to 15 are linked to gpio pin number 0 to 15. Gpio port can
        be selected throught multiplexer.

    (+) PendClearSource used to set the D3 Smart Run Domain autoamtic pend clear soure.
        It is applicable for line with wkaeup target is Any (CPU1 , CPU2 and D3 smart run domain).
        Value can be one of the following:
        (++)  EXTI_D3_PENDCLR_SRC_NONE : no pend clear source is selcted :
              In this case corresponding bit of D2PMRx register is set to 0
                (+++) On a configurable Line : the D3 domain wakeup signal is
                      automatically cleared after after the Delay + Rising Edge detect
                (+++) On a direct Line : the D3 domain wakeup signal is
                      cleared after the direct event input signal is cleared

        (++)  EXTI_D3_PENDCLR_SRC_DMACH6 : no pend clear source is selcted :
              In this case corresponding bit of D2PMRx register is set to 1
              and corresponding bits(2) of D3PCRxL/H is set to b00 :
                DMA ch6 event selected as D3 domain pendclear source

        (++)  EXTI_D3_PENDCLR_SRC_DMACH7 : no pend clear source is selcted :
              In this case corresponding bit of D2PMRx register is set to 1
              and corresponding bits(2) of D3PCRxL/H is set to b01 :
                DMA ch7 event selected as D3 domain pendclear source

        (++)  EXTI_D3_PENDCLR_SRC_LPTIM4 : no pend clear source is selcted :
              In this case corresponding bit of D2PMRx register is set to 1
              and corresponding bits(2) of D3PCRxL/H is set to b10 :
                LPTIM4 out selected as D3 domain pendclear source

        (++)  EXTI_D3_PENDCLR_SRC_LPTIM5 : no pend clear source is selcted :
              In this case corresponding bit of D2PMRx register is set to 1
              and corresponding bits(2) of D3PCRxL/H is set to b11 :
                LPTIM5 out selected as D3 domain pendclear source


                     ##### How to use this driver #####
  ==============================================================================
  [..]

    (#) Configure the EXTI line using HAL_EXTI_SetConfigLine().
        (++) Choose the interrupt line number by setting "Line" member from
             EXTI_ConfigTypeDef structure.
        (++) Configure the interrupt and/or event mode using "Mode" member from
             EXTI_ConfigTypeDef structure.
        (++) For configurable lines, configure rising and/or falling trigger
             "Trigger" member from EXTI_ConfigTypeDef structure.
        (++) For Exti lines linked to gpio, choose gpio port using "GPIOSel"
             member from GPIO_InitTypeDef structure.
        (++) For Exti lines with wkaeup target is Any (CPU1 , CPU2 and D3 smart run domain),
             choose gpio D3 PendClearSource using PendClearSource
             member from EXTI_PendClear_Source structure.

    (#) Get current Exti configuration of a dedicated line using
        HAL_EXTI_GetConfigLine().
        (++) Provide exiting handle as parameter.
        (++) Provide pointer on EXTI_ConfigTypeDef structure as second parameter.

    (#) Clear Exti configuration of a dedicated line using HAL_EXTI_GetConfigLine().
        (++) Provide exiting handle as parameter.

    (#) Register callback to treat Exti interrupts using HAL_EXTI_RegisterCallback().
        (++) Provide exiting handle as first parameter.
        (++) Provide which callback will be registered using one value from
             EXTI_CallbackIDTypeDef.
        (++) Provide callback function pointer.

    (#) Get interrupt pending bit using HAL_EXTI_GetPending().

    (#) Clear interrupt pending bit using HAL_EXTI_GetPending().

    (#) Generate software interrupt using HAL_EXTI_GenerateSWI().

  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup EXTI
  * @{
  */

#ifdef HAL_EXTI_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/** @defgroup EXTI_Private_Constants EXTI Private Constants
  * @{
  */
#define EXTI_MODE_OFFSET                    0x04U   /* 0x10: offset between CPU IMR/EMR registers */
#define EXTI_CONFIG_OFFSET                  0x08U   /* 0x20: offset between CPU Rising/Falling configuration registers */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup EXTI_Exported_Functions
  * @{
  */

/** @addtogroup EXTI_Exported_Functions_Group1
 *  @brief    Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Configuration functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Set configuration of a dedicated Exti line.
  * @param  hexti Exti handle.
  * @param  pExtiConfig Pointer on EXTI configuration to be set.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig)
{
  __IO uint32_t *regaddr;
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;
  uint32_t offset;
  uint32_t pcrlinepos;

  /* Check null pointer */
  if ((hexti == NULL) || (pExtiConfig == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_EXTI_LINE(pExtiConfig->Line));
  assert_param(IS_EXTI_MODE(pExtiConfig->Mode));

  /* Assign line number to handle */
  hexti->Line = pExtiConfig->Line;

  /* compute line register offset and line mask */
  offset = ((pExtiConfig->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  linepos = (pExtiConfig->Line & EXTI_PIN_MASK);
  maskline = (1UL << linepos);

  /* Configure triggers for configurable lines */
  if ((pExtiConfig->Line & EXTI_CONFIG) != 0x00U)
  {
    assert_param(IS_EXTI_TRIGGER(pExtiConfig->Trigger));

    /* Configure rising trigger */
    regaddr = (__IO uint32_t *)(&EXTI->RTSR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = *regaddr;

    /* Mask or set line */
    if ((pExtiConfig->Trigger & EXTI_TRIGGER_RISING) != 0x00U)
    {
      regval |= maskline;
    }
    else
    {
      regval &= ~maskline;
    }

    /* Store rising trigger mode */
    *regaddr = regval;

    /* Configure falling trigger */
    regaddr = (__IO uint32_t *)(&EXTI->FTSR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = *regaddr;

    /* Mask or set line */
    if ((pExtiConfig->Trigger & EXTI_TRIGGER_FALLING) != 0x00U)
    {
      regval |= maskline;
    }
    else
    {
      regval &= ~maskline;
    }

    /* Store falling trigger mode */
    *regaddr = regval;

    /* Configure gpio port selection in case of gpio exti line */
    if ((pExtiConfig->Line & EXTI_GPIO) == EXTI_GPIO)
    {
      assert_param(IS_EXTI_GPIO_PORT(pExtiConfig->GPIOSel));
      assert_param(IS_EXTI_GPIO_PIN(linepos));

      regval = SYSCFG->EXTICR[(linepos >> 2U) & 0x03UL];
      regval &= ~(SYSCFG_EXTICR1_EXTI0 << (SYSCFG_EXTICR1_EXTI1_Pos * (linepos & 0x03U)));
      regval |= (pExtiConfig->GPIOSel << (SYSCFG_EXTICR1_EXTI1_Pos * (linepos & 0x03U)));
      SYSCFG->EXTICR[(linepos >> 2U) & 0x03UL] = regval;
    }
  }

  /* Configure interrupt mode : read current mode */
  regaddr = (__IO uint32_t *)(&EXTI->IMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Mask or set line */
  if ((pExtiConfig->Mode & EXTI_MODE_INTERRUPT) != 0x00U)
  {
    regval |= maskline;
  }
  else
  {
    regval &= ~maskline;
  }

  /* Store interrupt mode */
  *regaddr = regval;

  /* The event mode cannot be configured if the line does not support it */
  assert_param(((pExtiConfig->Line & EXTI_EVENT) == EXTI_EVENT) || ((pExtiConfig->Mode & EXTI_MODE_EVENT) != EXTI_MODE_EVENT));

  /* Configure event mode : read current mode */
  regaddr = (__IO uint32_t *)(&EXTI->EMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Mask or set line */
  if ((pExtiConfig->Mode & EXTI_MODE_EVENT) != 0x00U)
  {
    regval |= maskline;
  }
  else
  {
    regval &= ~maskline;
  }

  /* Store event mode */
  *regaddr = regval;

#if defined (DUAL_CORE)
  /* Configure interrupt mode for Core2 : read current mode */
  regaddr = (__IO uint32_t *)(&EXTI->C2IMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Mask or set line */
  if ((pExtiConfig->Mode & EXTI_MODE_CORE2_INTERRUPT) != 0x00U)
  {
    regval |= maskline;
  }
  else
  {
    regval &= ~maskline;
  }

  /* Store interrupt mode */
  *regaddr = regval;

  /* The event mode cannot be configured if the line does not support it */
  assert_param(((pExtiConfig->Line & EXTI_EVENT) == EXTI_EVENT) || ((pExtiConfig->Mode & EXTI_MODE_CORE2_EVENT) != EXTI_MODE_CORE2_EVENT));

  /* Configure event mode : read current mode */
  regaddr = (__IO uint32_t *)(&EXTI->C2EMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Mask or set line */
  if ((pExtiConfig->Mode & EXTI_MODE_CORE2_EVENT) != 0x00U)
  {
    regval |= maskline;
  }
  else
  {
    regval &= ~maskline;
  }

  /* Store event mode */
  *regaddr = regval;
#endif /* DUAL_CORE */

  /* Configure the D3 PendClear source in case of Wakeup target is Any */
  if ((pExtiConfig->Line & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_ALL)
  {
    assert_param(IS_EXTI_D3_PENDCLR_SRC(pExtiConfig->PendClearSource));

    /*Calc the PMR register address for the given line */
    regaddr = (__IO uint32_t *)(&EXTI->D3PMR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = *regaddr;

    if(pExtiConfig->PendClearSource == EXTI_D3_PENDCLR_SRC_NONE)
    {
      /* Clear D3PMRx register for the given line */
      regval &= ~maskline;
      /* Store D3PMRx register value */
      *regaddr = regval;
    }
    else
    {
      /* Set D3PMRx register to 1 for the given line */
      regval |= maskline;
      /* Store D3PMRx register value */
      *regaddr = regval;

      if(linepos < 16UL)
      {
        regaddr = (__IO uint32_t *)(&EXTI->D3PCR1L + (EXTI_CONFIG_OFFSET * offset));
        pcrlinepos = 1UL << linepos;
      }
      else
      {
        regaddr = (__IO uint32_t *)(&EXTI->D3PCR1H + (EXTI_CONFIG_OFFSET * offset));
        pcrlinepos = 1UL << (linepos - 16UL);
      }

      regval = (*regaddr & (~(pcrlinepos * pcrlinepos * 3UL))) | (pcrlinepos * pcrlinepos * (pExtiConfig->PendClearSource - 1UL));
      *regaddr = regval;
    }
  }

  return HAL_OK;
}


/**
  * @brief  Get configuration of a dedicated Exti line.
  * @param  hexti Exti handle.
  * @param  pExtiConfig Pointer on structure to store Exti configuration.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig)
{
  __IO uint32_t *regaddr;
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;
  uint32_t offset;
  uint32_t pcrlinepos;

  /* Check null pointer */
  if ((hexti == NULL) || (pExtiConfig == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the parameter */
  assert_param(IS_EXTI_LINE(hexti->Line));

  /* Store handle line number to configuration structure */
  pExtiConfig->Line = hexti->Line;

  /* compute line register offset and line mask */
  offset = ((pExtiConfig->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  linepos = (pExtiConfig->Line & EXTI_PIN_MASK);
  maskline = (1UL << linepos);

  /* 1] Get core mode : interrupt */
  regaddr = (__IO uint32_t *)(&EXTI->IMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  pExtiConfig->Mode = EXTI_MODE_NONE;

  /* Check if selected line is enable */
  if ((regval & maskline) != 0x00U)
  {
    pExtiConfig->Mode = EXTI_MODE_INTERRUPT;
  }

  /* Get event mode */
  regaddr = (__IO uint32_t *)(&EXTI->EMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Check if selected line is enable */
  if ((regval & maskline) != 0x00U)
  {
    pExtiConfig->Mode |= EXTI_MODE_EVENT;
  }
#if defined (DUAL_CORE)
  regaddr = (__IO uint32_t *)(&EXTI->C2IMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Check if selected line is enable */
  if ((regval & maskline) != 0x00U)
  {
    pExtiConfig->Mode = EXTI_MODE_CORE2_INTERRUPT;
  }

  /* Get event mode */
  regaddr = (__IO uint32_t *)(&EXTI->C2EMR1 + (EXTI_MODE_OFFSET * offset));
  regval = *regaddr;

  /* Check if selected line is enable */
  if ((regval & maskline) != 0x00U)
  {
    pExtiConfig->Mode |= EXTI_MODE_CORE2_EVENT;
  }
#endif /*DUAL_CORE*/

  /* 2] Get trigger for configurable lines : rising */
  if ((pExtiConfig->Line & EXTI_CONFIG) != 0x00U)
  {
    regaddr = (__IO uint32_t *)(&EXTI->RTSR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = *regaddr;

    /* Check if configuration of selected line is enable */
    if ((regval & maskline) != 0x00U)
    {
      pExtiConfig->Trigger = EXTI_TRIGGER_RISING;
    }
    else
    {
      pExtiConfig->Trigger = EXTI_TRIGGER_NONE;
    }

    /* Get falling configuration */
    regaddr = (__IO uint32_t *)(&EXTI->FTSR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = *regaddr;

    /* Check if configuration of selected line is enable */
    if ((regval & maskline) != 0x00U)
    {
      pExtiConfig->Trigger |= EXTI_TRIGGER_FALLING;
    }

    /* Get Gpio port selection for gpio lines */
    if ((pExtiConfig->Line & EXTI_GPIO) == EXTI_GPIO)
    {
      assert_param(IS_EXTI_GPIO_PIN(linepos));

      regval = SYSCFG->EXTICR[(linepos >> 2U) & 0x03UL];
      pExtiConfig->GPIOSel = ((regval << (SYSCFG_EXTICR1_EXTI1_Pos * (3UL - (linepos & 0x03UL)))) >> 24U);
    }
    else
    {
      pExtiConfig->GPIOSel = 0x00U;
    }
  }
  else
  {
    pExtiConfig->Trigger = EXTI_TRIGGER_NONE;
    pExtiConfig->GPIOSel = 0x00U;
  }

  /* 3] Get D3 Pend Clear source */
  if ((pExtiConfig->Line & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_ALL)
  {
    regaddr = (__IO uint32_t *)(&EXTI->D3PMR1 + (EXTI_CONFIG_OFFSET * offset));
    if(((*regaddr) & linepos) == 0UL)
    {
      /* if PMR unset, then no pend clear source is used */
      pExtiConfig->PendClearSource = EXTI_D3_PENDCLR_SRC_NONE;
    }
    else
    {
      /* if wakeup target is any and PMR set, the read pend clear source from  D3PCRxL/H */
      if(linepos < 16UL)
      {
        regaddr = (__IO uint32_t *)(&EXTI->D3PCR1L + (EXTI_CONFIG_OFFSET * offset));
        pcrlinepos = 1UL << linepos;
      }
      else
      {
        regaddr = (__IO uint32_t *)(&EXTI->D3PCR1H + (EXTI_CONFIG_OFFSET * offset));
        pcrlinepos = 1UL << (linepos - 16UL);
      }

      pExtiConfig->PendClearSource = 1UL + ((*regaddr & (pcrlinepos * pcrlinepos * 3UL)) / (pcrlinepos * pcrlinepos));
    }
  }
  else
  {
    /* if line wakeup target is not any, then no pend clear source is used  */
    pExtiConfig->PendClearSource = EXTI_D3_PENDCLR_SRC_NONE;
  }

  return HAL_OK;
}


/**
  * @brief  Clear whole configuration of a dedicated Exti line.
  * @param  hexti Exti handle.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti)
{
  __IO uint32_t *regaddr;
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;
  uint32_t offset;
  uint32_t pcrlinepos;

  /* Check null pointer */
  if (hexti == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameter */
  assert_param(IS_EXTI_LINE(hexti->Line));

  /* compute line register offset and line mask */
  offset = ((hexti->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  linepos = (hexti->Line & EXTI_PIN_MASK);
  maskline = (1UL << linepos);

  /* 1] Clear interrupt mode */
  regaddr = (__IO uint32_t *)(&EXTI->IMR1 + (EXTI_MODE_OFFSET * offset));
  regval = (*regaddr & ~maskline);
  *regaddr = regval;

  /* 2] Clear event mode */
  regaddr = (__IO uint32_t *)(&EXTI->EMR1 + (EXTI_MODE_OFFSET * offset));
  regval = (*regaddr & ~maskline);
  *regaddr = regval;

#if defined (DUAL_CORE)
    /* 1] Clear CM4 interrupt mode */
  regaddr = (__IO uint32_t *)(&EXTI->C2IMR1 + (EXTI_MODE_OFFSET * offset));
  regval = (*regaddr & ~maskline);
  *regaddr = regval;

  /* 2] Clear CM4 event mode */
  regaddr = (__IO uint32_t *)(&EXTI->C2EMR1 + (EXTI_MODE_OFFSET * offset));
  regval = (*regaddr & ~maskline);
  *regaddr = regval;
#endif /* DUAL_CORE */

  /* 3] Clear triggers in case of configurable lines */
  if ((hexti->Line & EXTI_CONFIG) != 0x00U)
  {
    regaddr = (__IO uint32_t *)(&EXTI->RTSR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = (*regaddr & ~maskline);
    *regaddr = regval;

    regaddr = (__IO uint32_t *)(&EXTI->FTSR1 + (EXTI_CONFIG_OFFSET * offset));
    regval = (*regaddr & ~maskline);
    *regaddr = regval;

    /* Get Gpio port selection for gpio lines */
    if ((hexti->Line & EXTI_GPIO) == EXTI_GPIO)
    {
      assert_param(IS_EXTI_GPIO_PIN(linepos));

      regval = SYSCFG->EXTICR[(linepos >> 2U) & 0x03UL];
      regval &= ~(SYSCFG_EXTICR1_EXTI0 << (SYSCFG_EXTICR1_EXTI1_Pos * (linepos & 0x03UL)));
      SYSCFG->EXTICR[(linepos >> 2U) & 0x03UL] = regval;
    }
  }

  /* 4] Clear D3 Config lines */
  if ((hexti->Line & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_ALL)
  {
    regaddr = (__IO uint32_t *)(&EXTI->D3PMR1 + (EXTI_CONFIG_OFFSET * offset));
    *regaddr = (*regaddr & ~maskline);

    if(linepos < 16UL)
    {
      regaddr = (__IO uint32_t *)(&EXTI->D3PCR1L + (EXTI_CONFIG_OFFSET * offset));
      pcrlinepos = 1UL << linepos;
    }
    else
    {
      regaddr = (__IO uint32_t *)(&EXTI->D3PCR1H + (EXTI_CONFIG_OFFSET * offset));
      pcrlinepos = 1UL << (linepos - 16UL);
    }

    /*Clear D3 PendClear source */
    *regaddr &= (~(pcrlinepos * pcrlinepos * 3UL));
  }

  return HAL_OK;
}


/**
  * @brief  Register callback for a dedicated Exti line.
  * @param  hexti Exti handle.
  * @param  CallbackID User callback identifier.
  *         This parameter can be one of @arg @ref EXTI_CallbackIDTypeDef values.
  * @param  pPendingCbfn function pointer to be stored as callback.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void))
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check null pointer */
  if (hexti == NULL)
  {
    return HAL_ERROR;
  }

  switch (CallbackID)
  {
    case  HAL_EXTI_COMMON_CB_ID:
      hexti->PendingCallback = pPendingCbfn;
      break;

    default:
      status = HAL_ERROR;
      break;
  }

  return status;
}


/**
  * @brief  Store line number as handle private field.
  * @param  hexti Exti handle.
  * @param  ExtiLine Exti line number.
  *         This parameter can be from 0 to @ref EXTI_LINE_NB.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine)
{
  /* Check the parameters */
  assert_param(IS_EXTI_LINE(ExtiLine));

  /* Check null pointer */
  if (hexti == NULL)
  {
    return HAL_ERROR;
  }
  else
  {
    /* Store line number as handle private field */
    hexti->Line = ExtiLine;

    return HAL_OK;
  }
}


/**
  * @}
  */

/** @addtogroup EXTI_Exported_Functions_Group2
 *  @brief EXTI IO functions.
 *
@verbatim
 ===============================================================================
                       ##### IO operation functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Handle EXTI interrupt request.
  * @param  hexti Exti handle.
  * @retval none.
  */
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti)
{
  __IO uint32_t *regaddr;
  uint32_t regval;
  uint32_t maskline;
  uint32_t offset;

  /* Compute line register offset and line mask */
  offset = ((hexti->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  maskline = (1UL << (hexti->Line & EXTI_PIN_MASK));

#if defined(DUAL_CORE)
  if (HAL_GetCurrentCPUID() == CM7_CPUID)
  {
    /* Get pending register address */
    regaddr = (__IO uint32_t *)(&EXTI->PR1 + (EXTI_MODE_OFFSET * offset));
  }
  else /* Cortex-M4*/
  {
    /* Get pending register address */
    regaddr = (__IO uint32_t *)(&EXTI->C2PR1 + (EXTI_MODE_OFFSET * offset));
  }
#else
  regaddr = (__IO uint32_t *)(&EXTI->PR1 + (EXTI_MODE_OFFSET * offset));
#endif /* DUAL_CORE */

  /* Get pending bit  */
  regval = (*regaddr & maskline);

  if (regval != 0x00U)
  {
    /* Clear pending bit */
    *regaddr = maskline;

    /* Call callback */
    if (hexti->PendingCallback != NULL)
    {
      hexti->PendingCallback();
    }
  }
}


/**
  * @brief  Get interrupt pending bit of a dedicated line.
  * @param  hexti Exti handle.
  * @param  Edge Specify which pending edge as to be checked.
  *         This parameter can be one of the following values:
  *           @arg @ref EXTI_TRIGGER_RISING_FALLING
  *         This parameter is kept for compatibility with other series.
  * @retval 1 if interrupt is pending else 0.
  */
uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge)
{
  __IO uint32_t *regaddr;
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;
  uint32_t offset;

  /* Check parameters */
  assert_param(IS_EXTI_LINE(hexti->Line));
  assert_param(IS_EXTI_CONFIG_LINE(hexti->Line));
  assert_param(IS_EXTI_PENDING_EDGE(Edge));

  /* compute line register offset and line mask */
  offset = ((hexti->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  linepos = (hexti->Line & EXTI_PIN_MASK);
  maskline = (1UL << linepos);

#if defined(DUAL_CORE)
  if (HAL_GetCurrentCPUID() == CM7_CPUID)
  {
    /* Get pending register address */
    regaddr = (__IO uint32_t *)(&EXTI->PR1 + (EXTI_MODE_OFFSET * offset));
  }
  else /* Cortex-M4 */
  {
    /* Get pending register address */
    regaddr = (__IO uint32_t *)(&EXTI->C2PR1 + (EXTI_MODE_OFFSET * offset));
  }
#else
  regaddr = (__IO uint32_t *)(&EXTI->PR1 + (EXTI_MODE_OFFSET * offset));
#endif /* DUAL_CORE */

  /* return 1 if bit is set else 0 */
  regval = ((*regaddr & maskline) >> linepos);
  return regval;
}


/**
  * @brief  Clear interrupt pending bit of a dedicated line.
  * @param  hexti Exti handle.
  * @param  Edge Specify which pending edge as to be clear.
  *         This parameter can be one of the following values:
  *           @arg @ref EXTI_TRIGGER_RISING_FALLING
  *         This parameter is kept for compatibility with other series.
  * @retval None.
  */
void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge)
{
  __IO uint32_t *regaddr;
  uint32_t maskline;
  uint32_t offset;

  /* Check parameters */
  assert_param(IS_EXTI_LINE(hexti->Line));
  assert_param(IS_EXTI_CONFIG_LINE(hexti->Line));
  assert_param(IS_EXTI_PENDING_EDGE(Edge));

  /* compute line register offset and line mask */
  offset = ((hexti->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  maskline = (1UL << (hexti->Line & EXTI_PIN_MASK));

#if defined(DUAL_CORE)
  if (HAL_GetCurrentCPUID() == CM7_CPUID)
  {
    /* Get pending register address */
    regaddr = (__IO uint32_t *)(&EXTI->PR1 + (EXTI_MODE_OFFSET * offset));
  }
  else /* Cortex-M4 */
  {
    /* Get pending register address */
    regaddr = (__IO uint32_t *)(&EXTI->C2PR1 + (EXTI_MODE_OFFSET * offset));
  }
#else
  regaddr = (__IO uint32_t *)(&EXTI->PR1 + (EXTI_MODE_OFFSET * offset));
#endif /* DUAL_CORE */

  /* Clear Pending bit */
  *regaddr =  maskline;
}

/**
  * @brief  Generate a software interrupt for a dedicated line.
  * @param  hexti Exti handle.
  * @retval None.
  */
void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti)
{
  __IO uint32_t *regaddr;
  uint32_t maskline;
  uint32_t offset;

  /* Check parameters */
  assert_param(IS_EXTI_LINE(hexti->Line));
  assert_param(IS_EXTI_CONFIG_LINE(hexti->Line));

  /* compute line register offset and line mask */
  offset = ((hexti->Line & EXTI_REG_MASK) >> EXTI_REG_SHIFT);
  maskline = (1UL << (hexti->Line & EXTI_PIN_MASK));

  regaddr = (__IO uint32_t *)(&EXTI->SWIER1 + (EXTI_CONFIG_OFFSET * offset));
  *regaddr = maskline;
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_EXTI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
