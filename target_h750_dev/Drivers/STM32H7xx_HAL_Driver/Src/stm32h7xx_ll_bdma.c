/**
  ******************************************************************************
  * @file    stm32h7xx_ll_bdma.c
  * @author  MCD Application Team
  * @brief   BDMA LL module driver.
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

#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_ll_bdma.h"
#include "stm32h7xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (BDMA) || defined (BDMA1) || defined (BDMA2)

/** @addtogroup BDMA_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup BDMA_LL_Private_Macros
  * @{
  */
#define IS_LL_BDMA_DIRECTION(__VALUE__)          (((__VALUE__) == LL_BDMA_DIRECTION_PERIPH_TO_MEMORY) || \
                                                  ((__VALUE__) == LL_BDMA_DIRECTION_MEMORY_TO_PERIPH) || \
                                                  ((__VALUE__) == LL_BDMA_DIRECTION_MEMORY_TO_MEMORY))

#define IS_LL_BDMA_MODE(__VALUE__)               (((__VALUE__) == LL_BDMA_MODE_NORMAL) || \
                                                  ((__VALUE__) == LL_BDMA_MODE_CIRCULAR))

#define IS_LL_BDMA_PERIPHINCMODE(__VALUE__)      (((__VALUE__) == LL_BDMA_PERIPH_INCREMENT) || \
                                                  ((__VALUE__) == LL_BDMA_PERIPH_NOINCREMENT))

#define IS_LL_BDMA_MEMORYINCMODE(__VALUE__)      (((__VALUE__) == LL_BDMA_MEMORY_INCREMENT) || \
                                                  ((__VALUE__) == LL_BDMA_MEMORY_NOINCREMENT))

#define IS_LL_BDMA_PERIPHDATASIZE(__VALUE__)     (((__VALUE__) == LL_BDMA_PDATAALIGN_BYTE)      || \
                                                  ((__VALUE__) == LL_BDMA_PDATAALIGN_HALFWORD)  || \
                                                  ((__VALUE__) == LL_BDMA_PDATAALIGN_WORD))

#define IS_LL_BDMA_MEMORYDATASIZE(__VALUE__)     (((__VALUE__) == LL_BDMA_MDATAALIGN_BYTE)      || \
                                                  ((__VALUE__) == LL_BDMA_MDATAALIGN_HALFWORD)  || \
                                                  ((__VALUE__) == LL_BDMA_MDATAALIGN_WORD))

#define IS_LL_BDMA_NBDATA(__VALUE__)             ((__VALUE__)  <= 0x0000FFFFU)

#if defined(ADC3)
#define IS_LL_BDMA_PERIPHREQUEST(__VALUE__)      ((__VALUE__) <= LL_DMAMUX2_REQ_ADC3)
#else
#define IS_LL_BDMA_PERIPHREQUEST(__VALUE__)      ((__VALUE__) <= LL_DMAMUX2_REQ_DFSDM2_FLT0)
#endif /* ADC3 */

#define IS_LL_BDMA_PRIORITY(__VALUE__)           (((__VALUE__) == LL_BDMA_PRIORITY_LOW)    || \
                                                  ((__VALUE__) == LL_BDMA_PRIORITY_MEDIUM) || \
                                                  ((__VALUE__) == LL_BDMA_PRIORITY_HIGH)   || \
                                                  ((__VALUE__) == LL_BDMA_PRIORITY_VERYHIGH))

