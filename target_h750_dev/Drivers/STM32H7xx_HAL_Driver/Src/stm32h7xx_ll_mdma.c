/**
  ******************************************************************************
  * @file    stm32h7xx_ll_mdma.c
  * @author  MCD Application Team
  * @brief   MDMA LL module driver.
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
#include "stm32h7xx_ll_mdma.h"
#include "stm32h7xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (MDMA)

/** @defgroup MDMA_LL MDMA
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup MDMA_LL_Private_Macros
  * @{
  */

#define IS_LL_MDMA_ALL_CHANNEL_INSTANCE(INSTANCE, CHANNEL) (((INSTANCE) == MDMA) && \
                                                            (((CHANNEL) == LL_MDMA_CHANNEL_0) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_1) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_2) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_3) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_4) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_5) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_6) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_7) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_8) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_9) || \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_10)|| \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_11)|| \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_12)|| \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_13)|| \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_14)|| \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_15)|| \
                                                             ((CHANNEL) == LL_MDMA_CHANNEL_ALL)))

#define IS_LL_MDMA_BLK_DATALENGTH(__VALUE__)              ((__VALUE__)  <= 0x00010000U)

#define IS_LL_MDMA_BLK_REPEATCOUNT(__VALUE__)             ((__VALUE__)  <= 0x00000FFFU)

#define IS_LL_MDMA_WORDENDIANESS(__VALUE__)               (((__VALUE__) == LL_MDMA_WORD_ENDIANNESS_PRESERVE) || \
                                                           ((__VALUE__) == LL_MDMA_WORD_ENDIANNESS_EXCHANGE))

#define IS_LL_MDMA_HALFWORDENDIANESS(__VALUE__)           (((__VALUE__) == LL_MDMA_HALFWORD_ENDIANNESS_PRESERVE) || \
                                                           ((__VALUE__) == LL_MDMA_HALFWORD_ENDIANNESS_EXCHANGE))

#define IS_LL_MDMA_BYTEENDIANESS(__VALUE__)               (((__VALUE__) == LL_MDMA_BYTE_ENDIANNESS_PRESERVE) || \
                                                           ((__VALUE__) == LL_MDMA_BYTE_ENDIANNESS_EXCHANGE))

#define IS_LL_MDMA_PRIORITY(__VALUE__)                    (((__VALUE__) == LL_MDMA_PRIORITY_LOW)    || \
                                                           ((__VALUE__) == LL_MDMA_PRIORITY_MEDIUM) || \
                                                           ((__VALUE__) == LL_MDMA_PRIORITY_HIGH)   || \
                                                           ((__VALUE__) == LL_MDMA_PRIORITY_VERYHIGH))

#define IS_LL_MDMA_BUFFWRITEMODE(__VALUE__)               (((__VALUE__) == LL_MDMA_BUFF_WRITE_DISABLE) || \
                                                           ((__VALUE__) == LL_MDMA_BUFF_WRITE_ENABLE))

#define IS_LL_MDMA_REQUESTMODE(__VALUE__)                 (((__VALUE__) == LL_MDMA_REQUEST_MODE_HW) || \
                                                           ((__VALUE__) == LL_MDMA_REQUEST_MODE_SW))

#define IS_LL_MDMA_TRIGGERMODE(__VALUE__)                 (((__VALUE__) == LL_MDMA_BUFFER_TRANSFER)       || \
                                                           ((__VALUE__) == LL_MDMA_BLOCK_TRANSFER)        || \
                                                           ((__VALUE__) == LL_MDMA_REPEAT_BLOCK_TRANSFER) || \
                                                           ((__VALUE__) == LL_MDMA_FULL_TRANSFER))

#define IS_LL_MDMA_PADDINGALIGNEMENT(__VALUE__)           (((__VALUE__) == LL_MDMA_DATAALIGN_RIGHT)        || \
                                                           ((__VALUE__) == LL_MDMA_DATAALIGN_RIGHT_SIGNED) || \
                                                           ((__VALUE__) == LL_MDMA_DATAALIGN_LEFT))

#define IS_LL_MDMA_PACKMODE(__VALUE__)                    (((__VALUE__) == LL_MDMA_PACK_DISABLE) || \
                                                           ((__VALUE__) == LL_MDMA_PACK_ENABLE))

#define IS_LL_MDMA_BUFFER_XFERLENGTH(__VALUE__)           ((__VALUE__)  <= 0x0000007FU)

#define IS_LL_MDMA_DESTBURST(__VALUE__)                   (((__VALUE__) == LL_MDMA_DEST_BURST_SINGLE) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_2BEATS) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_4BEATS) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_8BEATS) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_16BEATS)|| \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_32BEATS)|| \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_64BEATS)|| \
                                                           ((__VALUE__) == LL_MDMA_DEST_BURST_128BEATS))

#define IS_LL_MDMA_SRCTBURST(__VALUE__)                   (((__VALUE__) == LL_MDMA_SRC_BURST_SINGLE) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_2BEATS) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_4BEATS) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_8BEATS) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_16BEATS)|| \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_32BEATS)|| \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_64BEATS)|| \
                                                           ((__VALUE__) == LL_MDMA_SRC_BURST_128BEATS))

