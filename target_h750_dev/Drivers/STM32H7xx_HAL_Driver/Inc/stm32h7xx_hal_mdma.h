/**
  ******************************************************************************
  * @file    stm32h7xx_hal_mdma.h
  * @author  MCD Application Team
  * @brief   Header file of DMA HAL module.
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
#ifndef STM32H7xx_HAL_MDMA_H
#define STM32H7xx_HAL_MDMA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup MDMA
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup MDMA_Exported_Types MDMA Exported Types
  * @brief    MDMA Exported Types
  * @{
  */

/**
  * @brief  MDMA Configuration Structure definition
  */
typedef struct
{

  uint32_t Request;                 /*!< Specifies the MDMA request.
                                        This parameter can be a value of @ref MDMA_Request_selection*/

  uint32_t TransferTriggerMode;     /*!< Specifies the Trigger Transfer mode : each request triggers a :
                                         a buffer transfer, a block transfer, a repeated block transfer or a linked list transfer
                                         This parameter can be a value of @ref MDMA_Transfer_TriggerMode  */

  uint32_t Priority;                 /*!< Specifies the software priority for the MDMAy channelx.
                                         This parameter can be a value of @ref MDMA_Priority_level */

  uint32_t Endianness;                /*!< Specifies if the MDMA transactions preserve the Little endianness.
                                         This parameter can be a value of @ref MDMA_Endianness */

  uint32_t SourceInc;                /*!< Specifies if the Source increment mode .
                                         This parameter can be a value of @ref MDMA_Source_increment_mode */

  uint32_t DestinationInc;           /*!< Specifies if the Destination increment mode .
                                         This parameter can be a value of @ref MDMA_Destination_increment_mode */

  uint32_t SourceDataSize;           /*!< Specifies the source data size.
                                         This parameter can be a value of @ref MDMA_Source_data_size */

  uint32_t DestDataSize;             /*!< Specifies the destination data size.
                                          This parameter can be a value of @ref MDMA_Destination_data_size */


  uint32_t DataAlignment;            /*!< Specifies the source to destination Memory data packing/padding mode.
                                            This parameter can be a value of @ref MDMA_data_Alignment */

  uint32_t BufferTransferLength;      /*!< Specifies the buffer Transfer Length (number of bytes),
                                          this is the number of bytes to be transferred in a single transfer (1 byte to 128 bytes)*/

  uint32_t SourceBurst;              /*!< Specifies the Burst transfer configuration for the source memory transfers.
                                         It specifies the amount of data to be transferred in a single non interruptable
                                         transaction.
                                         This parameter can be a value of @ref MDMA_Source_burst
                                         @note : the burst may be FIXED/INCR based on SourceInc value ,
                                         the BURST must be programmed as to ensure that the burst size will be lower than than
                                         BufferTransferLength */

  uint32_t DestBurst;                 /*!< Specifies the Burst transfer configuration for the destination memory transfers.
                                           It specifies the amount of data to be transferred in a single non interruptable
                                           transaction.
                                           This parameter can be a value of @ref MDMA_Destination_burst
                                           @note : the burst may be FIXED/INCR based on DestinationInc value ,
                                           the BURST must be programmed as to ensure that the burst size will be lower than than
                                           BufferTransferLength */

  int32_t SourceBlockAddressOffset;   /*!< this field specifies the Next block source address offset
                                           signed value : if > 0 then  increment the next block source Address by offset from where the last block ends
                                                          if < 0 then  decrement the next block source Address by offset from where the last block ends
                                                          if == 0, the next block source address starts from where the last block ends
                                       */


  int32_t DestBlockAddressOffset;      /*!< this field specifies the Next block destination address offset
                                           signed value : if > 0 then  increment the next block destination Address by offset from where the last block ends
                                                          if < 0 then  decrement the next block destination Address by offset from where the last block ends
                                                          if == 0, the next block destination address starts from where the last block ends
                                       */

}MDMA_InitTypeDef;

/**
  * @brief  HAL MDMA linked list node structure definition
  * @note   The Linked list node allows to define a new MDMA configuration
  *         (CTCR ,CBNDTR ,CSAR ,CDAR ,CBRUR, CLAR, CTBR, CMAR and CMDR registers).
  *         When CLAR register is configured to a non NULL value , each time a transfer ends,
  *         a new configuration (linked list node) is automatically loaded from the address given in CLAR register.
  */
typedef struct
{
  __IO uint32_t CTCR;     /*!< New CTCR register configuration for the given MDMA linked list node   */
  __IO uint32_t CBNDTR;   /*!< New CBNDTR register configuration for the given MDMA linked list node */
  __IO uint32_t CSAR;     /*!< New CSAR register configuration for the given MDMA linked list node   */
  __IO uint32_t CDAR;     /*!< New CDAR register configuration for the given MDMA linked list node   */
  __IO uint32_t CBRUR;    /*!< New CBRUR register configuration for the given MDMA linked list node  */
  __IO uint32_t CLAR;     /*!< New CLAR register configuration for the given MDMA linked list node   */
  __IO uint32_t CTBR;     /*!< New CTBR register configuration for the given MDMA linked list node   */
  __IO uint32_t Reserved; /*!< Reserved register                                                     */
  __IO uint32_t CMAR;     /*!< New CMAR register configuration for the given MDMA linked list node   */
  __IO uint32_t CMDR;     /*!< New CMDR register configuration for the given MDMA linked list node   */

}MDMA_LinkNodeTypeDef;

/**
  * @brief  HAL MDMA linked list node configuration structure definition
  * @note   used with HAL_MDMA_LinkedList_CreateNode function
  */
typedef struct
{
  MDMA_InitTypeDef Init;            /*!< configuration of the specified MDMA Linked List Node    */
  uint32_t         SrcAddress;      /*!< The source memory address for the Linked list Node      */
  uint32_t         DstAddress;      /*!< The destination memory address for the Linked list Node */
  uint32_t         BlockDataLength; /*!< The data length of a block in bytes                     */
  uint32_t         BlockCount;      /*!< The number of blocks to be transferred                  */

  uint32_t PostRequestMaskAddress;  /*!< specifies the address to be updated (written) with PostRequestMaskData after a request is served.
                                         PostRequestMaskAddress and PostRequestMaskData could be used to automatically clear a peripheral flag when the request is served  */

  uint32_t PostRequestMaskData;     /*!< specifies the value to be written to PostRequestMaskAddress after a request is served.
                                         PostRequestMaskAddress and PostRequestMaskData could be used to automatically clear a peripheral flag when the request is served  */


}MDMA_LinkNodeConfTypeDef;


