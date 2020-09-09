/**
  ******************************************************************************
  * @file    stm32h7xx_ll_rng.c
  * @author  MCD Application Team
  * @brief   RNG LL module driver.
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
#include "stm32h7xx_ll_rng.h"
#include "stm32h7xx_ll_bus.h"

#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (RNG)

/** @addtogroup RNG_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup RNG_LL_Private_Macros
  * @{
  */
#define IS_LL_RNG_CED(__MODE__) (((__MODE__) == LL_RNG_CED_ENABLE) || \
                                 ((__MODE__) == LL_RNG_CED_DISABLE))

#if defined(RNG_CR_CONDRST)
#define IS_LL_RNG_CLOCK_DIVIDER(__CLOCK_DIV__) ((__CLOCK_DIV__) <=0x0Fu)


#define IS_LL_RNG_NIST_COMPLIANCE(__NIST_COMPLIANCE__) (((__NIST_COMPLIANCE__) == LL_RNG_NIST_COMPLIANT) || \
                                                     ((__NIST_COMPLIANCE__) == LL_RNG_NOTNIST_COMPLIANT))

#define IS_LL_RNG_CONFIG1 (__CONFIG1__) ((__CONFIG1__) <= 0x3FUL)

#define IS_LL_RNG_CONFIG2 (__CONFIG2__) ((__CONFIG2__) <= 0x07UL)

#define IS_LL_RNG_CONFIG3 (__CONFIG3__) ((__CONFIG3__) <= 0xFUL)
#endif /* RNG_CR_CONDRST*/
/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup RNG_LL_Exported_Functions
  * @{
  */

/** @addtogroup RNG_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize RNG registers (Registers restored to their default values).
  * @param  RNGx RNG Instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: RNG registers are de-initialized
  *          - ERROR: not applicable
  */
ErrorStatus LL_RNG_DeInit(RNG_TypeDef *RNGx)
{
  /* Check the parameters */
  assert_param(IS_RNG_ALL_INSTANCE(RNGx));
  /* Enable RNG reset state */
  LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_RNG);

  /* Release RNG from reset state */
  LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_RNG);
  return (SUCCESS);
}

/**
  * @brief  Initialize RNG registers according to the specified parameters in RNG_InitStruct.
  * @param  RNGx RNG Instance
  * @param  RNG_InitStruct pointer to a LL_RNG_InitTypeDef structure
  *         that contains the configuration information for the specified RNG peripheral.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: RNG registers are initialized according to RNG_InitStruct content
  *          - ERROR: not applicable
  */
ErrorStatus LL_RNG_Init(RNG_TypeDef *RNGx, LL_RNG_InitTypeDef *RNG_InitStruct)
{
  /* Check the parameters */
  assert_param(IS_RNG_ALL_INSTANCE(RNGx));
  assert_param(IS_LL_RNG_CED(RNG_InitStruct->ClockErrorDetection));

#if defined(RNG_CR_CONDRST)
  /* Clock Error Detection Configuration when CONDRT bit is set to 1 */
  MODIFY_REG(RNGx->CR, RNG_CR_CED | RNG_CR_CONDRST, RNG_InitStruct->ClockErrorDetection | RNG_CR_CONDRST);
  /* Writing bits CONDRST=0*/
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
#else
  /* Clock Error Detection configuration */
  MODIFY_REG(RNGx->CR, RNG_CR_CED, RNG_InitStruct->ClockErrorDetection);
#endif /* RNG_CR_CONDRST */

  return (SUCCESS);
}

/**
  * @brief Set each @ref LL_RNG_InitTypeDef field to default value.
  * @param RNG_InitStruct pointer to a @ref LL_RNG_InitTypeDef structure
  *                       whose fields will be set to default values.
  * @retval None
  */
void LL_RNG_StructInit(LL_RNG_InitTypeDef *RNG_InitStruct)
{
  /* Set RNG_InitStruct fields to default values */
  RNG_InitStruct->ClockErrorDetection = LL_RNG_CED_ENABLE;

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

#endif /* RNG */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