#define IS_LL_MDMA_DESTINCSIZE(__VALUE__)                 (((__VALUE__) == LL_MDMA_DEST_INC_OFFSET_BYTE) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_INC_OFFSET_HALFWORD) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_INC_OFFSET_WORD) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_INC_OFFSET_DOUBLEWORD))

#define IS_LL_MDMA_SRCINCSIZE(__VALUE__)                  (((__VALUE__) == LL_MDMA_SRC_INC_OFFSET_BYTE)     || \
                                                           ((__VALUE__) == LL_MDMA_SRC_INC_OFFSET_HALFWORD) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_INC_OFFSET_WORD)     || \
                                                           ((__VALUE__) == LL_MDMA_SRC_INC_OFFSET_DOUBLEWORD))

#define IS_LL_MDMA_DESTDATASIZE(__VALUE__)                (((__VALUE__) == LL_MDMA_DEST_DATA_SIZE_BYTE) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_DATA_SIZE_HALFWORD) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_DATA_SIZE_WORD) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_DATA_SIZE_DOUBLEWORD))

#define IS_LL_MDMA_SRCDATASIZE(__VALUE__)                 (((__VALUE__) == LL_MDMA_SRC_DATA_SIZE_BYTE) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_DATA_SIZE_HALFWORD) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_DATA_SIZE_WORD) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_DATA_SIZE_DOUBLEWORD))

#define IS_LL_MDMA_DESTINCMODE(__VALUE__)                 (((__VALUE__) == LL_MDMA_DEST_FIXED) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_INCREMENT) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_DECREMENT))

#define IS_LL_MDMA_SRCINCMODE(__VALUE__)                  (((__VALUE__) == LL_MDMA_SRC_FIXED) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_INCREMENT) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_DECREMENT))

#define IS_LL_MDMA_BLKRPT_DEST_ADDRUPDATEMODE(__VALUE__)  (((__VALUE__) == LL_MDMA_BLK_RPT_DEST_ADDR_INCREMENT) || \
                                                           ((__VALUE__) == LL_MDMA_BLK_RPT_DEST_ADDR_DECREMENT))


#define IS_LL_MDMA_BLKRPT_SRC_ADDRUPDATEMODE(__VALUE__)   (((__VALUE__) == LL_MDMA_BLK_RPT_SRC_ADDR_INCREMENT) || \
                                                           ((__VALUE__) == LL_MDMA_BLK_RPT_SRC_ADDR_DECREMENT))

#define IS_LL_MDMA_BLKRPT_DEST_ADDRUPDATEVAL(__VALUE__)   ((__VALUE__)  <= 0x0000FFFFU)

#define IS_LL_MDMA_BLKRPT_SRC_ADDRUPDATEVAL(__VALUE__)    ((__VALUE__)  <= 0x0000FFFFU)

#define IS_LL_MDMA_DEST_BUS(__VALUE__)                    (((__VALUE__) == LL_MDMA_DEST_BUS_SYSTEM_AXI) || \
                                                           ((__VALUE__) == LL_MDMA_DEST_BUS_AHB_TCM))

#define IS_LL_MDMA_SRC_BUS(__VALUE__)                     (((__VALUE__) == LL_MDMA_SRC_BUS_SYSTEM_AXI) || \
                                                           ((__VALUE__) == LL_MDMA_SRC_BUS_AHB_TCM))
#if defined (QUADSPI) && defined (JPEG) && defined (DSI) /* STM32H747/57 devices */
#define IS_LL_MDMA_HWTRIGGER(__VALUE__)                   (((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_LTDC_LINE_IT)         || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_INFIFO_TH)       || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_INFIFO_NF)       || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_OUTFIFO_TH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_OUTFIFO_NE)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_END_CONVERSION)  || \
                                                           ((__VALUE__) == LL_MDMA_REQ_QUADSPI_FIFO_TH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_QUADSPI_TC)           || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_CLUT_TC)        || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TC)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TW)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DSI_TEARING_EFFECT)   || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DSI_END_REFRESH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_END_DATA)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER) || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_COMMAND_END))
#elif defined (QUADSPI) && defined (JPEG) /* STM32H743/53/45/55 devices */
#define IS_LL_MDMA_HWTRIGGER(__VALUE__)                   (((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_LTDC_LINE_IT)         || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_INFIFO_TH)       || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_INFIFO_NF)       || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_OUTFIFO_TH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_OUTFIFO_NE)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_END_CONVERSION)  || \
                                                           ((__VALUE__) == LL_MDMA_REQ_QUADSPI_FIFO_TH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_QUADSPI_TC)           || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_CLUT_TC)        || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TC)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TW)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_END_DATA)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER) || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_COMMAND_END))
#elif defined (QUADSPI) /* STM32H742 devices */
#define IS_LL_MDMA_HWTRIGGER(__VALUE__)                   (((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_QUADSPI_FIFO_TH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_QUADSPI_TC)           || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_CLUT_TC)        || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TC)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TW)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_END_DATA)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER) || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_COMMAND_END))