/**
  * @brief  HAL MDMA State structure definition
  */
typedef enum
{
  HAL_MDMA_STATE_RESET               = 0x00U,  /*!< MDMA not yet initialized or disabled */
  HAL_MDMA_STATE_READY               = 0x01U,  /*!< MDMA initialized and ready for use   */
  HAL_MDMA_STATE_BUSY                = 0x02U,  /*!< MDMA process is ongoing              */
  HAL_MDMA_STATE_ERROR               = 0x03U,  /*!< MDMA error state                     */
  HAL_MDMA_STATE_ABORT               = 0x04U,  /*!< MDMA Abort state                     */

}HAL_MDMA_StateTypeDef;

/**
  * @brief  HAL MDMA Level Complete structure definition
  */
typedef enum
{
  HAL_MDMA_FULL_TRANSFER         = 0x00U,   /*!< Full transfer         */
  HAL_MDMA_BUFFER_TRANSFER       = 0x01U,   /*!< Buffer Transfer       */
  HAL_MDMA_BLOCK_TRANSFER        = 0x02U,   /*!< Block Transfer        */
  HAL_MDMA_REPEAT_BLOCK_TRANSFER = 0x03U    /*!< repeat block Transfer */

}HAL_MDMA_LevelCompleteTypeDef;

/**
  * @brief  HAL MDMA Callbacks IDs structure definition
  */
typedef enum
{
  HAL_MDMA_XFER_CPLT_CB_ID          = 0x00U,    /*!< Full transfer           */
  HAL_MDMA_XFER_BUFFERCPLT_CB_ID    = 0x01U,    /*!< Buffer Transfer         */
  HAL_MDMA_XFER_BLOCKCPLT_CB_ID     = 0x02U,    /*!< Block Transfer          */
  HAL_MDMA_XFER_REPBLOCKCPLT_CB_ID  = 0x03U,    /*!< Repeated Block Transfer */
  HAL_MDMA_XFER_ERROR_CB_ID         = 0x04U,    /*!< Error                   */
  HAL_MDMA_XFER_ABORT_CB_ID         = 0x05U,    /*!< Abort                   */
  HAL_MDMA_XFER_ALL_CB_ID           = 0x06U     /*!< All                     */

}HAL_MDMA_CallbackIDTypeDef;


/**
  * @brief  MDMA handle Structure definition
  */