#define IS_LL_BDMA_ALL_CHANNEL_INSTANCE(INSTANCE, CHANNEL)  ((((INSTANCE) == BDMA) && \
                                                             (((CHANNEL) == LL_BDMA_CHANNEL_0) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_1) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_2) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_3) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_4) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_5) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_6) || \
                                                              ((CHANNEL) == LL_BDMA_CHANNEL_7))))

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup BDMA_LL_Exported_Functions
  * @{
  */

/** @addtogroup BDMA_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the DMA registers to their default reset values.
  * @param  BDMAx BDMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_BDMA_CHANNEL_0
  *         @arg @ref LL_BDMA_CHANNEL_1
  *         @arg @ref LL_BDMA_CHANNEL_2
  *         @arg @ref LL_BDMA_CHANNEL_3
  *         @arg @ref LL_BDMA_CHANNEL_4
  *         @arg @ref LL_BDMA_CHANNEL_5
  *         @arg @ref LL_BDMA_CHANNEL_6
  *         @arg @ref LL_BDMA_CHANNEL_7
  *         @arg @ref LL_BDMA_CHANNEL_ALL
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: DMA registers are de-initialized
  *          - ERROR: DMA registers are not de-initialized
  */
uint32_t LL_BDMA_DeInit(BDMA_TypeDef *BDMAx, uint32_t Channel)
{
  BDMA_Channel_TypeDef *tmp ;
  ErrorStatus status = SUCCESS;

  /* Check the DMA Instance DMAx and Channel parameters */
  assert_param(IS_LL_BDMA_ALL_CHANNEL_INSTANCE(BDMAx, Channel) || (Channel == LL_BDMA_CHANNEL_ALL));

  if (Channel == LL_BDMA_CHANNEL_ALL)
  {
    if (BDMAx == BDMA)
    {
      /* Force reset of BDMA clock */
      LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_DMA1);

      /* Release reset of BDMA clock */
      LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_DMA1);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    tmp = (BDMA_Channel_TypeDef *)(__LL_BDMA_GET_CHANNEL_INSTANCE(BDMAx, Channel));

    /* Disable the selected DMAx_Channely */
    CLEAR_BIT(tmp->CCR, BDMA_CCR_EN);

    /* Reset DMAx_Channely control register */
    LL_BDMA_WriteReg(tmp, CCR, 0U);

    /* Reset DMAx_Channely remaining bytes register */
    LL_BDMA_WriteReg(tmp, CNDTR, 0U);

    /* Reset DMAx_Channely peripheral address register */
    LL_BDMA_WriteReg(tmp, CPAR, 0U);

    /* Reset DMAx_Channely memory 0 address register */
    LL_BDMA_WriteReg(tmp, CM0AR, 0U);

    /* Reset DMAx_Channely memory 1 address register */
    LL_BDMA_WriteReg(tmp, CM1AR, 0U);

    /* Reset Request register field for BDMAx Channel */
    LL_BDMA_SetPeriphRequest(BDMAx, Channel, LL_DMAMUX2_REQ_MEM2MEM);

    if (Channel == LL_BDMA_CHANNEL_0)
    {
      /* Reset interrupt pending bits for DMAx Channel0 */
      LL_BDMA_ClearFlag_GI0(BDMAx);
    }
    else if (Channel == LL_BDMA_CHANNEL_1)
    {
      /* Reset interrupt pending bits for DMAx Channel1 */
      LL_BDMA_ClearFlag_GI1(BDMAx);
    }
    else if (Channel == LL_BDMA_CHANNEL_2)
    {
      /* Reset interrupt pending bits for DMAx Channel2 */
      LL_BDMA_ClearFlag_GI2(BDMAx);
    }
    else if (Channel == LL_BDMA_CHANNEL_3)
    {
      /* Reset interrupt pending bits for DMAx Channel3 */
      LL_BDMA_ClearFlag_GI3(BDMAx);
    }
    else if (Channel == LL_BDMA_CHANNEL_4)
    {
      /* Reset interrupt pending bits for DMAx Channel4 */
      LL_BDMA_ClearFlag_GI4(BDMAx);
    }
    else if (Channel == LL_BDMA_CHANNEL_5)
    {
      /* Reset interrupt pending bits for DMAx Channel5 */
      LL_BDMA_ClearFlag_GI5(BDMAx);
    }

    else if (Channel == LL_BDMA_CHANNEL_6)
    {
      /* Reset interrupt pending bits for DMAx Channel6 */
      LL_BDMA_ClearFlag_GI6(BDMAx);
    }
    else if (Channel == LL_BDMA_CHANNEL_7)
    {
      /* Reset interrupt pending bits for DMAx Channel7 */
      LL_BDMA_ClearFlag_GI7(BDMAx);
    }
    else
    {
      status = ERROR;
    }
  }

  return (uint32_t)status;
}

/**
  * @brief  Initialize the BDMA registers according to the specified parameters in BDMA_InitStruct.
  * @note   To convert BDMAx_Channely Instance to BDMAx Instance and Channely, use helper macros :
  *         @arg @ref __LL_BDMA_GET_INSTANCE
  *         @arg @ref __LL_BDMA_GET_CHANNEL
  * @param  BDMAx BDMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_BDMA_CHANNEL_0
  *         @arg @ref LL_BDMA_CHANNEL_1
  *         @arg @ref LL_BDMA_CHANNEL_2
  *         @arg @ref LL_BDMA_CHANNEL_3
  *         @arg @ref LL_BDMA_CHANNEL_4
  *         @arg @ref LL_BDMA_CHANNEL_5
  *         @arg @ref LL_BDMA_CHANNEL_6
  *         @arg @ref LL_BDMA_CHANNEL_7
  * @param  BDMA_InitStruct pointer to a @ref LL_BDMA_InitTypeDef structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: DMA registers are initialized
  *          - ERROR: Not applicable
  */