#else /* STM32H7A3/B3 devices */
#define IS_LL_MDMA_HWTRIGGER(__VALUE__)                   (((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA1_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM0_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM1_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM2_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM3_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM4_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM5_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM6_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2_STREAM7_TC)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_LTDC_LINE_IT)         || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_INFIFO_TH)       || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_INFIFO_NF)       || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_OUTFIFO_TH)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_OUTFIFO_NE)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_JPEG_END_CONVERSION)  || \
                                                           ((__VALUE__) == LL_MDMA_REQ_OCTOSPI1_FIFO_TH)     || \
                                                           ((__VALUE__) == LL_MDMA_REQ_OCTOSPI1_TC)          || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_CLUT_TC)        || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TC)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_DMA2D_TW)             || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_END_DATA)      || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER) || \
                                                           ((__VALUE__) == LL_MDMA_REQ_SDMMC1_COMMAND_END)   || \
                                                           ((__VALUE__) == LL_MDMA_REQ_OCTOSPI2_FIFO_TH)     || \
                                                           ((__VALUE__) == LL_MDMA_REQ_OCTOSPI2_TC))

#endif /* QUADSPI && JPEG && DSI */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup MDMA_LL_Exported_Functions
  * @{
  */

/** @addtogroup MDMA_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the MDMA registers to their default reset values.
  * @param  MDMAx MDMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_CHANNEL_0
  *         @arg @ref LL_MDMA_CHANNEL_1
  *         @arg @ref LL_MDMA_CHANNEL_2
  *         @arg @ref LL_MDMA_CHANNEL_3
  *         @arg @ref LL_MDMA_CHANNEL_4
  *         @arg @ref LL_MDMA_CHANNEL_5
  *         @arg @ref LL_MDMA_CHANNEL_6
  *         @arg @ref LL_MDMA_CHANNEL_7
  *         @arg @ref LL_MDMA_CHANNEL_8
  *         @arg @ref LL_MDMA_CHANNEL_9
  *         @arg @ref LL_MDMA_CHANNEL_10
  *         @arg @ref LL_MDMA_CHANNEL_11
  *         @arg @ref LL_MDMA_CHANNEL_12
  *         @arg @ref LL_MDMA_CHANNEL_13
  *         @arg @ref LL_MDMA_CHANNEL_14
  *         @arg @ref LL_MDMA_CHANNEL_15
  *         @arg @ref LL_MDMA_CHANNEL_ALL
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: MDMA registers are de-initialized
  *          - ERROR: Not applicable
  */
uint32_t LL_MDMA_DeInit(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  MDMA_Channel_TypeDef *tmp;
  ErrorStatus status = SUCCESS;

  /* Check the MDMA Instance MDMAx and Channel parameters*/
  assert_param(IS_LL_MDMA_ALL_CHANNEL_INSTANCE(MDMAx, Channel));

  if (Channel == LL_MDMA_CHANNEL_ALL)
  {
    LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_MDMA);
    LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_MDMA);
  }
  else
  {
    /* Disable the selected Channel */
    LL_MDMA_DisableChannel(MDMAx,Channel);

    /* Get the MDMA Channel Instance */
    tmp = (MDMA_Channel_TypeDef *)(LL_MDMA_GET_CHANNEL_INSTANCE(MDMAx, Channel));

    /* Reset MDMAx_Channely control register */
    LL_MDMA_WriteReg(tmp, CCR, 0U);

    /* Reset MDMAx_Channely Configuration register */
    LL_MDMA_WriteReg(tmp, CTCR, 0U);

    /* Reset MDMAx_Channely block number of data register */
    LL_MDMA_WriteReg(tmp, CBNDTR, 0U);

    /* Reset MDMAx_Channely source address register */
    LL_MDMA_WriteReg(tmp, CSAR, 0U);

    /* Reset MDMAx_Channely destination address register */
    LL_MDMA_WriteReg(tmp, CDAR, 0U);

    /* Reset MDMAx_Channely Block Repeat address Update register */
    LL_MDMA_WriteReg(tmp, CBRUR, 0U);

    /* Reset MDMAx_Channely Link Address register */
    LL_MDMA_WriteReg(tmp, CLAR, 0U);

    /* Reset MDMAx_Channely Trigger and Bus selection register */
    LL_MDMA_WriteReg(tmp, CTBR, 0U);

    /* Reset MDMAx_Channely Mask address register */
    LL_MDMA_WriteReg(tmp, CMAR, 0U);

    /* Reset MDMAx_Channely Mask Data register */
    LL_MDMA_WriteReg(tmp, CMDR, 0U);

    /* Reset the Channel pending flags */
    LL_MDMA_WriteReg(tmp, CIFCR, 0x0000001FU);
  }

  return (uint32_t)status;
}