typedef struct __MDMA_HandleTypeDef
{
  MDMA_Channel_TypeDef *Instance;                                                              /*!< Register base address                  */

  MDMA_InitTypeDef      Init;                                                                  /*!< MDMA communication parameters          */

  HAL_LockTypeDef       Lock;                                                                  /*!< MDMA locking object                    */

  __IO HAL_MDMA_StateTypeDef  State;                                                           /*!< MDMA transfer state                    */

  void                  *Parent;                                                               /*!< Parent object state                    */

  void                  (* XferCpltCallback)( struct __MDMA_HandleTypeDef * hmdma);            /*!< MDMA transfer complete callback        */

  void                  (* XferBufferCpltCallback)( struct __MDMA_HandleTypeDef * hmdma);      /*!< MDMA buffer transfer complete callback */

  void                  (* XferBlockCpltCallback)( struct __MDMA_HandleTypeDef * hmdma);       /*!< MDMA block transfer complete callback  */

  void                  (* XferRepeatBlockCpltCallback)( struct __MDMA_HandleTypeDef * hmdma); /*!< MDMA block transfer repeat callback    */

  void                  (* XferErrorCallback)( struct __MDMA_HandleTypeDef * hmdma);           /*!< MDMA transfer error callback           */

  void                  (* XferAbortCallback)( struct __MDMA_HandleTypeDef * hmdma);           /*!< MDMA transfer Abort callback           */


  MDMA_LinkNodeTypeDef *FirstLinkedListNodeAddress;                                             /*!< specifies the first node address of the transfer list
                                                                                                     (after the initial node defined by the Init struct)
                                                                                                     this parameter is used internally by the MDMA driver
                                                                                                     to construct the linked list node
                                                                                                */

  MDMA_LinkNodeTypeDef *LastLinkedListNodeAddress;                                             /*!< specifies the last node address of the transfer list
                                                                                                    this parameter is used internally by the MDMA driver
                                                                                                    to construct the linked list node
                                                                                                */
  uint32_t LinkedListNodeCounter;                                                               /*!< Number of nodes in the MDMA linked list */

  __IO uint32_t          ErrorCode;                                                            /*!< MDMA Error code                        */

} MDMA_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup MDMA_Exported_Constants MDMA Exported Constants
  * @brief    MDMA Exported constants
  * @{
  */

/** @defgroup MDMA_Error_Codes MDMA Error Codes
  * @brief    MDMA Error Codes
  * @{
  */
#define HAL_MDMA_ERROR_NONE        ((uint32_t)0x00000000U)   /*!< No error                               */
#define HAL_MDMA_ERROR_READ_XFER   ((uint32_t)0x00000001U)   /*!< Read Transfer error                    */
#define HAL_MDMA_ERROR_WRITE_XFER  ((uint32_t)0x00000002U)   /*!< Write Transfer error                   */
#define HAL_MDMA_ERROR_MASK_DATA   ((uint32_t)0x00000004U)   /*!< Error Mask Data error                  */
#define HAL_MDMA_ERROR_LINKED_LIST ((uint32_t)0x00000008U)   /*!< Linked list Data error                 */
#define HAL_MDMA_ERROR_ALIGNMENT   ((uint32_t)0x00000010U)   /*!< Address/Size alignment  error          */
#define HAL_MDMA_ERROR_BLOCK_SIZE  ((uint32_t)0x00000020U)   /*!< Block Size error                       */
#define HAL_MDMA_ERROR_TIMEOUT     ((uint32_t)0x00000040U)   /*!< Timeout error                          */
#define HAL_MDMA_ERROR_NO_XFER     ((uint32_t)0x00000080U)   /*!< Abort or SW trigger requested with no Xfer ongoing   */
#define HAL_MDMA_ERROR_BUSY        ((uint32_t)0x00000100U)   /*!< DeInit or SW trigger requested with Xfer ongoing   */

/**
  * @}
  */

/** @defgroup MDMA_Request_selection MDMA Request selection
  * @brief    MDMA_Request_selection
  * @{
  */

#define MDMA_REQUEST_DMA1_Stream0_TC      ((uint32_t)0x00000000U)  /*!< MDMA HW request is DMA1 Stream 0 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream1_TC      ((uint32_t)0x00000001U)  /*!< MDMA HW request is DMA1 Stream 1 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream2_TC      ((uint32_t)0x00000002U)  /*!< MDMA HW request is DMA1 Stream 2 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream3_TC      ((uint32_t)0x00000003U)  /*!< MDMA HW request is DMA1 Stream 3 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream4_TC      ((uint32_t)0x00000004U)  /*!< MDMA HW request is DMA1 Stream 4 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream5_TC      ((uint32_t)0x00000005U)  /*!< MDMA HW request is DMA1 Stream 5 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream6_TC      ((uint32_t)0x00000006U)  /*!< MDMA HW request is DMA1 Stream 6 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA1_Stream7_TC      ((uint32_t)0x00000007U)  /*!< MDMA HW request is DMA1 Stream 7 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream0_TC      ((uint32_t)0x00000008U)  /*!< MDMA HW request is DMA2 Stream 0 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream1_TC      ((uint32_t)0x00000009U)  /*!< MDMA HW request is DMA2 Stream 1 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream2_TC      ((uint32_t)0x0000000AU)  /*!< MDMA HW request is DMA2 Stream 2 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream3_TC      ((uint32_t)0x0000000BU)  /*!< MDMA HW request is DMA2 Stream 3 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream4_TC      ((uint32_t)0x0000000CU)  /*!< MDMA HW request is DMA2 Stream 4 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream5_TC      ((uint32_t)0x0000000DU)  /*!< MDMA HW request is DMA2 Stream 5 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream6_TC      ((uint32_t)0x0000000EU)  /*!< MDMA HW request is DMA2 Stream 6 Transfer Complete Flag   */
#define MDMA_REQUEST_DMA2_Stream7_TC      ((uint32_t)0x0000000FU)  /*!< MDMA HW request is DMA2 Stream 7 Transfer Complete Flag   */
#if defined (LTDC)
#define MDMA_REQUEST_LTDC_LINE_IT         ((uint32_t)0x00000010U)  /*!< MDMA HW request is LTDC Line interrupt Flag               */
#endif /* LTDC */
#if defined (JPEG)
#define MDMA_REQUEST_JPEG_INFIFO_TH       ((uint32_t)0x00000011U)  /*!< MDMA HW request is JPEG Input FIFO threshold Flag         */
#define MDMA_REQUEST_JPEG_INFIFO_NF       ((uint32_t)0x00000012U)  /*!< MDMA HW request is JPEG Input FIFO not full Flag          */
#define MDMA_REQUEST_JPEG_OUTFIFO_TH      ((uint32_t)0x00000013U)  /*!< MDMA HW request is JPEG Output FIFO threshold Flag        */
#define MDMA_REQUEST_JPEG_OUTFIFO_NE      ((uint32_t)0x00000014U)  /*!< MDMA HW request is JPEG Output FIFO not empty Flag        */
#define MDMA_REQUEST_JPEG_END_CONVERSION  ((uint32_t)0x00000015U)  /*!< MDMA HW request is JPEG End of conversion Flag            */
#endif /* JPEG */
#if defined (OCTOSPI1)
#define MDMA_REQUEST_OCTOSPI1_FIFO_TH     ((uint32_t)0x00000016U)  /*!< MDMA HW request is OCTOSPI1 FIFO threshold Flag           */
#define MDMA_REQUEST_OCTOSPI1_TC          ((uint32_t)0x00000017U)  /*!< MDMA HW request is OCTOSPI1 Transfer complete Flag        */
#endif /* OCTOSPI1 */
#if defined (QUADSPI)
#define MDMA_REQUEST_QUADSPI_FIFO_TH      ((uint32_t)0x00000016U)  /*!< MDMA HW request is QSPI FIFO threshold Flag               */
#define MDMA_REQUEST_QUADSPI_TC           ((uint32_t)0x00000017U)  /*!< MDMA HW request is QSPI Transfer complete Flag            */
#endif /* QUADSPI */
#define MDMA_REQUEST_DMA2D_CLUT_TC        ((uint32_t)0x00000018U)  /*!< MDMA HW request is DMA2D CLUT Transfer Complete Flag      */
#define MDMA_REQUEST_DMA2D_TC             ((uint32_t)0x00000019U)  /*!< MDMA HW request is DMA2D Transfer Complete Flag           */
#define MDMA_REQUEST_DMA2D_TW             ((uint32_t)0x0000001AU)  /*!< MDMA HW request is DMA2D Transfer Watermark Flag          */

#if defined (DSI)
#define MDMA_REQUEST_DSI_TEARING_EFFECT   ((uint32_t)0x0000001BU)  /*!< MDMA HW request is DSI Tearing Effect Flag                */
#define MDMA_REQUEST_DSI_END_REFRESH      ((uint32_t)0x0000001CU)  /*!< MDMA HW request is DSI End of refresh  Flag               */
#endif /* DSI */

#define MDMA_REQUEST_SDMMC1_END_DATA      ((uint32_t)0x0000001DU)  /*!< MDMA HW request is SDMMC1 End of Data Flag                */

#define MDMA_REQUEST_SDMMC1_DMA_ENDBUFFER ((uint32_t)0x0000001EU)  /*!< MDMA HW request is SDMMC1 Internal DMA buffer End Flag    */
#define MDMA_REQUEST_SDMMC1_COMMAND_END   ((uint32_t)0x0000001FU)  /*!< MDMA HW request is SDMMC1 Command End Flag                */

#if defined (OCTOSPI2)
#define MDMA_REQUEST_OCTOSPI2_FIFO_TH     ((uint32_t)0x00000020U)  /*!< MDMA HW request is OCTOSPI2 FIFO threshold Flag           */
#define MDMA_REQUEST_OCTOSPI2_TC          ((uint32_t)0x00000021U)  /*!< MDMA HW request is OCTOSPI2 Transfer complete Flag        */
#endif /* OCTOSPI2 */

#define MDMA_REQUEST_SW                   ((uint32_t)0x40000000U) /*!< MDMA SW request                                            */

/**
  * @}
  */

/** @defgroup MDMA_Transfer_TriggerMode MDMA Transfer Trigger  Mode
  * @brief    MDMA Transfer Trigger Mode
  * @{
  */
#define MDMA_BUFFER_TRANSFER          ((uint32_t)0x00000000U)        /*!< Each MDMA request (SW or HW) triggers a buffer transfer                                */
#define MDMA_BLOCK_TRANSFER           ((uint32_t)MDMA_CTCR_TRGM_0)   /*!< Each MDMA request (SW or HW) triggers a block transfer                                 */
#define MDMA_REPEAT_BLOCK_TRANSFER    ((uint32_t)MDMA_CTCR_TRGM_1)   /*!< Each MDMA request (SW or HW) triggers a repeated block transfer                        */
#define MDMA_FULL_TRANSFER            ((uint32_t)MDMA_CTCR_TRGM)     /*!< Each MDMA request (SW or HW) triggers a Full transfer or a linked list transfer if any */

/**
  * @}
  */

/** @defgroup MDMA_Priority_level MDMA Priority level
  * @brief    MDMA Priority level
  * @{
  */
#define MDMA_PRIORITY_LOW             ((uint32_t)0x00000000U)     /*!< Priority level: Low      */
#define MDMA_PRIORITY_MEDIUM          ((uint32_t)MDMA_CCR_PL_0)  /*!< Priority level: Medium    */
#define MDMA_PRIORITY_HIGH            ((uint32_t)MDMA_CCR_PL_1)  /*!< Priority level: High      */
#define MDMA_PRIORITY_VERY_HIGH       ((uint32_t)MDMA_CCR_PL)    /*!< Priority level: Very High */

/**
  * @}
  */


/** @defgroup MDMA_Endianness MDMA Endianness
  * @brief    MDMA Endianness
  * @{
  */
#define MDMA_LITTLE_ENDIANNESS_PRESERVE          ((uint32_t)0x00000000U)   /*!< little endianness preserve                                               */
#define MDMA_LITTLE_BYTE_ENDIANNESS_EXCHANGE     ((uint32_t)MDMA_CCR_BEX)  /*!< BYTEs endianness exchange when destination data size is > Byte           */
#define MDMA_LITTLE_HALFWORD_ENDIANNESS_EXCHANGE ((uint32_t)MDMA_CCR_HEX)  /*!< HALF WORDs endianness exchange when destination data size is > HALF WORD */
#define MDMA_LITTLE_WORD_ENDIANNESS_EXCHANGE     ((uint32_t)MDMA_CCR_WEX)  /*!< WORDs endianness exchange  when destination data size is > DOUBLE WORD   */

/**
  * @}
  */

/** @defgroup MDMA_Source_increment_mode MDMA Source increment mode
  * @brief    MDMA Source increment mode
  * @{
  */
#define MDMA_SRC_INC_DISABLE      ((uint32_t)0x00000000U)                                     /*!< Source address pointer is fixed                                   */
#define MDMA_SRC_INC_BYTE         ((uint32_t)MDMA_CTCR_SINC_1)                                /*!< Source address pointer is incremented by a BYTE (8 bits)          */
#define MDMA_SRC_INC_HALFWORD     ((uint32_t)MDMA_CTCR_SINC_1 | (uint32_t)MDMA_CTCR_SINCOS_0) /*!< Source address pointer is incremented by a half Word (16 bits)    */
#define MDMA_SRC_INC_WORD         ((uint32_t)MDMA_CTCR_SINC_1 | (uint32_t)MDMA_CTCR_SINCOS_1) /*!< Source address pointer is incremented by a Word (32 bits)         */
#define MDMA_SRC_INC_DOUBLEWORD   ((uint32_t)MDMA_CTCR_SINC_1 | (uint32_t)MDMA_CTCR_SINCOS)   /*!< Source address pointer is incremented by a double Word (64 bits)) */
#define MDMA_SRC_DEC_BYTE         ((uint32_t)MDMA_CTCR_SINC)                                  /*!< Source address pointer is decremented by a BYTE (8 bits)          */
#define MDMA_SRC_DEC_HALFWORD     ((uint32_t)MDMA_CTCR_SINC | (uint32_t)MDMA_CTCR_SINCOS_0)   /*!< Source address pointer is decremented by a half Word (16 bits)    */
#define MDMA_SRC_DEC_WORD         ((uint32_t)MDMA_CTCR_SINC | (uint32_t)MDMA_CTCR_SINCOS_1)   /*!< Source address pointer is decremented by a Word (32 bits)         */
#define MDMA_SRC_DEC_DOUBLEWORD   ((uint32_t)MDMA_CTCR_SINC | (uint32_t)MDMA_CTCR_SINCOS)     /*!< Source address pointer is decremented by a double Word (64 bits)) */

/**
  * @}
  */

/** @defgroup MDMA_Destination_increment_mode MDMA Destination increment mode
  * @brief    MDMA Destination increment mode
  * @{
  */
#define MDMA_DEST_INC_DISABLE      ((uint32_t)0x00000000U)                                     /*!< Source address pointer is fixed                                   */
#define MDMA_DEST_INC_BYTE         ((uint32_t)MDMA_CTCR_DINC_1)                                /*!< Source address pointer is incremented by a BYTE (8 bits)          */
#define MDMA_DEST_INC_HALFWORD     ((uint32_t)MDMA_CTCR_DINC_1 | (uint32_t)MDMA_CTCR_DINCOS_0) /*!< Source address pointer is incremented by a half Word (16 bits)    */
#define MDMA_DEST_INC_WORD         ((uint32_t)MDMA_CTCR_DINC_1 | (uint32_t)MDMA_CTCR_DINCOS_1) /*!< Source address pointer is incremented by a Word (32 bits)         */
#define MDMA_DEST_INC_DOUBLEWORD   ((uint32_t)MDMA_CTCR_DINC_1 | (uint32_t)MDMA_CTCR_DINCOS)   /*!< Source address pointer is incremented by a double Word (64 bits)) */
#define MDMA_DEST_DEC_BYTE         ((uint32_t)MDMA_CTCR_DINC)                                  /*!< Source address pointer is decremented by a BYTE (8 bits)          */
#define MDMA_DEST_DEC_HALFWORD     ((uint32_t)MDMA_CTCR_DINC | (uint32_t)MDMA_CTCR_DINCOS_0)   /*!< Source address pointer is decremented by a half Word (16 bits)    */
#define MDMA_DEST_DEC_WORD         ((uint32_t)MDMA_CTCR_DINC | (uint32_t)MDMA_CTCR_DINCOS_1)   /*!< Source address pointer is decremented by a Word (32 bits)         */
#define MDMA_DEST_DEC_DOUBLEWORD   ((uint32_t)MDMA_CTCR_DINC | (uint32_t)MDMA_CTCR_DINCOS)     /*!< Source address pointer is decremented by a double Word (64 bits)) */

/**
  * @}
  */

/** @defgroup MDMA_Source_data_size MDMA Source data size
  * @brief    MDMA Source data size
  * @{
  */
#define MDMA_SRC_DATASIZE_BYTE        ((uint32_t)0x00000000U)         /*!< Source data size is Byte        */
#define MDMA_SRC_DATASIZE_HALFWORD    ((uint32_t)MDMA_CTCR_SSIZE_0)   /*!< Source data size is half word   */
#define MDMA_SRC_DATASIZE_WORD        ((uint32_t)MDMA_CTCR_SSIZE_1)   /*!< Source data size is word        */
#define MDMA_SRC_DATASIZE_DOUBLEWORD  ((uint32_t)MDMA_CTCR_SSIZE)     /*!< Source data size is double word */

/**
  * @}
  */

/** @defgroup MDMA_Destination_data_size MDMA Destination data size
  * @brief    MDMA Destination data size
  * @{
  */
#define MDMA_DEST_DATASIZE_BYTE        ((uint32_t)0x00000000U)         /*!< Destination data size is Byte        */
#define MDMA_DEST_DATASIZE_HALFWORD    ((uint32_t)MDMA_CTCR_DSIZE_0)   /*!< Destination data size is half word   */
#define MDMA_DEST_DATASIZE_WORD        ((uint32_t)MDMA_CTCR_DSIZE_1)   /*!< Destination data size is word        */
#define MDMA_DEST_DATASIZE_DOUBLEWORD  ((uint32_t)MDMA_CTCR_DSIZE)     /*!< Destination data size is double word */

/**
  * @}
  */

/** @defgroup MDMA_data_Alignment MDMA data alignment
  * @brief    MDMA data alignment
  * @{
  */
#define MDMA_DATAALIGN_PACKENABLE        ((uint32_t)MDMA_CTCR_PKE)     /*!< The source data is packed/un-packed into the destination data size
                                                                            All data are right aligned, in Little Endien mode.                                              */
#define MDMA_DATAALIGN_RIGHT            ((uint32_t)0x00000000U)        /*!< Right Aligned, padded w/ 0s (default)                                                           */
#define MDMA_DATAALIGN_RIGHT_SIGNED     ((uint32_t)MDMA_CTCR_PAM_0)    /*!< Right Aligned, Sign extended ,
                                                                            Note : this mode is allowed only if the Source data size is smaller than Destination data size  */
#define MDMA_DATAALIGN_LEFT             ((uint32_t)MDMA_CTCR_PAM_1)    /*!< Left Aligned (padded with 0s)                                                                   */

/**
  * @}
  */

/** @defgroup MDMA_Source_burst MDMA Source burst
  * @brief    MDMA Source burst
  * @{
  */
#define MDMA_SOURCE_BURST_SINGLE        ((uint32_t)0x00000000U)                                       /*!< single transfer */
#define MDMA_SOURCE_BURST_2BEATS        ((uint32_t)MDMA_CTCR_SBURST_0)                                /*!< Burst 2 beats   */
#define MDMA_SOURCE_BURST_4BEATS        ((uint32_t)MDMA_CTCR_SBURST_1)                                /*!< Burst 4 beats   */
#define MDMA_SOURCE_BURST_8BEATS        ((uint32_t)MDMA_CTCR_SBURST_0 | (uint32_t)MDMA_CTCR_SBURST_1) /*!< Burst 8 beats   */
#define MDMA_SOURCE_BURST_16BEATS       ((uint32_t)MDMA_CTCR_SBURST_2)                                /*!< Burst 16 beats  */
#define MDMA_SOURCE_BURST_32BEATS       ((uint32_t)MDMA_CTCR_SBURST_0 | (uint32_t)MDMA_CTCR_SBURST_2) /*!< Burst 32 beats  */
#define MDMA_SOURCE_BURST_64BEATS       ((uint32_t)MDMA_CTCR_SBURST_1 | (uint32_t)MDMA_CTCR_SBURST_2) /*!< Burst 64 beats  */
#define MDMA_SOURCE_BURST_128BEATS      ((uint32_t)MDMA_CTCR_SBURST)                                  /*!< Burst 128 beats */

/**
  * @}
  */

/** @defgroup MDMA_Destination_burst MDMA Destination burst
  * @brief    MDMA Destination burst
  * @{
  */
#define MDMA_DEST_BURST_SINGLE        ((uint32_t)0x00000000U)                                        /*!< single transfer */
#define MDMA_DEST_BURST_2BEATS        ((uint32_t)MDMA_CTCR_DBURST_0)                                 /*!< Burst 2 beats   */
#define MDMA_DEST_BURST_4BEATS        ((uint32_t)MDMA_CTCR_DBURST_1)                                 /*!< Burst 4 beats   */
#define MDMA_DEST_BURST_8BEATS        ((uint32_t)MDMA_CTCR_DBURST_0 | (uint32_t)MDMA_CTCR_DBURST_1)  /*!< Burst 8 beats   */
#define MDMA_DEST_BURST_16BEATS       ((uint32_t)MDMA_CTCR_DBURST_2)                                 /*!< Burst 16 beats  */
#define MDMA_DEST_BURST_32BEATS       ((uint32_t)MDMA_CTCR_DBURST_0 | (uint32_t)MDMA_CTCR_DBURST_2)  /*!< Burst 32 beats  */
#define MDMA_DEST_BURST_64BEATS       ((uint32_t)MDMA_CTCR_DBURST_1 | (uint32_t)MDMA_CTCR_DBURST_2)  /*!< Burst 64 beats  */
#define MDMA_DEST_BURST_128BEATS      ((uint32_t)MDMA_CTCR_DBURST)                                   /*!< Burst 128 beats */

/**
  * @}
  */

/** @defgroup MDMA_interrupt_enable_definitions MDMA interrupt enable definitions
  * @brief    MDMA interrupt enable definitions
  * @{
  */
#define MDMA_IT_TE   ((uint32_t)MDMA_CCR_TEIE)   /*!< Transfer Error interrupt            */
#define MDMA_IT_CTC  ((uint32_t)MDMA_CCR_CTCIE)  /*!< Channel Transfer Complete interrupt */
#define MDMA_IT_BRT  ((uint32_t)MDMA_CCR_BRTIE)  /*!< Block Repeat Transfer interrupt     */
#define MDMA_IT_BT   ((uint32_t)MDMA_CCR_BTIE)   /*!< Block Transfer interrupt            */
#define MDMA_IT_BFTC ((uint32_t)MDMA_CCR_TCIE)   /*!< Buffer Transfer Complete interrupt  */

/**
  * @}
  */

/** @defgroup MDMA_flag_definitions MDMA flag definitions
  * @brief    MDMA flag definitions
  * @{
  */
#define MDMA_FLAG_TE    ((uint32_t)MDMA_CISR_TEIF)  /*!< Transfer Error flag                 */
#define MDMA_FLAG_CTC   ((uint32_t)MDMA_CISR_CTCIF) /*!< Channel Transfer Complete flag      */
#define MDMA_FLAG_BRT   ((uint32_t)MDMA_CISR_BRTIF) /*!< Block Repeat Transfer complete flag */
#define MDMA_FLAG_BT    ((uint32_t)MDMA_CISR_BTIF)  /*!< Block Transfer complete flag        */
#define MDMA_FLAG_BFTC  ((uint32_t)MDMA_CISR_TCIF)  /*!< BuFfer Transfer complete flag       */
#define MDMA_FLAG_CRQA  ((uint32_t)MDMA_CISR_CRQA)  /*!< Channel ReQest Active flag          */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup MDMA_Exported_Macros MDMA Exported Macros
  * @{
  */

/**
  * @brief  Enable the specified MDMA Channel.
  * @param  __HANDLE__: MDMA handle
  * @retval None
  */
#define __HAL_MDMA_ENABLE(__HANDLE__)  ((__HANDLE__)->Instance->CCR |=  MDMA_CCR_EN)

/**
  * @brief  Disable the specified MDMA Channel.
  * @param  __HANDLE__: MDMA handle
  * @retval None
  */
#define __HAL_MDMA_DISABLE(__HANDLE__)  ((__HANDLE__)->Instance->CCR &=  ~MDMA_CCR_EN)

/**
  * @brief  Get the MDMA Channel pending flags.
  * @param  __HANDLE__: MDMA handle
  * @param  __FLAG__: Get the specified flag.
  *          This parameter can be any combination of the following values:
  *            @arg MDMA_FLAG_TE   : Transfer Error flag.
  *            @arg MDMA_FLAG_CTC  : Channel Transfer Complete flag.
  *            @arg MDMA_FLAG_BRT  : Block Repeat Transfer flag.
  *            @arg MDMA_FLAG_BT   : Block Transfer complete flag.
  *            @arg MDMA_FLAG_BFTC : BuFfer Transfer Complete flag.
  *            @arg MDMA_FLAG_CRQA : Channel ReQest Active flag.
  * @retval The state of FLAG (SET or RESET).
  */
#define __HAL_MDMA_GET_FLAG(__HANDLE__, __FLAG__)  ((__HANDLE__)->Instance->CISR & (__FLAG__))

/**
  * @brief  Clear the MDMA Stream pending flags.
  * @param  __HANDLE__: MDMA handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg MDMA_FLAG_TE   : Transfer Error flag.
  *            @arg MDMA_FLAG_CTC  : Channel Transfer Complete flag.
  *            @arg MDMA_FLAG_BRT  : Block Repeat Transfer flag.
  *            @arg MDMA_FLAG_BT   : Block Transfer complete flag.
  *            @arg MDMA_FLAG_BFTC : BuFfer Transfer Complete flag.
  * @retval None
  */
#define __HAL_MDMA_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->CIFCR = (__FLAG__))

/**
  * @brief  Enables the specified MDMA Channel interrupts.
  * @param  __HANDLE__: MDMA handle
  * @param __INTERRUPT__: specifies the DMA interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg MDMA_IT_TE   :  Transfer Error interrupt mask
  *            @arg MDMA_IT_CTC  :  Channel Transfer Complete interrupt mask
  *            @arg MDMA_IT_BRT  :  Block Repeat Transfer interrupt mask
  *            @arg MDMA_IT_BT   :  Block Transfer interrupt mask
  *            @arg MDMA_IT_BFTC :  BuFfer Transfer Complete interrupt mask
  * @retval None
  */
#define __HAL_MDMA_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CCR |= (__INTERRUPT__))

/**
  * @brief  Disables the specified MDMA Channel interrupts.
  * @param  __HANDLE__: MDMA handle
  * @param __INTERRUPT__: specifies the MDMA interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg MDMA_IT_TE   :  Transfer Error interrupt mask
  *            @arg MDMA_IT_CTC  :  Channel Transfer Complete interrupt mask
  *            @arg MDMA_IT_BRT  :  Block Repeat Transfer interrupt mask
  *            @arg MDMA_IT_BT   :  Block Transfer interrupt mask
  *            @arg MDMA_IT_BFTC :  BuFfer Transfer Complete interrupt mask
  * @retval None
  */
#define __HAL_MDMA_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CCR &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified MDMA Channel interrupt is enabled or not.
  * @param  __HANDLE__: DMA handle
  * @param  __INTERRUPT__: specifies the DMA interrupt source to check.
  *            @arg MDMA_IT_TE   :  Transfer Error interrupt mask
  *            @arg MDMA_IT_CTC  :  Channel Transfer Complete interrupt mask
  *            @arg MDMA_IT_BRT  :  Block Repeat Transfer interrupt mask
  *            @arg MDMA_IT_BT   :  Block Transfer interrupt mask
  *            @arg MDMA_IT_BFTC :  BuFfer Transfer Complete interrupt mask
  * @retval The state of MDMA_IT (SET or RESET).
  */
#define __HAL_MDMA_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)  (((__HANDLE__)->Instance->CCR & (__INTERRUPT__)))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup MDMA_Exported_Functions  MDMA Exported Functions
  * @{
  */

/* Initialization and de-initialization functions *****************************/
/** @defgroup MDMA_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief   Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_MDMA_Init(MDMA_HandleTypeDef *hmdma);
HAL_StatusTypeDef HAL_MDMA_DeInit (MDMA_HandleTypeDef *hmdma);
HAL_StatusTypeDef HAL_MDMA_ConfigPostRequestMask(MDMA_HandleTypeDef *hmdma, uint32_t MaskAddress, uint32_t MaskData);

HAL_StatusTypeDef HAL_MDMA_RegisterCallback(MDMA_HandleTypeDef *hmdma, HAL_MDMA_CallbackIDTypeDef CallbackID, void (* pCallback)(MDMA_HandleTypeDef *_hmdma));
HAL_StatusTypeDef HAL_MDMA_UnRegisterCallback(MDMA_HandleTypeDef *hmdma, HAL_MDMA_CallbackIDTypeDef CallbackID);

/**
  * @}
  */

/* Linked list operation functions ********************************************/
/** @defgroup MDMA_Exported_Functions_Group2 Linked List operation functions
  * @brief   Linked list operation functions
  * @{
  */

HAL_StatusTypeDef HAL_MDMA_LinkedList_CreateNode(MDMA_LinkNodeTypeDef *pNode, MDMA_LinkNodeConfTypeDef *pNodeConfig);
HAL_StatusTypeDef HAL_MDMA_LinkedList_AddNode(MDMA_HandleTypeDef *hmdma, MDMA_LinkNodeTypeDef *pNewNode, MDMA_LinkNodeTypeDef *pPrevNode);
HAL_StatusTypeDef HAL_MDMA_LinkedList_RemoveNode(MDMA_HandleTypeDef *hmdma, MDMA_LinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_MDMA_LinkedList_EnableCircularMode(MDMA_HandleTypeDef *hmdma);
HAL_StatusTypeDef HAL_MDMA_LinkedList_DisableCircularMode(MDMA_HandleTypeDef *hmdma);


/**
  * @}
  */

/* IO operation functions *****************************************************/
/** @defgroup MDMA_Exported_Functions_Group3 I/O operation functions
  * @brief   I/O operation functions
  * @{
  */
HAL_StatusTypeDef HAL_MDMA_Start (MDMA_HandleTypeDef *hmdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t BlockDataLength, uint32_t BlockCount);
HAL_StatusTypeDef HAL_MDMA_Start_IT(MDMA_HandleTypeDef *hmdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t BlockDataLength, uint32_t BlockCount);
HAL_StatusTypeDef HAL_MDMA_Abort(MDMA_HandleTypeDef *hmdma);
HAL_StatusTypeDef HAL_MDMA_Abort_IT(MDMA_HandleTypeDef *hmdma);
HAL_StatusTypeDef HAL_MDMA_PollForTransfer(MDMA_HandleTypeDef *hmdma, HAL_MDMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout);
HAL_StatusTypeDef HAL_MDMA_GenerateSWRequest(MDMA_HandleTypeDef *hmdma);
void HAL_MDMA_IRQHandler(MDMA_HandleTypeDef *hmdma);

/**
  * @}
  */

/* Peripheral State and Error functions ***************************************/
/** @defgroup MDMA_Exported_Functions_Group4 Peripheral State functions
  * @brief    Peripheral State functions
  * @{
  */
HAL_MDMA_StateTypeDef HAL_MDMA_GetState(MDMA_HandleTypeDef *hmdma);
uint32_t              HAL_MDMA_GetError(MDMA_HandleTypeDef *hmdma);

/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup MDMA_Private_Types MDMA Private Types
  * @{
  */

/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/** @defgroup MDMA_Private_Defines MDMA Private Defines
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup MDMA_Private_Variables MDMA Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup MDMA_Private_Constants MDMA Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup MDMA_Private_Macros MDMA Private Macros
  * @{
  */

#define IS_MDMA_LEVEL_COMPLETE(__LEVEL__) (((__LEVEL__) == HAL_MDMA_FULL_TRANSFER )  || \
                                           ((__LEVEL__) == HAL_MDMA_BUFFER_TRANSFER )|| \
                                           ((__LEVEL__) == HAL_MDMA_BLOCK_TRANSFER ) || \
                                           ((__LEVEL__) == HAL_MDMA_REPEAT_BLOCK_TRANSFER ))


#define IS_MDMA_PRIORITY(__PRIORITY__) (((__PRIORITY__) == MDMA_PRIORITY_LOW )   || \
                                        ((__PRIORITY__) == MDMA_PRIORITY_MEDIUM) || \
                                        ((__PRIORITY__) == MDMA_PRIORITY_HIGH)   || \
                                        ((__PRIORITY__) == MDMA_PRIORITY_VERY_HIGH))

#define IS_MDMA_ENDIANNESS_MODE(__ENDIANNESS__) (((__ENDIANNESS__) == MDMA_LITTLE_ENDIANNESS_PRESERVE )         || \
                                                 ((__ENDIANNESS__) == MDMA_LITTLE_BYTE_ENDIANNESS_EXCHANGE)     || \
                                                 ((__ENDIANNESS__) == MDMA_LITTLE_HALFWORD_ENDIANNESS_EXCHANGE) || \
                                                 ((__ENDIANNESS__) == MDMA_LITTLE_WORD_ENDIANNESS_EXCHANGE))


#if defined (OCTOSPI2)
#define IS_MDMA_REQUEST(__REQUEST__) (((__REQUEST__) == MDMA_REQUEST_SW ) || ((__REQUEST__) <= MDMA_REQUEST_OCTOSPI2_TC))
#else
#define IS_MDMA_REQUEST(__REQUEST__) (((__REQUEST__) == MDMA_REQUEST_SW ) || ((__REQUEST__) <= MDMA_REQUEST_SDMMC1_COMMAND_END))
#endif /* OCTOSPI2 */

#define IS_MDMA_SOURCE_INC(__INC__) (((__INC__) == MDMA_SRC_INC_DISABLE )   || \
                                     ((__INC__) == MDMA_SRC_INC_BYTE )      || \
                                     ((__INC__) == MDMA_SRC_INC_HALFWORD )  || \
                                     ((__INC__) == MDMA_SRC_INC_WORD )      || \
                                     ((__INC__) == MDMA_SRC_INC_DOUBLEWORD) || \
                                     ((__INC__) == MDMA_SRC_DEC_BYTE)       || \
                                     ((__INC__) == MDMA_SRC_DEC_HALFWORD)   || \
                                     ((__INC__) == MDMA_SRC_DEC_WORD)       || \
                                     ((__INC__) == MDMA_SRC_DEC_DOUBLEWORD))

#define IS_MDMA_DESTINATION_INC(__INC__) (((__INC__) == MDMA_DEST_INC_DISABLE )   || \
                                          ((__INC__) == MDMA_DEST_INC_BYTE )      || \
                                          ((__INC__) == MDMA_DEST_INC_HALFWORD )  || \
                                          ((__INC__) == MDMA_DEST_INC_WORD )      || \
                                          ((__INC__) == MDMA_DEST_INC_DOUBLEWORD) || \
                                          ((__INC__) == MDMA_DEST_DEC_BYTE)       || \
                                          ((__INC__) == MDMA_DEST_DEC_HALFWORD)   || \
                                          ((__INC__) == MDMA_DEST_DEC_WORD)       || \
                                          ((__INC__) == MDMA_DEST_DEC_DOUBLEWORD))