uint32_t LL_BDMA_Init(BDMA_TypeDef *BDMAx, uint32_t Channel, LL_BDMA_InitTypeDef *BDMA_InitStruct)
{
  /* Check the DMA Instance DMAx and Channel parameters */
  assert_param(IS_LL_BDMA_ALL_CHANNEL_INSTANCE(BDMAx, Channel));

  /* Check the DMA parameters from BDMA_InitStruct */
  assert_param(IS_LL_BDMA_DIRECTION(BDMA_InitStruct->Direction));
  assert_param(IS_LL_BDMA_MODE(BDMA_InitStruct->Mode));
  assert_param(IS_LL_BDMA_PERIPHINCMODE(BDMA_InitStruct->PeriphOrM2MSrcIncMode));
  assert_param(IS_LL_BDMA_MEMORYINCMODE(BDMA_InitStruct->MemoryOrM2MDstIncMode));
  assert_param(IS_LL_BDMA_PERIPHDATASIZE(BDMA_InitStruct->PeriphOrM2MSrcDataSize));
  assert_param(IS_LL_BDMA_MEMORYDATASIZE(BDMA_InitStruct->MemoryOrM2MDstDataSize));
  assert_param(IS_LL_BDMA_NBDATA(BDMA_InitStruct->NbData));
  assert_param(IS_LL_BDMA_PERIPHREQUEST(BDMA_InitStruct->PeriphRequest));
  assert_param(IS_LL_BDMA_PRIORITY(BDMA_InitStruct->Priority));

  /*---------------------------- DMAx CCR Configuration ------------------------
   * Configure DMAx_Channely: data transfer direction, data transfer mode,
   *                          peripheral and memory increment mode,
   *                          data size alignment and  priority level with parameters :
   * - Direction:      BDMA_CCR_DIR and BDMA_CCR_MEM2MEM bits
   * - Mode:           BDMA_CCR_CIRC bit
   * - PeriphOrM2MSrcIncMode:  BDMA_CCR_PINC bit
   * - MemoryOrM2MDstIncMode:  BDMA_CCR_MINC bit
   * - PeriphOrM2MSrcDataSize: BDMA_CCR_PSIZE[1:0] bits
   * - MemoryOrM2MDstDataSize: BDMA_CCR_MSIZE[1:0] bits
   * - Priority:               BDMA_CCR_PL[1:0] bits
   */
  LL_BDMA_ConfigTransfer(BDMAx, Channel, BDMA_InitStruct->Direction              | \
                         BDMA_InitStruct->Mode                   | \
                         BDMA_InitStruct->PeriphOrM2MSrcIncMode  | \
                         BDMA_InitStruct->MemoryOrM2MDstIncMode  | \
                         BDMA_InitStruct->PeriphOrM2MSrcDataSize | \
                         BDMA_InitStruct->MemoryOrM2MDstDataSize | \
                         BDMA_InitStruct->Priority);

  /*-------------------------- DMAx CMAR Configuration -------------------------
   * Configure the memory or destination base address with parameter :
   * - MemoryOrM2MDstAddress: BDMA_CMAR_MA[31:0] bits
   */
  LL_BDMA_SetMemoryAddress(BDMAx, Channel, BDMA_InitStruct->MemoryOrM2MDstAddress);

  /*-------------------------- DMAx CPAR Configuration -------------------------
   * Configure the peripheral or source base address with parameter :
   * - PeriphOrM2MSrcAddress: BDMA_CPAR_PA[31:0] bits
   */
  LL_BDMA_SetPeriphAddress(BDMAx, Channel, BDMA_InitStruct->PeriphOrM2MSrcAddress);

  /*--------------------------- DMAx CNDTR Configuration -----------------------
   * Configure the peripheral base address with parameter :
   * - NbData: BDMA_CNDTR_NDT[15:0] bits
   */
  LL_BDMA_SetDataLength(BDMAx, Channel, BDMA_InitStruct->NbData);

  /*--------------------------- DMAMUXx CCR Configuration ----------------------
   * Configure the DMA request for DMA Channels on DMAMUX Channel x with parameter :
   * - PeriphRequest: BDMA_CxCR[7:0] bits
   */
  LL_BDMA_SetPeriphRequest(BDMAx, Channel, BDMA_InitStruct->PeriphRequest);

  return (uint32_t)SUCCESS;
}

/**
  * @brief  Set each @ref LL_BDMA_InitTypeDef field to default value.
  * @param  BDMA_InitStruct Pointer to a @ref LL_BDMA_InitTypeDef structure.
  * @retval None
  */
void LL_BDMA_StructInit(LL_BDMA_InitTypeDef *BDMA_InitStruct)
{
  /* Set BDMA_InitStruct fields to default values */
  BDMA_InitStruct->PeriphOrM2MSrcAddress  = 0x00000000U;
  BDMA_InitStruct->MemoryOrM2MDstAddress  = 0x00000000U;
  BDMA_InitStruct->Direction              = LL_BDMA_DIRECTION_PERIPH_TO_MEMORY;
  BDMA_InitStruct->Mode                   = LL_BDMA_MODE_NORMAL;
  BDMA_InitStruct->PeriphOrM2MSrcIncMode  = LL_BDMA_PERIPH_NOINCREMENT;
  BDMA_InitStruct->MemoryOrM2MDstIncMode  = LL_BDMA_MEMORY_NOINCREMENT;
  BDMA_InitStruct->PeriphOrM2MSrcDataSize = LL_BDMA_PDATAALIGN_BYTE;
  BDMA_InitStruct->MemoryOrM2MDstDataSize = LL_BDMA_MDATAALIGN_BYTE;
  BDMA_InitStruct->NbData                 = 0x00000000U;
  BDMA_InitStruct->PeriphRequest          = LL_DMAMUX2_REQ_MEM2MEM;
  BDMA_InitStruct->Priority               = LL_BDMA_PRIORITY_LOW;
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

#endif /* BDMA || BDMA1 || BDMA2 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