/**
  * @brief  Initialize the MDMA registers according to the specified parameters in MDMA_InitStruct.
  * @note   To convert MDMAx_Channely Instance to MDMAx Instance and Channely, use helper macros :
  *         @arg @ref LL_MDMA_GET_INSTANCE
  *         @arg @ref LL_MDMA_GET_CHANNEL
  * @param  MDMAx MDMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_CHANNEL_0
  *         @arg @ref LL_MDMA_CHANNEL_1
  *         @arg @ref LL_MDMA_CHANNEL_2
  *         @arg @ref LL_MDMA_CHANNEL_3
  *         @arg @ref LL_MDMA_CHANNEL_4
  *         @arg @ref LL_MDMA_CHANNEL_5
  *         @arg @ref LL_MDMA_CHANNEL_6
  *         @arg @ref LL_MDMA_CHANNEL_7
  *         @arg @ref LL_MDMA_CHANNEL_8
  *         @arg @ref LL_MDMA_CHANNEL_9
  *         @arg @ref LL_MDMA_CHANNEL_10
  *         @arg @ref LL_MDMA_CHANNEL_11
  *         @arg @ref LL_MDMA_CHANNEL_12
  *         @arg @ref LL_MDMA_CHANNEL_13
  *         @arg @ref LL_MDMA_CHANNEL_14
  *         @arg @ref LL_MDMA_CHANNEL_15
  * @param  MDMA_InitStruct pointer to a @ref LL_MDMA_InitTypeDef structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: MDMA registers are initialized
  *          - ERROR: Not applicable
  */