#define IS_MDMA_SOURCE_DATASIZE(__SIZE__) (((__SIZE__) == MDMA_SRC_DATASIZE_BYTE )     || \
                                           ((__SIZE__) == MDMA_SRC_DATASIZE_HALFWORD ) || \
                                           ((__SIZE__) == MDMA_SRC_DATASIZE_WORD )     || \
                                           ((__SIZE__) == MDMA_SRC_DATASIZE_DOUBLEWORD))

#define IS_MDMA_DESTINATION_DATASIZE(__SIZE__) (((__SIZE__) == MDMA_DEST_DATASIZE_BYTE )     || \
                                                ((__SIZE__) == MDMA_DEST_DATASIZE_HALFWORD ) || \
                                                ((__SIZE__) == MDMA_DEST_DATASIZE_WORD )     || \
                                                ((__SIZE__) == MDMA_DEST_DATASIZE_DOUBLEWORD))

#define IS_MDMA_DATA_ALIGNMENT(__ALIGNMENT__) (((__ALIGNMENT__) == MDMA_DATAALIGN_PACKENABLE )    || \
                                               ((__ALIGNMENT__) == MDMA_DATAALIGN_RIGHT )         || \
                                               ((__ALIGNMENT__) == MDMA_DATAALIGN_RIGHT_SIGNED )  || \
                                               ((__ALIGNMENT__) == MDMA_DATAALIGN_LEFT))


#define IS_MDMA_SOURCE_BURST(__BURST__) (((__BURST__) == MDMA_SOURCE_BURST_SINGLE ) || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_2BEATS ) || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_4BEATS ) || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_8BEATS)  || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_16BEATS) || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_32BEATS) || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_64BEATS) || \
                                         ((__BURST__) == MDMA_SOURCE_BURST_128BEATS))


#define IS_MDMA_DESTINATION_BURST(__BURST__) (((__BURST__) == MDMA_DEST_BURST_SINGLE ) || \
                                              ((__BURST__) == MDMA_DEST_BURST_2BEATS ) || \
                                              ((__BURST__) == MDMA_DEST_BURST_4BEATS ) || \
                                              ((__BURST__) == MDMA_DEST_BURST_8BEATS)  || \
                                              ((__BURST__) == MDMA_DEST_BURST_16BEATS) || \
                                              ((__BURST__) == MDMA_DEST_BURST_32BEATS) || \
                                              ((__BURST__) == MDMA_DEST_BURST_64BEATS) || \
                                              ((__BURST__) == MDMA_DEST_BURST_128BEATS))

 #define IS_MDMA_TRANSFER_TRIGGER_MODE(__MODE__) (((__MODE__) == MDMA_BUFFER_TRANSFER )      || \
                                                  ((__MODE__) == MDMA_BLOCK_TRANSFER )        || \
                                                  ((__MODE__) == MDMA_REPEAT_BLOCK_TRANSFER ) || \
                                                  ((__MODE__) == MDMA_FULL_TRANSFER))

#define IS_MDMA_BUFFER_TRANSFER_LENGTH(__LENGTH__) (((__LENGTH__) >= 0x00000001U) && ((__LENGTH__) < 0x000000FFU))

#define IS_MDMA_BLOCK_COUNT(__COUNT__) (((__COUNT__) > 0U ) && ((__COUNT__) <= 4096U))

#define IS_MDMA_TRANSFER_LENGTH(SIZE) (((SIZE) > 0U) && ((SIZE) <= 65536U))

#define IS_MDMA_BLOCK_ADDR_OFFSET(__BLOCK_ADD_OFFSET__) (((__BLOCK_ADD_OFFSET__) > (-65536)) && ((__BLOCK_ADD_OFFSET__) < 65536))

/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup MDMA_Private_Functions_Prototypes MDMA Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup MDMA_Private_Functions MDMA Private Functions
  * @{
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

#endif /* STM32H7xx_HAL_MDMA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