uint32_t LL_MDMA_Init(MDMA_TypeDef *MDMAx, uint32_t Channel, LL_MDMA_InitTypeDef *MDMA_InitStruct)
{
  /* Check the MDMA Instance MDMAx and Channel parameters*/
  assert_param(IS_LL_MDMA_ALL_CHANNEL_INSTANCE(MDMAx, Channel));

  /* Check the MDMA parameters from MDMA_InitStruct */
  assert_param(IS_LL_MDMA_BLK_DATALENGTH(MDMA_InitStruct->BlockDataLength));
  assert_param(IS_LL_MDMA_BLK_REPEATCOUNT(MDMA_InitStruct->BlockRepeatCount));
  assert_param(IS_LL_MDMA_WORDENDIANESS(MDMA_InitStruct->WordEndianess));
  assert_param(IS_LL_MDMA_HALFWORDENDIANESS(MDMA_InitStruct->HalfWordEndianess));
  assert_param(IS_LL_MDMA_BYTEENDIANESS(MDMA_InitStruct->ByteEndianess));
  assert_param(IS_LL_MDMA_PRIORITY(MDMA_InitStruct->Priority));
  assert_param(IS_LL_MDMA_BUFFWRITEMODE(MDMA_InitStruct->BufferableWriteMode));
  assert_param(IS_LL_MDMA_REQUESTMODE(MDMA_InitStruct->RequestMode));
  assert_param(IS_LL_MDMA_TRIGGERMODE(MDMA_InitStruct->TriggerMode));
  assert_param(IS_LL_MDMA_PADDINGALIGNEMENT(MDMA_InitStruct->PaddingAlignment));
  assert_param(IS_LL_MDMA_PACKMODE(MDMA_InitStruct->PackMode));
  assert_param(IS_LL_MDMA_BUFFER_XFERLENGTH(MDMA_InitStruct->BufferTransferLength));
  assert_param(IS_LL_MDMA_DESTBURST(MDMA_InitStruct->DestBurst));
  assert_param(IS_LL_MDMA_SRCTBURST(MDMA_InitStruct->SrctBurst));
  assert_param(IS_LL_MDMA_DESTINCSIZE(MDMA_InitStruct->DestIncSize));
  assert_param(IS_LL_MDMA_SRCINCSIZE(MDMA_InitStruct->SrcIncSize));
  assert_param(IS_LL_MDMA_DESTDATASIZE(MDMA_InitStruct->DestDataSize));
  assert_param(IS_LL_MDMA_SRCDATASIZE(MDMA_InitStruct->SrcDataSize));
  assert_param(IS_LL_MDMA_DESTINCMODE(MDMA_InitStruct->DestIncMode));
  assert_param(IS_LL_MDMA_SRCINCMODE(MDMA_InitStruct->SrcIncMode));
  assert_param(IS_LL_MDMA_BLKRPT_DEST_ADDRUPDATEMODE(MDMA_InitStruct->BlockRepeatDestAddrUpdateMode));
  assert_param(IS_LL_MDMA_BLKRPT_SRC_ADDRUPDATEMODE(MDMA_InitStruct->BlockRepeatSrcAddrUpdateMode));
  assert_param(IS_LL_MDMA_BLKRPT_DEST_ADDRUPDATEVAL(MDMA_InitStruct->BlockRepeatDestAddrUpdateVal));
  assert_param(IS_LL_MDMA_BLKRPT_SRC_ADDRUPDATEVAL(MDMA_InitStruct->BlockRepeatSrcAddrUpdateVal));
  assert_param(IS_LL_MDMA_DEST_BUS(MDMA_InitStruct->DestBus));
  assert_param(IS_LL_MDMA_SRC_BUS(MDMA_InitStruct->SrcBus));
  assert_param(IS_LL_MDMA_HWTRIGGER(MDMA_InitStruct->HWTrigger));


  /*-------------------------- MDMAx CCR Configuration --------------------------
   * Configure the Transfer endianness na priority with parameter :
   * - WordEndianess:     MDMA_CCR_WEX[14] bit
   * - HalfWordEndianess: MDMA_CCR_HEX[13] bit
   * - WordEndianess:     MDMA_CCR_BEX[12] bit
   * - Priority:          MDMA_CCR_BEX[7:6] bits
   */
  LL_MDMA_ConfigXferEndianness(MDMAx, Channel, MDMA_InitStruct->WordEndianess     | \
                                               MDMA_InitStruct->HalfWordEndianess | \
                                               MDMA_InitStruct->ByteEndianess);

  LL_MDMA_SetChannelPriorityLevel(MDMAx, Channel, MDMA_InitStruct->Priority);

  /*-------------------------- MDMAx CTCR Configuration --------------------------
   * Configure the Transfer  parameter :
   * - BufferableWriteMode:     MDMA_CTCR_BWM[31] bit
   * - RequestMode:             MDMA_CTCR_SWRM[30] bit
   * - TriggerMode:             MDMA_CTCR_TRGM[29:28] bits
   * - PaddingAlignment:       MDMA_CTCR_PAM[27:26] bits
   * - PackMode:                MDMA_CTCR_PKE[25] bit
   * - BufferTransferLength:    MDMA_CTCR_TLEN[24:18] bits
   * - DestBurst:               MDMA_CTCR_DBURST[17:15] bits
   * - SrctBurst:               MDMA_CTCR_SBURST[14:12] bits
   * - DestIncSize:             MDMA_CTCR_DINCOS[11:10] bits
   * - SrcIncSize:              MDMA_CTCR_SINCOS[9:8] bits
   * - DestDataSize:            MDMA_CTCR_DSIZE[7:6] bits
   * - SrcDataSize:             MDMA_CTCR_SSIZE[5:4] bits
   * - DestIncMode:             MDMA_CTCR_DINC[3:2] bits
   * - SrcIncMode:              MDMA_CTCR_SINC[1:0] bits
   */
  LL_MDMA_ConfigTransfer(MDMAx, Channel, MDMA_InitStruct->BufferableWriteMode | \
                                         MDMA_InitStruct->RequestMode         | \
                                         MDMA_InitStruct->TriggerMode         | \
                                         MDMA_InitStruct->PaddingAlignment    | \
                                         MDMA_InitStruct->PackMode            | \
                                         MDMA_InitStruct->DestBurst           | \
                                         MDMA_InitStruct->SrctBurst           | \
                                         MDMA_InitStruct->DestIncSize         | \
                                         MDMA_InitStruct->SrcIncSize          | \
                                         MDMA_InitStruct->DestDataSize        | \
                                         MDMA_InitStruct->SrcDataSize         | \
                                         MDMA_InitStruct->DestIncMode         | \
                                         MDMA_InitStruct->SrcIncMode, MDMA_InitStruct->BufferTransferLength);

  /*-------------------------- MDMAx CBNDTR Configuration --------------------------
   * Configure the Transfer Block counters and update mode with parameter :
   * - BlockRepeatCount:                MDMA_CBNDTR_BRC[31:20] bits
   * - BlockDataLength:                 MDMA_CBNDTR_BNDT[16:0] bits
   * - BlockRepeatDestAddrUpdateMode:   MDMA_CBNDTR_BRDUM[19] bit
   * - BlockRepeatDestAddrUpdateMode:   MDMA_CBNDTR_BRSUM[18] bit
   */
  LL_MDMA_ConfigBlkCounters(MDMAx, Channel, MDMA_InitStruct->BlockRepeatCount, MDMA_InitStruct->BlockDataLength);

  LL_MDMA_ConfigBlkRepeatAddrUpdate(MDMAx, Channel, MDMA_InitStruct->BlockRepeatDestAddrUpdateMode | \
                                                   MDMA_InitStruct->BlockRepeatSrcAddrUpdateMode);



  /*-------------------------- MDMAx CSAR Configuration --------------------------
   * Configure the Transfer source address with parameter :
   * - SrcAddress:     MDMA_CSAR_SAR[31:0] bits
   */
  LL_MDMA_SetSourceAddress(MDMAx, Channel, MDMA_InitStruct->SrcAddress);

  /*-------------------------- MDMAx CDAR Configuration --------------------------
   * Configure the Transfer destination address with parameter :
   * - DstAddress:     MDMA_CDAR_DAR[31:0] bits
   */
  LL_MDMA_SetDestinationAddress(MDMAx, Channel, MDMA_InitStruct->DstAddress);

  /*-------------------------- MDMAx CBRUR Configuration --------------------------
   * Configure the Transfer Block repeat address update value with parameter :
   * - BlockRepeatDestAddrUpdateVal:    MDMA_CBRUR_DUV[31:16] bits
   * - BlockRepeatSrcAddrUpdateVal:     MDMA_CBRUR_SUV[15:0] bits
   */
  LL_MDMA_ConfigBlkRptAddrUpdateValue(MDMAx, Channel, MDMA_InitStruct->BlockRepeatSrcAddrUpdateVal, \
                                               MDMA_InitStruct->BlockRepeatDestAddrUpdateVal);

  /*-------------------------- MDMAx CLAR Configuration --------------------------
   * Configure the Transfer linked list address with parameter :
   * - LinkAddress:     MDMA_CLAR_LAR[31:0] bits
   */
  LL_MDMA_SetLinkAddress(MDMAx, Channel, MDMA_InitStruct->LinkAddress);

  /*-------------------------- MDMAx CTBR Configuration --------------------------
   * Configure the Transfer HW trigger and bus selection with parameter :
   * - DestBus:     MDMA_TBR_DBUS[17] bit
   * - SrcBus:      MDMA_TBR_SBUS[16] bit
   * - HWTrigger:   MDMA_TBR_TSEL[5:0] bits
   */
  LL_MDMA_ConfigBusSelection(MDMAx, Channel, MDMA_InitStruct->DestBus | MDMA_InitStruct->SrcBus);

  LL_MDMA_SetHWTrigger(MDMAx, Channel, MDMA_InitStruct->HWTrigger);

  /*-------------------------- MDMAx CMAR Configuration --------------------------
   * Configure the mask address with parameter :
   * - MaskAddress:     MDMA_CMAR_MAR[31:0] bits
   */
  LL_MDMA_SetMaskAddress(MDMAx, Channel, MDMA_InitStruct->MaskAddress);

  /*-------------------------- MDMAx CMDR Configuration --------------------------
   * Configure the mask data with parameter :
   * - MaskData:     MDMA_CMDR_MDR[31:0] bits
   */
  LL_MDMA_SetMaskData(MDMAx, Channel, MDMA_InitStruct->MaskData);

  return (uint32_t)SUCCESS;
}

/**
  * @brief  Set each @ref LL_MDMA_InitTypeDef field to default value.
  * @param  MDMA_InitStruct Pointer to a @ref LL_MDMA_InitTypeDef structure.
  * @retval None
  */
void LL_MDMA_StructInit(LL_MDMA_InitTypeDef *MDMA_InitStruct)
{
  /* Set DMA_InitStruct fields to default values */
  MDMA_InitStruct->SrcAddress                     = 0x00000000U;
  MDMA_InitStruct->DstAddress                     = 0x00000000U;
  MDMA_InitStruct->BlockDataLength                = 0x00000000U;
  MDMA_InitStruct->BlockRepeatCount               = 0x00000000U;
  MDMA_InitStruct->WordEndianess                  = LL_MDMA_WORD_ENDIANNESS_PRESERVE;
  MDMA_InitStruct->HalfWordEndianess              = LL_MDMA_HALFWORD_ENDIANNESS_PRESERVE;
  MDMA_InitStruct->ByteEndianess                  = LL_MDMA_BYTE_ENDIANNESS_PRESERVE;
  MDMA_InitStruct->Priority                       = LL_MDMA_PRIORITY_LOW;
  MDMA_InitStruct->BufferableWriteMode            = LL_MDMA_BUFF_WRITE_DISABLE;
  MDMA_InitStruct->RequestMode                    = LL_MDMA_REQUEST_MODE_HW;
  MDMA_InitStruct->TriggerMode                    = LL_MDMA_BUFFER_TRANSFER;
  MDMA_InitStruct->PaddingAlignment               = LL_MDMA_DATAALIGN_RIGHT;
  MDMA_InitStruct->PackMode                       = LL_MDMA_PACK_DISABLE;
  MDMA_InitStruct->BufferTransferLength           = 0x00000000U;
  MDMA_InitStruct->DestBurst                      = LL_MDMA_DEST_BURST_SINGLE;
  MDMA_InitStruct->SrctBurst                      = LL_MDMA_SRC_BURST_SINGLE;
  MDMA_InitStruct->DestIncSize                    = LL_MDMA_DEST_INC_OFFSET_BYTE;
  MDMA_InitStruct->SrcIncSize                     = LL_MDMA_SRC_INC_OFFSET_BYTE;
  MDMA_InitStruct->DestDataSize                   = LL_MDMA_DEST_DATA_SIZE_BYTE;
  MDMA_InitStruct->SrcDataSize                    = LL_MDMA_SRC_DATA_SIZE_BYTE;
  MDMA_InitStruct->DestIncMode                    = LL_MDMA_DEST_FIXED;
  MDMA_InitStruct->SrcIncMode                     = LL_MDMA_SRC_FIXED;
  MDMA_InitStruct->BlockRepeatDestAddrUpdateMode  = LL_MDMA_BLK_RPT_DEST_ADDR_INCREMENT;
  MDMA_InitStruct->BlockRepeatSrcAddrUpdateMode   = LL_MDMA_BLK_RPT_SRC_ADDR_INCREMENT;
  MDMA_InitStruct->BlockRepeatDestAddrUpdateVal   = 0x00000000U;
  MDMA_InitStruct->BlockRepeatSrcAddrUpdateVal    = 0x00000000U;
  MDMA_InitStruct->LinkAddress                    = 0x00000000U;
  MDMA_InitStruct->DestBus                        = LL_MDMA_DEST_BUS_SYSTEM_AXI;
  MDMA_InitStruct->SrcBus                         = LL_MDMA_SRC_BUS_SYSTEM_AXI;
  MDMA_InitStruct->HWTrigger                      = LL_MDMA_REQ_DMA1_STREAM0_TC;
  MDMA_InitStruct->MaskAddress                    = 0x00000000U;
  MDMA_InitStruct->MaskData                       = 0x00000000U;
}

/**
  * @brief  Initializes MDMA linked list node according to the specified
  *         parameters in the MDMA_InitStruct.
  * @param  MDMA_InitStruct Pointer to a @ref LL_MDMA_InitTypeDef structure that contains
  *         linked list node registers configurations.
  * @param  pNode Pointer to linked list node to fill according to MDMA_InitStruct parameters.
  * @retval None
  */
void LL_MDMA_CreateLinkNode(LL_MDMA_InitTypeDef *MDMA_InitStruct, LL_MDMA_LinkNodeTypeDef *pNode)
{

  /* Check the MDMA parameters from MDMA_InitStruct */
  assert_param(IS_LL_MDMA_BLK_DATALENGTH(MDMA_InitStruct->BlockDataLength));
  assert_param(IS_LL_MDMA_BLK_REPEATCOUNT(MDMA_InitStruct->BlockRepeatCount));

  assert_param(IS_LL_MDMA_BUFFWRITEMODE(MDMA_InitStruct->BufferableWriteMode));
  assert_param(IS_LL_MDMA_REQUESTMODE(MDMA_InitStruct->RequestMode));
  assert_param(IS_LL_MDMA_TRIGGERMODE(MDMA_InitStruct->TriggerMode));
  assert_param(IS_LL_MDMA_PADDINGALIGNEMENT(MDMA_InitStruct->PaddingAlignment));
  assert_param(IS_LL_MDMA_PACKMODE(MDMA_InitStruct->PackMode));
  assert_param(IS_LL_MDMA_BUFFER_XFERLENGTH(MDMA_InitStruct->BufferTransferLength));
  assert_param(IS_LL_MDMA_DESTBURST(MDMA_InitStruct->DestBurst));
  assert_param(IS_LL_MDMA_SRCTBURST(MDMA_InitStruct->SrctBurst));
  assert_param(IS_LL_MDMA_DESTINCSIZE(MDMA_InitStruct->DestIncSize));
  assert_param(IS_LL_MDMA_SRCINCSIZE(MDMA_InitStruct->SrcIncSize));
  assert_param(IS_LL_MDMA_DESTDATASIZE(MDMA_InitStruct->DestDataSize));
  assert_param(IS_LL_MDMA_SRCDATASIZE(MDMA_InitStruct->SrcDataSize));
  assert_param(IS_LL_MDMA_DESTINCMODE(MDMA_InitStruct->DestIncMode));
  assert_param(IS_LL_MDMA_SRCINCMODE(MDMA_InitStruct->SrcIncMode));
  assert_param(IS_LL_MDMA_BLKRPT_DEST_ADDRUPDATEMODE(MDMA_InitStruct->BlockRepeatDestAddrUpdateMode));
  assert_param(IS_LL_MDMA_BLKRPT_SRC_ADDRUPDATEMODE(MDMA_InitStruct->BlockRepeatSrcAddrUpdateMode));
  assert_param(IS_LL_MDMA_BLKRPT_DEST_ADDRUPDATEVAL(MDMA_InitStruct->BlockRepeatDestAddrUpdateVal));
  assert_param(IS_LL_MDMA_BLKRPT_SRC_ADDRUPDATEVAL(MDMA_InitStruct->BlockRepeatSrcAddrUpdateVal));
  assert_param(IS_LL_MDMA_DEST_BUS(MDMA_InitStruct->DestBus));
  assert_param(IS_LL_MDMA_SRC_BUS(MDMA_InitStruct->SrcBus));
  assert_param(IS_LL_MDMA_HWTRIGGER(MDMA_InitStruct->HWTrigger));


  /*-------------------------- MDMAx CTCR Configuration --------------------------
   * Configure the Transfer  parameter :
   * - BufferableWriteMode:     MDMA_CTCR_BWM[31] bit
   * - RequestMode:             MDMA_CTCR_SWRM[30] bit
   * - TriggerMode:             MDMA_CTCR_TRGM[29:28] bits
   * - PaddingAlignment:       MDMA_CTCR_PAM[27:26] bits
   * - PackMode:                MDMA_CTCR_PKE[25] bit
   * - BufferTransferLength:    MDMA_CTCR_TLEN[24:18] bits
   * - DestBurst:               MDMA_CTCR_DBURST[17:15] bits
   * - SrctBurst:               MDMA_CTCR_SBURST[14:12] bits
   * - DestIncSize:             MDMA_CTCR_DINCOS[11:10] bits
   * - SrcIncSize:              MDMA_CTCR_SINCOS[9:8] bits
   * - DestDataSize:            MDMA_CTCR_DSIZE[7:6] bits
   * - SrcDataSize:             MDMA_CTCR_SSIZE[5:4] bits
   * - DestIncMode:             MDMA_CTCR_DINC[3:2] bits
   * - SrcIncMode:              MDMA_CTCR_SINC[1:0] bits
   */
  pNode->CTCR =  MDMA_InitStruct->BufferableWriteMode | \
                 MDMA_InitStruct->RequestMode         | \
                 MDMA_InitStruct->TriggerMode         | \
                 MDMA_InitStruct->PaddingAlignment    | \
                 MDMA_InitStruct->PackMode            | \
                 MDMA_InitStruct->DestBurst           | \
                 MDMA_InitStruct->SrctBurst           | \
                 MDMA_InitStruct->DestIncSize         | \
                 MDMA_InitStruct->SrcIncSize          | \
                 MDMA_InitStruct->DestDataSize        | \
                 MDMA_InitStruct->SrcDataSize         | \
                 MDMA_InitStruct->DestIncMode         | \
                 MDMA_InitStruct->SrcIncMode          | \
                ((MDMA_InitStruct->BufferTransferLength << MDMA_CTCR_TLEN_Pos) & MDMA_CTCR_TLEN_Msk);



  /*-------------------------- MDMAx CBNDTR Configuration --------------------------
   * Configure the Transfer Block counters and update mode with parameter :
   * - BlockRepeatCount:                MDMA_CBNDTR_BRC[31:20] bits
   * - BlockDataLength:                 MDMA_CBNDTR_BNDT[16:0] bits
   * - BlockRepeatDestAddrUpdateMode:   MDMA_CBNDTR_BRDUM[19] bit
   * - BlockRepeatDestAddrUpdateMode:   MDMA_CBNDTR_BRSUM[18] bit
   */
  pNode->CBNDTR =  ((MDMA_InitStruct->BlockRepeatCount << MDMA_CBNDTR_BRC_Pos) & MDMA_CBNDTR_BRC_Msk) | \
                    MDMA_InitStruct->BlockRepeatDestAddrUpdateMode                                    | \
                    MDMA_InitStruct->BlockRepeatSrcAddrUpdateMode                                     | \
                   (MDMA_InitStruct->BlockDataLength & MDMA_CBNDTR_BNDT_Msk);


  /*-------------------------- MDMAx CSAR Configuration --------------------------
   * Configure the Transfer source address with parameter :
   * - SrcAddress:     MDMA_CSAR_SAR[31:0] bits
   */
  pNode->CSAR =  MDMA_InitStruct->SrcAddress;


  /*-------------------------- MDMAx CDAR Configuration --------------------------
   * Configure the Transfer destination address with parameter :
   * - DstAddress:     MDMA_CDAR_DAR[31:0] bits
   */
  pNode->CDAR =  MDMA_InitStruct->DstAddress;

  /*-------------------------- MDMAx CBRUR Configuration --------------------------
   * Configure the Transfer Block repeat address update value with parameter :
   * - BlockRepeatDestAddrUpdateVal:    MDMA_CBRUR_DUV[31:16] bits
   * - BlockRepeatSrcAddrUpdateVal:     MDMA_CBRUR_SUV[15:0] bits
   */
  pNode->CBRUR =  (MDMA_InitStruct->BlockRepeatSrcAddrUpdateVal & MDMA_CBRUR_SUV_Msk)  | \
                  ((MDMA_InitStruct->BlockRepeatDestAddrUpdateVal << MDMA_CBRUR_DUV_Pos) & MDMA_CBRUR_DUV_Msk) ;

  /*-------------------------- MDMAx CLAR Configuration --------------------------
   * Configure the Transfer linked list address with parameter :
   * - LinkAddress:     MDMA_CLAR_LAR[31:0] bits
   */
  pNode->CLAR = MDMA_InitStruct->LinkAddress;

  /*-------------------------- MDMAx CTBR Configuration --------------------------
   * Configure the Transfer HW trigger and bus selection with parameter :
   * - DestBus:     MDMA_TBR_DBUS[17] bit
   * - SrcBus:      MDMA_TBR_SBUS[16] bit
   * - HWTrigger:   MDMA_TBR_TSEL[5:0] bits
   */
  pNode->CTBR = MDMA_InitStruct->DestBus | MDMA_InitStruct->SrcBus | MDMA_InitStruct->HWTrigger;

  /*-------------------------- MDMAx CMAR Configuration --------------------------
   * Configure the mask address with parameter :
   * - MaskAddress:     MDMA_CMAR_MAR[31:0] bits
   */
  pNode->CMAR = MDMA_InitStruct->MaskAddress;

  /*-------------------------- MDMAx CMDR Configuration --------------------------
   * Configure the mask data with parameter :
   * - MaskData:     MDMA_CMDR_MDR[31:0] bits
   */
  pNode->CMDR = MDMA_InitStruct->MaskData;


  pNode->Reserved = 0;

}

/**
  * @brief  Connect Linked list Nodes.
  * @param  pPrevLinkNode Pointer to previous linked list node to be connected to new Lined list node.
  * @param  pNewLinkNode Pointer to new Linked list.
  * @retval None
  */
void LL_MDMA_ConnectLinkNode(LL_MDMA_LinkNodeTypeDef *pPrevLinkNode, LL_MDMA_LinkNodeTypeDef *pNewLinkNode)
{
  pPrevLinkNode->CLAR = (uint32_t)pNewLinkNode;
}

/**
  * @brief  Disconnect the next linked list node.
  * @param  pLinkNode Pointer to linked list node to be disconnected from the next one.
  * @retval None
  */
void LL_MDMA_DisconnectNextLinkNode(LL_MDMA_LinkNodeTypeDef *pLinkNode)
{
  pLinkNode->CLAR = 0;
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

#endif /* MDMA */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
