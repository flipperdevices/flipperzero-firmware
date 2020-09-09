/**
  ******************************************************************************
  * @file    stm32h7xx_ll_mdma.h
  * @author  MCD Application Team
  * @brief   Header file of MDMA LL module.
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
#ifndef STM32H7xx_LL_MDMA_H
#define STM32H7xx_LL_MDMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (MDMA)

/** @defgroup MDMA_LL MDMA
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup MDMA_LL_Private_Variables MDMA Private Variables
  * @{
  */
/* Array used to get the MDMA channel register offset versus channel index LL_MDMA_CHANNEL_x */
static const uint32_t LL_MDMA_CH_OFFSET_TAB[] =
{
  (uint32_t)(MDMA_Channel0_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel1_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel2_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel3_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel4_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel5_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel6_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel7_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel8_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel9_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel10_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel11_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel12_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel13_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel14_BASE - MDMA_BASE),
  (uint32_t)(MDMA_Channel15_BASE - MDMA_BASE)
};

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup MDMA_LL_Private_Constants MDMA Private Constants
  * @{
  */
/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup MDMA_LL_ES_INIT MDMA Exported Init structure
  * @{
  */
typedef struct
{
  uint32_t SrcAddress;            /*!< Specifies the transfer source address
                                       This parameter must be a value between Min_Data = 0 and Max_Data = 0xFFFFFFFF.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetSourceAddress() */

  uint32_t DstAddress;            /*!< Specifies the transfer destination address
                                       This parameter must be a value between Min_Data = 0 and Max_Data = 0xFFFFFFFF.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetDestinationAddress() */

  uint32_t RequestMode;           /*!< Specifies the request mode Hardware or Software.
                                       This parameter can be a value of @ref MDMA_LL_EC_REQUEST_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetRequestMode() */

  uint32_t TriggerMode;           /*!< Specifies the transfer trigger mode.
                                       This parameter can be a value of @ref MDMA_LL_EC_TRIGGER_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetTriggerMode() */

  uint32_t HWTrigger;             /*!< Specifies the HW transfer trigger used when RequestMode is HW.
                                       This parameter can be a value of @ref MDMA_LL_EC_HW_TRIGGER_SELCTION
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetHWTrigger() */

  uint32_t BlockDataLength;       /*!< Specifies the length of a block transfer in bytes
                                       This parameter must be a value between Min_Data = 0 and Max_Data = 0x00010000.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBlkDataLength() */

  uint32_t BlockRepeatCount;      /*!< Specifies the Block Repeat Count
                                       This parameter must be a value between Min_Data = 0 and Max_Data = 0x00000FFF.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBlkRepeatCount() */

  uint32_t BlockRepeatDestAddrUpdateMode; /*!< Specifies the block repeat destination address update mode.
                                               This parameter can be a value of @ref MDMA_LL_EC_BLK_RPT_DEST_ADDR_UPDATE_MODE
                                               This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBlkRepeatDestAddrUpdate() */

  uint32_t BlockRepeatSrcAddrUpdateMode;  /*!< Specifies the block repeat source address update mode.
                                               This parameter can be a value of @ref MDMA_LL_EC_SRC_BLK_RPT_ADDR_UPDATE_MODE
                                               This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBlkRepeatSrcAddrUpdate() */

  uint32_t BlockRepeatDestAddrUpdateVal;  /*!< Specifies the block repeat destination address update value.
                                               This parameter can be a value Between 0 to 0x0000FFFF
                                               This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBlkRptDestAddrUpdateValue() */

  uint32_t BlockRepeatSrcAddrUpdateVal;   /*!< Specifies the block repeat source address update value.
                                               This parameter can be a value Between 0 to 0x0000FFFF
                                               This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBlkRptSrcAddrUpdateValue() */

  uint32_t LinkAddress;                   /*!< Specifies the linked list next transfer node address.
                                               This parameter can be a value Between 0 to 0xFFFFFFFF
                                               This feature can be modified afterwards using unitary function @ref LL_MDMA_SetLinkAddress() */

  uint32_t WordEndianess;         /*!< Specifies the Word transfer endianness
                                       This parameter can be a value of @ref MDMA_LL_EC_WORD_ENDIANNESS.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetWordEndianness() */

  uint32_t HalfWordEndianess;     /*!< Specifies the Half Word transfer endianness
                                       This parameter can be a value of @ref MDMA_LL_EC_HALFWORD_ENDIANNESS.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetHalfWordEndianness() */

  uint32_t ByteEndianess;         /*!< Specifies the Byte transfer endianness
                                       This parameter can be a value of @ref MDMA_LL_EC_BYTE_ENDIANNESS.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetByteEndianness() */

  uint32_t Priority;              /*!< Specifies the channel priority level.
                                       This parameter can be a value of @ref MDMA_LL_EC_PRIORITY
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetChannelPriorityLevel() */

  uint32_t BufferableWriteMode;   /*!< Specifies the transfer Bufferable Write Mode.
                                       This parameter can be a value of @ref MDMA_LL_EC_BUFF_WRITE_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_EnableBufferableWrMode()
                                       and LL_MDMA_DisableBufferableWrMode */


  uint32_t PaddingAlignment;     /*!< Specifies the transfer Padding and Alignment.
                                       This parameter can be a value of @ref MDMA_LL_EC_PADDING_ALIGNMENT_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetPaddingAlignment() */

  uint32_t PackMode;              /*!< Specifies the transfer Packing enabled or disabled.
                                       This parameter can be a value of @ref MDMA_LL_EC_PACKING_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_EnablePacking()
                                       and LL_MDMA_DisablePacking() */

  uint32_t BufferTransferLength;  /*!< Specifies the length of a buffer transfer in bytes
                                       This parameter must be a value between Min_Data = 0 and Max_Data = 0x0000007F.
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetBufferTransferLength() */

  uint32_t DestBurst;             /*!< Specifies the destination burst size.
                                       This parameter can be a value of @ref MDMA_LL_EC_DEST_BURST
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetDestinationBurstSize() */

  uint32_t SrctBurst;             /*!< Specifies the source burst size.
                                       This parameter can be a value of @ref MDMA_LL_EC_SRC_BURST
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetSourceBurstSize() */

  uint32_t DestIncSize;           /*!< Specifies the destination increment size.
                                       This parameter can be a value of @ref MDMA_LL_EC_DEST_INC_OFFSET_SIZE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetDestinationIncSize() */

  uint32_t SrcIncSize;            /*!< Specifies the source increment size.
                                       This parameter can be a value of @ref MDMA_LL_EC_SRC_INC_OFFSET_SIZE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetSourceIncSize() */

  uint32_t DestDataSize;          /*!< Specifies the destination data size.
                                       This parameter can be a value of @ref MDMA_LL_EC_DEST_DATA_SIZE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetDestinationDataSize() */

  uint32_t SrcDataSize;           /*!< Specifies the source data size.
                                       This parameter can be a value of @ref MDMA_LL_EC_SRC_DATA_SIZE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetSourceDataSize() */

  uint32_t DestIncMode;           /*!< Specifies the destination increment mode.
                                       This parameter can be a value of @ref MDMA_LL_EC_DEST_INC_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetDestinationIncMode() */

  uint32_t SrcIncMode;            /*!< Specifies the source increment mode.
                                       This parameter can be a value of @ref MDMA_LL_EC_SRC_INC_MODE
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetSourceIncMode() */


  uint32_t DestBus;               /*!< Specifies the destination transfer bus, System AXI or AHB/TCM bus.
                                       This parameter can be a value of @ref MDMA_LL_EC_DEST_BUS
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetDestBusSelection() */

  uint32_t SrcBus;                /*!< Specifies the source transfer bus, System AXI or AHB/TCM bus.
                                       This parameter can be a value of @ref MDMA_LL_EC_SRC_BUS
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetSrcBusSelection() */

  uint32_t MaskAddress;           /*!< Specifies the address to be updated (written) with MaskData after a request is served.
                                       MaskAddress and MaskData could be used to automatically clear a peripheral flag when the request is served
                                       This parameter can be a value Between 0 to 0xFFFFFFFF
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetMaskAddress() */

  uint32_t MaskData;              /*!< Specifies the value to be written to MaskAddress after a request is served.
                                       MaskAddress and MaskData could be used to automatically clear a peripheral flag when the request is served
                                       This parameter can be a value Between 0 to 0xFFFFFFFF
                                       This feature can be modified afterwards using unitary function @ref LL_MDMA_SetMaskData() */

} LL_MDMA_InitTypeDef;

/**
  * @brief  LL MDMA linked list node structure definition
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
  __IO uint32_t Reserved; /*!< Reserved register*/
  __IO uint32_t CMAR;     /*!< New CMAR register configuration for the given MDMA linked list node   */
  __IO uint32_t CMDR;     /*!< New CMDR register configuration for the given MDMA linked list node   */

}LL_MDMA_LinkNodeTypeDef;

/**
  * @}
  */
#endif /*USE_FULL_LL_DRIVER*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup MDMA_LL_Exported_Constants MDMA Exported Constants
  * @{
  */

/** @defgroup MDMA_LL_EC_CHANNEL CHANNEL
  * @{
  */
#define LL_MDMA_CHANNEL_0                   0x00000000U
#define LL_MDMA_CHANNEL_1                   0x00000001U
#define LL_MDMA_CHANNEL_2                   0x00000002U
#define LL_MDMA_CHANNEL_3                   0x00000003U
#define LL_MDMA_CHANNEL_4                   0x00000004U
#define LL_MDMA_CHANNEL_5                   0x00000005U
#define LL_MDMA_CHANNEL_6                   0x00000006U
#define LL_MDMA_CHANNEL_7                   0x00000007U
#define LL_MDMA_CHANNEL_8                   0x00000008U
#define LL_MDMA_CHANNEL_9                   0x00000009U
#define LL_MDMA_CHANNEL_10                  0x0000000AU
#define LL_MDMA_CHANNEL_11                  0x0000000BU
#define LL_MDMA_CHANNEL_12                  0x0000000CU
#define LL_MDMA_CHANNEL_13                  0x0000000DU
#define LL_MDMA_CHANNEL_14                  0x0000000EU
#define LL_MDMA_CHANNEL_15                  0x0000000FU
#define LL_MDMA_CHANNEL_ALL                 0xFFFF0000U
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_WORD_ENDIANNESS Word Endianness
  * @{
  */
#define LL_MDMA_WORD_ENDIANNESS_PRESERVE     0x00000000U  /*!< Little endianess preserved for words */
#define LL_MDMA_WORD_ENDIANNESS_EXCHANGE     MDMA_CCR_WEX /*!< word order exchanged when destination data size is double word  */

/**
  * @}
  */

/** @defgroup MDMA_LL_EC_HALFWORD_ENDIANNESS Half Word Endianness
  * @{
  */
#define LL_MDMA_HALFWORD_ENDIANNESS_PRESERVE     0x00000000U  /*!< Little endianess preserved for half words */
#define LL_MDMA_HALFWORD_ENDIANNESS_EXCHANGE     MDMA_CCR_HEX /*!< half word order exchanged when destination data size is word or double word  */

/**
  * @}
  */

/** @defgroup MDMA_LL_EC_BYTE_ENDIANNESS Byte Endianness
  * @{
  */
#define LL_MDMA_BYTE_ENDIANNESS_PRESERVE     0x00000000U  /*!< Little endianess preserved for bytes */
#define LL_MDMA_BYTE_ENDIANNESS_EXCHANGE     MDMA_CCR_BEX /*!< byte order exchanged when destination data size is half word , word or double word  */

/**
  * @}
  */

/** @defgroup MDMA_LL_EC_PRIORITY Transfer Priority level
  * @{
  */
#define LL_MDMA_PRIORITY_LOW               0x00000000U               /*!< Priority level : Low       */
#define LL_MDMA_PRIORITY_MEDIUM            MDMA_CCR_PL_0             /*!< Priority level : Medium    */
#define LL_MDMA_PRIORITY_HIGH              MDMA_CCR_PL_1             /*!< Priority level : High      */
#define LL_MDMA_PRIORITY_VERYHIGH          MDMA_CCR_PL               /*!< Priority level : Very_High */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_BUFF_WRITE_MODE Bufferable Write Mode
  * @{
  */
#define LL_MDMA_BUFF_WRITE_DISABLE         0x00000000U               /*!< destination write operation is non-bufferable */
#define LL_MDMA_BUFF_WRITE_ENABLE          MDMA_CTCR_BWM             /*!< destination write operation is bufferable     */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_REQUEST_MODE Request Mode
  * @{
  */
#define LL_MDMA_REQUEST_MODE_HW            0x00000000U               /*!< Request mode is Hardware    */
#define LL_MDMA_REQUEST_MODE_SW            MDMA_CTCR_SWRM            /*!< Request mode is Software    */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_TRIGGER_MODE Trigger Mode
  * @{
  */
#define LL_MDMA_BUFFER_TRANSFER          0x00000000U               /*!< Each MDMA request (SW or HW) triggers a buffer transfer */
#define LL_MDMA_BLOCK_TRANSFER           MDMA_CTCR_TRGM_0          /*!< Each MDMA request (SW or HW) triggers a block transfer */
#define LL_MDMA_REPEAT_BLOCK_TRANSFER    MDMA_CTCR_TRGM_1          /*!< Each MDMA request (SW or HW) triggers a repeated block transfer */
#define LL_MDMA_FULL_TRANSFER            MDMA_CTCR_TRGM            /*!< Each MDMA request (SW or HW) triggers a Full transfer or a linked list transfer if any */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_PADDING_ALIGNMENT_MODE Padding Alignment Mode
  * @{
  */
#define LL_MDMA_DATAALIGN_RIGHT          0x00000000U              /*!< Right Aligned, padded w/ 0s (default) */
#define LL_MDMA_DATAALIGN_RIGHT_SIGNED   MDMA_CTCR_PAM_0          /*!< Right Aligned, Sign extended ,
                                                                       Note : this mode is allowed only if the Source data size smaller than Destination data size  */
#define LL_MDMA_DATAALIGN_LEFT           MDMA_CTCR_PAM_1          /*!< Left Aligned (padded with 0s) */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_PACKING_MODE Transfer Packing
  * @{
  */
#define LL_MDMA_PACK_DISABLE          0x00000000U              /*!< Packing disabled */
#define LL_MDMA_PACK_ENABLE           MDMA_CTCR_PKE            /*!< Packing enabled  */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_DEST_BURST Transfer Destination Burst
  * @{
  */
#define LL_MDMA_DEST_BURST_SINGLE        0x00000000U                               /*!< Single transfer */
#define LL_MDMA_DEST_BURST_2BEATS        MDMA_CTCR_DBURST_0                        /*!< Burst 2 beats   */
#define LL_MDMA_DEST_BURST_4BEATS        MDMA_CTCR_DBURST_1                        /*!< Burst 4 beats   */
#define LL_MDMA_DEST_BURST_8BEATS       (MDMA_CTCR_DBURST_0 | MDMA_CTCR_DBURST_1)  /*!< Burst 8 beats   */
#define LL_MDMA_DEST_BURST_16BEATS       MDMA_CTCR_DBURST_2                        /*!< Burst 16 beats  */
#define LL_MDMA_DEST_BURST_32BEATS      (MDMA_CTCR_DBURST_0 | MDMA_CTCR_DBURST_2)  /*!< Burst 32 beats  */
#define LL_MDMA_DEST_BURST_64BEATS      (MDMA_CTCR_DBURST_1 | MDMA_CTCR_DBURST_2)  /*!< Burst 64 beats  */
#define LL_MDMA_DEST_BURST_128BEATS     (MDMA_CTCR_DBURST)                         /*!< Burst 128 beats */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_SRC_BURST Transfer Source Burst
  * @{
  */
#define LL_MDMA_SRC_BURST_SINGLE        0x00000000U                                /*!< Single transfer */
#define LL_MDMA_SRC_BURST_2BEATS        MDMA_CTCR_SBURST_0                         /*!< Burst 2 beats   */
#define LL_MDMA_SRC_BURST_4BEATS        MDMA_CTCR_SBURST_1                         /*!< Burst 4 beats   */
#define LL_MDMA_SRC_BURST_8BEATS       (MDMA_CTCR_SBURST_0 | MDMA_CTCR_SBURST_1)   /*!< Burst 8 beats   */
#define LL_MDMA_SRC_BURST_16BEATS       MDMA_CTCR_SBURST_2                         /*!< Burst 16 beats  */
#define LL_MDMA_SRC_BURST_32BEATS      (MDMA_CTCR_SBURST_0 | MDMA_CTCR_SBURST_2)   /*!< Burst 32 beats  */
#define LL_MDMA_SRC_BURST_64BEATS      (MDMA_CTCR_SBURST_1 | MDMA_CTCR_SBURST_2)   /*!< Burst 64 beats  */
#define LL_MDMA_SRC_BURST_128BEATS      MDMA_CTCR_SBURST                           /*!< Burst 128 beats */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_DEST_INC_OFFSET_SIZE Destination Increment Offset Size
  * @{
  */
#define LL_MDMA_DEST_INC_OFFSET_BYTE        0x00000000U                           /*!< offset is Byte (8-bit)         */
#define LL_MDMA_DEST_INC_OFFSET_HALFWORD    MDMA_CTCR_DINCOS_0                    /*!< offset is Half Word (16-bit)   */
#define LL_MDMA_DEST_INC_OFFSET_WORD        MDMA_CTCR_DINCOS_1                    /*!< offset is Word (32-bit)        */
#define LL_MDMA_DEST_INC_OFFSET_DOUBLEWORD  MDMA_CTCR_DINCOS                      /*!< offset is Double Word (64-bit) */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_SRC_INC_OFFSET_SIZE Source Increment Offset Size
  * @{
  */
#define LL_MDMA_SRC_INC_OFFSET_BYTE        0x00000000U                           /*!< offset is Byte (8-bit)         */
#define LL_MDMA_SRC_INC_OFFSET_HALFWORD    MDMA_CTCR_SINCOS_0                    /*!< offset is Half Word (16-bit)   */
#define LL_MDMA_SRC_INC_OFFSET_WORD        MDMA_CTCR_SINCOS_1                    /*!< offset is Word (32-bit)        */
#define LL_MDMA_SRC_INC_OFFSET_DOUBLEWORD  MDMA_CTCR_SINCOS                      /*!< offset is Double Word (64-bit) */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_DEST_DATA_SIZE Destination Data Size
  * @{
  */
#define LL_MDMA_DEST_DATA_SIZE_BYTE       0x00000000U                            /*!< Destination data size is Byte        */
#define LL_MDMA_DEST_DATA_SIZE_HALFWORD   MDMA_CTCR_DSIZE_0                      /*!< Destination data size is half word   */
#define LL_MDMA_DEST_DATA_SIZE_WORD       MDMA_CTCR_DSIZE_1                      /*!< Destination data size is word        */
#define LL_MDMA_DEST_DATA_SIZE_DOUBLEWORD MDMA_CTCR_DSIZE                        /*!< Destination data size is double word */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_SRC_DATA_SIZE Source Data Size
  * @{
  */
#define LL_MDMA_SRC_DATA_SIZE_BYTE       0x00000000U                            /*!< Source data size is Byte        */
#define LL_MDMA_SRC_DATA_SIZE_HALFWORD   MDMA_CTCR_SSIZE_0                      /*!< Source data size is half word   */
#define LL_MDMA_SRC_DATA_SIZE_WORD       MDMA_CTCR_SSIZE_1                      /*!< Source data size is word        */
#define LL_MDMA_SRC_DATA_SIZE_DOUBLEWORD MDMA_CTCR_SSIZE                        /*!< Source data size is double word */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_DEST_INC_MODE Destination Increment Mode
  * @{
  */
#define LL_MDMA_DEST_FIXED               0x00000000U                            /*!< Destination address pointer is fixed */
#define LL_MDMA_DEST_INCREMENT           MDMA_CTCR_DINC_1                       /*!< Destination address pointer is incremented after each data transfer */
#define LL_MDMA_DEST_DECREMENT           MDMA_CTCR_DINC                         /*!< Destination address pointer is decremented after each data transfer */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_SRC_INC_MODE Source Increment Mode
  * @{
  */
#define LL_MDMA_SRC_FIXED                0x00000000U                            /*!< Destination address pointer is fixed */
#define LL_MDMA_SRC_INCREMENT            MDMA_CTCR_SINC_1                       /*!< Destination address pointer is incremented after each data transfer */
#define LL_MDMA_SRC_DECREMENT            MDMA_CTCR_SINC                         /*!< Destination address pointer is decremented after each data transfer */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_BLK_RPT_DEST_ADDR_UPDATE_MODE Block Repeat Destination address Update Mode
  * @{
  */
#define LL_MDMA_BLK_RPT_DEST_ADDR_INCREMENT   0x00000000U                       /*!< Destination address pointer is incremented after each block transfer by Destination Update Value */
#define LL_MDMA_BLK_RPT_DEST_ADDR_DECREMENT   MDMA_CBNDTR_BRDUM                 /*!< Destination address pointer is decremented after each block transfer by Destination Update Value */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_SRC_BLK_RPT_ADDR_UPDATE_MODE Source Block Repeat address Update Mode
  * @{
  */
#define LL_MDMA_BLK_RPT_SRC_ADDR_INCREMENT   0x00000000U                        /*!< Source address pointer is incremented after each block transfer by Source Update Value */
#define LL_MDMA_BLK_RPT_SRC_ADDR_DECREMENT   MDMA_CBNDTR_BRSUM                  /*!< Source address pointer is decremented after each block transfer by Source Update Value */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_DEST_BUS Destination BUS Selection
  * @{
  */
#define  LL_MDMA_DEST_BUS_SYSTEM_AXI    0x00000000U                             /*!< System/AXI bus is used as destination */
#define  LL_MDMA_DEST_BUS_AHB_TCM       MDMA_CTBR_DBUS                          /*!< AHB bus/TCM is used as destination    */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_SRC_BUS Source BUS Selection
  * @{
  */
#define LL_MDMA_SRC_BUS_SYSTEM_AXI     0x00000000U                             /*!< System/AXI bus is used as source */
#define LL_MDMA_SRC_BUS_AHB_TCM        MDMA_CTBR_SBUS                          /*!< AHB bus/TCM is used as source    */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_HW_TRIGGER_SELCTION HW Trigger Selection
  * @{
  */
#define LL_MDMA_REQ_DMA1_STREAM0_TC          0x00000000U    /*!< MDMA HW Trigger (request) is DMA1 Stream 0 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM1_TC          0x00000001U    /*!< MDMA HW Trigger (request) is DMA1 Stream 1 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM2_TC          0x00000002U    /*!< MDMA HW Trigger (request) is DMA1 Stream 2 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM3_TC          0x00000003U    /*!< MDMA HW Trigger (request) is DMA1 Stream 3 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM4_TC          0x00000004U    /*!< MDMA HW Trigger (request) is DMA1 Stream 4 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM5_TC          0x00000005U    /*!< MDMA HW Trigger (request) is DMA1 Stream 5 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM6_TC          0x00000006U    /*!< MDMA HW Trigger (request) is DMA1 Stream 6 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA1_STREAM7_TC          0x00000007U    /*!< MDMA HW Trigger (request) is DMA1 Stream 7 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM0_TC          0x00000008U    /*!< MDMA HW Trigger (request) is DMA2 Stream 0 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM1_TC          0x00000009U    /*!< MDMA HW Trigger (request) is DMA2 Stream 1 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM2_TC          0x0000000AU    /*!< MDMA HW Trigger (request) is DMA2 Stream 2 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM3_TC          0x0000000BU    /*!< MDMA HW Trigger (request) is DMA2 Stream 3 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM4_TC          0x0000000CU    /*!< MDMA HW Trigger (request) is DMA2 Stream 4 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM5_TC          0x0000000DU    /*!< MDMA HW Trigger (request) is DMA2 Stream 5 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM6_TC          0x0000000EU    /*!< MDMA HW Trigger (request) is DMA2 Stream 6 Transfer Complete Flag   */
#define LL_MDMA_REQ_DMA2_STREAM7_TC          0x0000000FU    /*!< MDMA HW Trigger (request) is DMA2 Stream 7 Transfer Complete Flag   */
#if defined (LTDC)
#define LL_MDMA_REQ_LTDC_LINE_IT             0x00000010U    /*!< MDMA HW Trigger (request) is LTDC Line interrupt Flag               */
#endif /* LTDC */
#if defined (JPEG)
#define LL_MDMA_REQ_JPEG_INFIFO_TH           0x00000011U    /*!< MDMA HW Trigger (request) is JPEG Input FIFO threshold Flag         */
#define LL_MDMA_REQ_JPEG_INFIFO_NF           0x00000012U    /*!< MDMA HW Trigger (request) is JPEG Input FIFO not full Flag          */
#define LL_MDMA_REQ_JPEG_OUTFIFO_TH          0x00000013U    /*!< MDMA HW Trigger (request) is JPEG Output FIFO threshold Flag        */
#define LL_MDMA_REQ_JPEG_OUTFIFO_NE          0x00000014U    /*!< MDMA HW Trigger (request) is JPEG Output FIFO not empty Flag        */
#define LL_MDMA_REQ_JPEG_END_CONVERSION      0x00000015U    /*!< MDMA HW Trigger (request) is JPEG End of conversion Flag            */
#endif /* JPEG */
#if defined (QUADSPI)
#define LL_MDMA_REQ_QUADSPI_FIFO_TH          0x00000016U    /*!< MDMA HW Trigger (request) is QSPI FIFO threshold Flag               */
#define LL_MDMA_REQ_QUADSPI_TC               0x00000017U    /*!< MDMA HW Trigger (request) is QSPI Transfer complete Flag            */
#endif /* QUADSPI */
#if defined (OCTOSPI1)
#define LL_MDMA_REQ_OCTOSPI1_FIFO_TH         0x00000016U    /*!< MDMA HW Trigger (request) is OCTOSPI1 FIFO threshold Flag           */
#define LL_MDMA_REQ_OCTOSPI1_TC              0x00000017U    /*!< MDMA HW Trigger (request) is OCTOSPI1 Transfer complete Flag        */
#endif /* OCTOSPI1 */
#define LL_MDMA_REQ_DMA2D_CLUT_TC            0x00000018U    /*!< MDMA HW Trigger (request) is DMA2D CLUT Transfer Complete Flag      */
#define LL_MDMA_REQ_DMA2D_TC                 0x00000019U    /*!< MDMA HW Trigger (request) is DMA2D Transfer Complete Flag           */
#define LL_MDMA_REQ_DMA2D_TW                 0x0000001AU    /*!< MDMA HW Trigger (request) is DMA2D Transfer Watermark Flag          */
#if defined (DSI)
#define LL_MDMA_REQ_DSI_TEARING_EFFECT       0x0000001BU    /*!< MDMA HW Trigger (request) is DSI Tearing Effect Flag                */
#define LL_MDMA_REQ_DSI_END_REFRESH          0x0000001CU    /*!< MDMA HW Trigger (request) is DSI End of refresh  Flag               */
#endif /* DSI */
#define LL_MDMA_REQ_SDMMC1_END_DATA          0x0000001DU    /*!< MDMA HW Trigger (request) is SDMMC1 End of Data Flag                */
#define LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER     0x0000001EU    /*!< MDMA HW Trigger (request) is SDMMC1 Internal DMA buffer End Flag : This trigger is available starting from STM32H7 Rev.B devices */
#define LL_MDMA_REQ_SDMMC1_COMMAND_END       0x0000001FU    /*!< MDMA HW Trigger (request) is SDMMC1 Command End Flag : This trigger is available starting from STM32H7 Rev.B devices */
#if defined (OCTOSPI2)
#define LL_MDMA_REQ_OCTOSPI2_FIFO_TH         0x00000020U    /*!< MDMA HW Trigger (request) is OCTOSPI2 FIFO threshold Flag           */
#define LL_MDMA_REQ_OCTOSPI2_TC              0x00000021U    /*!< MDMA HW Trigger (request) is OCTOSPI2 Transfer complete Flag        */
#endif /* OCTOSPI2 */
/**
  * @}
  */

/** @defgroup MDMA_LL_EC_XFER_ERROR_DIRECTION Transfer Error Direction
  * @{
  */
#define LL_MDMA_READ_ERROR                       0x00000000U                /*!< Last transfer error on the channel was a related to a read access    */
#define LL_MDMA_WRITE_ERROR                      MDMA_CESR_TED              /*!< Last transfer error on the channel was a related to a write access   */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup MDMA_LL_Exported_Macros MDMA Exported Macros
  * @{
  */

/** @defgroup MDMA_LL_EM_WRITE_READ Common Write and read registers macros
  * @{
  */
/**
  * @brief  Write a value in MDMA register
  * @param  __INSTANCE__ MDMA Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_MDMA_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in MDMA register
  * @param  __INSTANCE__ MDMA Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_MDMA_ReadReg(__INSTANCE__, __REG__) READ_REG((__INSTANCE__)->__REG__)
/**
  * @}
  */

/** @defgroup MDMA_LL_EM_CONVERT_DMAxCHANNELy Convert MDMAxChannely
  * @{
  */
/**
  * @brief  Convert MDMAx_Channely into MDMAx
  * @param  __CHANNEL_INSTANCE__ MDMAx_Channely
  * @retval MDMAx
  */
#define LL_MDMA_GET_INSTANCE(__CHANNEL_INSTANCE__) (MDMA)

/**
  * @brief  Convert MDMAx_Channely into LL_MDMA_CHANNEL_y
  * @param  __CHANNEL_INSTANCE__ MDMAx_Channely
  * @retval LL_MDMA_CHANNEL_y
  */
#define LL_MDMA_GET_CHANNEL(__CHANNEL_INSTANCE__)   \
(((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel0 )) ? LL_MDMA_CHANNEL_0  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel1 )) ? LL_MDMA_CHANNEL_1  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel2 )) ? LL_MDMA_CHANNEL_2  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel3 )) ? LL_MDMA_CHANNEL_3  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel4 )) ? LL_MDMA_CHANNEL_4  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel5 )) ? LL_MDMA_CHANNEL_5  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel6 )) ? LL_MDMA_CHANNEL_6  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel7 )) ? LL_MDMA_CHANNEL_7  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel8 )) ? LL_MDMA_CHANNEL_8  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel9 )) ? LL_MDMA_CHANNEL_9  : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel10)) ? LL_MDMA_CHANNEL_10 : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel11)) ? LL_MDMA_CHANNEL_11 : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel12)) ? LL_MDMA_CHANNEL_12 : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel13)) ? LL_MDMA_CHANNEL_13 : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)MDMA_Channel14)) ? LL_MDMA_CHANNEL_14 : \
 LL_MDMA_CHANNEL_15)

/**
  * @brief  Convert MDMA Instance MDMAx and LL_MDMA_CHANNEL_y into MDMAx_Channely
  * @param  __MDMA_INSTANCE__ MDMAx
  * @param  __CHANNEL__ LL_MDMA_CHANNEL_y
  * @retval MDMAx_Channely
  */
#define LL_MDMA_GET_CHANNEL_INSTANCE(__MDMA_INSTANCE__, __CHANNEL__)   \
(((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_0 )) ? MDMA_Channel0  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_1 )) ? MDMA_Channel1  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_2 )) ? MDMA_Channel2  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_3 )) ? MDMA_Channel3  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_4 )) ? MDMA_Channel4  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_5 )) ? MDMA_Channel5  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_6 )) ? MDMA_Channel6  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_7 )) ? MDMA_Channel7  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_8 )) ? MDMA_Channel8  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_9 )) ? MDMA_Channel9  : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_10)) ? MDMA_Channel10 : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_11)) ? MDMA_Channel11 : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_12)) ? MDMA_Channel12 : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_13)) ? MDMA_Channel13 : \
 ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_MDMA_CHANNEL_14)) ? MDMA_Channel14 : \
 MDMA_Channel15)

/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
 /** @defgroup MDMA_LL_Exported_Functions MDMA Exported Functions
  * @{
  */

/** @defgroup MDMA_LL_EF_Configuration Configuration
  * @{
  */
/**
  * @brief Enable MDMA channel.
  * @rmtoll CCR          EN            LL_MDMA_EnableChannel
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableChannel(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_EN);
}

/**
  * @brief Disable MDMA channel.
  * @rmtoll CCR          EN            LL_MDMA_DisableChannel
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableChannel(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_EN);
}

/**
  * @brief Check if MDMA channel is enabled or disabled.
  * @rmtoll CCR          EN            LL_MDMA_IsEnabledChannel
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledChannel(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_EN) == (MDMA_CCR_EN)) ? 1UL : 0UL);
}

/**
  * @brief Generate a SW transfer request on the MDMA channel.
  * @rmtoll CCR          SWRQ            LL_MDMA_GenerateSWRequest
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_GenerateSWRequest(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_SWRQ);
}

/**
  * @brief Configure Transfer endianness paremeters : Word, Half word and Bytes Endianess.
  * @rmtoll CCR          WEX            LL_MDMA_ConfigXferEndianness\n
  * @rmtoll CCR          HEX            LL_MDMA_ConfigXferEndianness\n
  * @rmtoll CCR          BEX            LL_MDMA_ConfigXferEndianness
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
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_MDMA_WORD_ENDIANNESS_PRESERVE or @ref LL_MDMA_WORD_ENDIANNESS_EXCHANGE
  *         @arg @ref LL_MDMA_HALFWORD_ENDIANNESS_PRESERVE or @ref LL_MDMA_HALFWORD_ENDIANNESS_EXCHANGE
  *         @arg @ref LL_MDMA_BYTE_ENDIANNESS_PRESERVE or @ref LL_MDMA_BYTE_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigXferEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Configuration)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR,
             MDMA_CCR_WEX | MDMA_CCR_HEX | MDMA_CCR_BEX, Configuration);
}

/**
  * @brief Set Words Endianess.
  * @rmtoll CCR          WEX            LL_MDMA_SetWordEndianness
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
  * @param  Endianness This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_WORD_ENDIANNESS_PRESERVE
  *         @arg @ref LL_MDMA_WORD_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetWordEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Endianness)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_WEX, Endianness);
}

/**
  * @brief Get Words Endianess.
  * @rmtoll CCR          WEX            LL_MDMA_GetWordEndianness
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_WORD_ENDIANNESS_PRESERVE
  *         @arg @ref LL_MDMA_WORD_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetWordEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_WEX));
}

/**
  * @brief Set Half Words Endianess.
  * @rmtoll CCR          HEX            LL_MDMA_SetHalfWordEndianness
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
  * @param  Endianness This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_HALFWORD_ENDIANNESS_PRESERVE
  *         @arg @ref LL_MDMA_HALFWORD_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetHalfWordEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Endianness)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_HEX, Endianness);
}

/**
  * @brief Get Half Words Endianess.
  * @rmtoll CCR          HEX            LL_MDMA_GetHalfWordEndianness
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_HALFWORD_ENDIANNESS_PRESERVE
  *         @arg @ref LL_MDMA_HALFWORD_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetHalfWordEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_HEX));
}

/**
  * @brief Set Bytes Endianess.
  * @rmtoll CCR          BEX            LL_MDMA_SetByteEndianness
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
  * @param  Endianness This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_BYTE_ENDIANNESS_PRESERVE
  *         @arg @ref LL_MDMA_BYTE_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetByteEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Endianness)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_BEX, Endianness);
}

/**
  * @brief Get Bytes Endianess.
  * @rmtoll CCR          BEX            LL_MDMA_GetByteEndianness
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_BYTE_ENDIANNESS_PRESERVE
  *         @arg @ref LL_MDMA_BYTE_ENDIANNESS_EXCHANGE
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetByteEndianness(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_BEX));
}

/**
  * @brief Set Channel priority level.
  * @rmtoll CCR          PL            LL_MDMA_SetChannelPriorityLevel
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
  * @param  Priority This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_PRIORITY_LOW
  *         @arg @ref LL_MDMA_PRIORITY_MEDIUM
  *         @arg @ref LL_MDMA_PRIORITY_HIGH
  *         @arg @ref LL_MDMA_PRIORITY_VERYHIGH
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetChannelPriorityLevel(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Priority)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_PL, Priority);
}

/**
  * @brief Get Channel priority level.
  * @rmtoll CCR          PL            LL_MDMA_GetChannelPriorityLevel
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_PRIORITY_LOW
  *         @arg @ref LL_MDMA_PRIORITY_MEDIUM
  *         @arg @ref LL_MDMA_PRIORITY_HIGH
  *         @arg @ref LL_MDMA_PRIORITY_VERYHIGH
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetChannelPriorityLevel(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR, MDMA_CCR_PL));
}

/**
  * @brief Configure MDMA transfer parameters.
  * @rmtoll CTCR          BWM            LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          SWRM           LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          TRGM           LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          PAM            LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          PKE            LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          TLEN           LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          DBURST         LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          SBURST         LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          DINCOS         LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          SINCOS         LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          DSIZE          LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          SSIZE          LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          DINC           LL_MDMA_ConfigTransfer\n
  * @rmtoll CTCR          SINC           LL_MDMA_ConfigTransfer
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
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_MDMA_BUFF_WRITE_DISABLE or @ref LL_MDMA_BUFF_WRITE_ENABLE
  *         @arg @ref LL_MDMA_REQUEST_MODE_HW or @ref LL_MDMA_REQUEST_MODE_SW
  *         @arg @ref LL_MDMA_BUFFER_TRANSFER or @ref LL_MDMA_BLOCK_TRANSFER or @ref LL_MDMA_REPEAT_BLOCK_TRANSFER or @ref LL_MDMA_FULL_TRANSFER
  *         @arg @ref LL_MDMA_DATAALIGN_RIGHT or @ref LL_MDMA_DATAALIGN_RIGHT_SIGNED or @ref LL_MDMA_DATAALIGN_LEFT
  *         @arg @ref LL_MDMA_PACK_DISABLE or @ref LL_MDMA_PACK_ENABLE
  *         @arg @ref LL_MDMA_DEST_BURST_SINGLE or @ref LL_MDMA_DEST_BURST_2BEATS or @ref LL_MDMA_DEST_BURST_4BEATS or @ref LL_MDMA_DEST_BURST_8BEATS
  *              or @ref LL_MDMA_DEST_BURST_16BEATS or @ref LL_MDMA_DEST_BURST_32BEATS or @ref LL_MDMA_DEST_BURST_64BEATS or @ref LL_MDMA_DEST_BURST_128BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_SINGLE or @ref LL_MDMA_SRC_BURST_2BEATS or @ref LL_MDMA_SRC_BURST_4BEATS or @ref LL_MDMA_SRC_BURST_8BEATS
  *              or @ref LL_MDMA_SRC_BURST_16BEATS or @ref LL_MDMA_SRC_BURST_32BEATS or @ref LL_MDMA_SRC_BURST_64BEATS or @ref LL_MDMA_SRC_BURST_128BEATS
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_BYTE or @ref LL_MDMA_DEST_INC_OFFSET_HALFWORD or @ref LL_MDMA_DEST_INC_OFFSET_WORD or @ref LL_MDMA_DEST_INC_OFFSET_DOUBLEWORD
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_BYTE or @ref LL_MDMA_SRC_INC_OFFSET_HALFWORD or @ref LL_MDMA_SRC_INC_OFFSET_WORD or @ref LL_MDMA_SRC_INC_OFFSET_DOUBLEWORD
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_BYTE or @ref LL_MDMA_DEST_DATA_SIZE_HALFWORD or @ref LL_MDMA_DEST_DATA_SIZE_WORD or @ref LL_MDMA_DEST_DATA_SIZE_DOUBLEWORD
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_BYTE or @ref LL_MDMA_SRC_DATA_SIZE_HALFWORD or @ref LL_MDMA_SRC_DATA_SIZE_WORD or @ref LL_MDMA_SRC_DATA_SIZE_DOUBLEWORD
  *         @arg @ref LL_MDMA_DEST_FIXED or @ref LL_MDMA_DEST_INCREMENT or @ref LL_MDMA_DEST_DECREMENT
  *         @arg @ref LL_MDMA_SRC_FIXED or @ref LL_MDMA_SRC_INCREMENT or @ref LL_MDMA_SRC_DECREMENT
  * @param  BufferXferLength This parameter can be a value Between 0 to 0x0000007F
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigTransfer(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Configuration, uint32_t BufferXferLength)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR,
  Configuration | ((BufferXferLength << MDMA_CTCR_TLEN_Pos) & MDMA_CTCR_TLEN_Msk));
}

/**
  * @brief Enable Bufferable Write Mode.
  * @rmtoll CTCR          BWM            LL_MDMA_EnableBufferableWrMode
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableBufferableWrMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_BWM);
}

/**
  * @brief Disable Bufferable Write Mode.
  * @rmtoll CTCR          BWM            LL_MDMA_DisableBufferableWrMode
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableBufferableWrMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_BWM);
}

/**
  * @brief Check if Bufferable Write Mode is enabled or disabled.
  * @rmtoll CTCR          BWM            LL_MDMA_IsEnabledBufferableWrMode
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledBufferableWrMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_BWM) == (MDMA_CTCR_BWM)) ? 1UL : 0UL);
}

/**
  * @brief Set Request Mode.
  * @rmtoll CTCR          SWRM            LL_MDMA_SetRequestMode
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
  * @param  RequestMode This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_REQUEST_MODE_HW
  *         @arg @ref LL_MDMA_REQUEST_MODE_SW
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetRequestMode(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t RequestMode)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SWRM, RequestMode);
}

/**
  * @brief Get Request Mode.
  * @rmtoll CTCR          SWRM            LL_MDMA_GetRequestMode
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_REQUEST_MODE_HW
  *         @arg @ref LL_MDMA_REQUEST_MODE_SW
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetRequestMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SWRM));
}

/**
  * @brief Set Trigger Mode.
  * @rmtoll CTCR          TRGM            LL_MDMA_SetTriggerMode
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
  * @param  TriggerMode This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_BUFFER_TRANSFER
  *         @arg @ref LL_MDMA_BLOCK_TRANSFER
  *         @arg @ref LL_MDMA_REPEAT_BLOCK_TRANSFER
  *         @arg @ref LL_MDMA_FULL_TRANSFER
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetTriggerMode(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t TriggerMode)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_TRGM, TriggerMode);
}

/**
  * @brief Get Trigger Mode.
  * @rmtoll CTCR          TRGM            LL_MDMA_GetTriggerMode
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_BUFFER_TRANSFER
  *         @arg @ref LL_MDMA_BLOCK_TRANSFER
  *         @arg @ref LL_MDMA_REPEAT_BLOCK_TRANSFER
  *         @arg @ref LL_MDMA_FULL_TRANSFER
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetTriggerMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_TRGM));
}

/**
  * @brief Set Padding Alignment.
  * @rmtoll CTCR          PAM            LL_MDMA_SetPaddingAlignment
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
  * @param  PaddingAlignment This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_DATAALIGN_RIGHT
  *         @arg @ref LL_MDMA_DATAALIGN_RIGHT_SIGNED
  *         @arg @ref LL_MDMA_DATAALIGN_LEFT
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetPaddingAlignment(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t PaddingAlignment)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_PAM, PaddingAlignment);
}

/**
  * @brief Get Padding Alignment.
  * @rmtoll CTCR          PAM            LL_MDMA_GetPaddingAlignment
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_DATAALIGN_RIGHT
  *         @arg @ref LL_MDMA_DATAALIGN_RIGHT_SIGNED
  *         @arg @ref LL_MDMA_DATAALIGN_LEFT
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetPaddingAlignment(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_PAM));
}


/**
  * @brief Enable Packing.
  * @rmtoll CTCR          PKE            LL_MDMA_EnablePacking
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnablePacking(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_PKE);
}

/**
  * @brief Disable Packing.
  * @rmtoll CTCR          PKE            LL_MDMA_DisablePacking
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisablePacking(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_PKE);
}

/**
  * @brief Check if packing is enabled or disabled.
  * @rmtoll CTCR          PKE            LL_MDMA_IsEnabledPacking
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledPacking(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_PKE) == (MDMA_CTCR_PKE)) ? 1UL : 0UL);
}

/**
  * @brief Set Buffer Transfer Length.
  * @rmtoll CTCR          TLEN            LL_MDMA_SetBufferTransferLength
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
  * @param  Length Between 0 to 0x0000007F
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBufferTransferLength(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Length)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_TLEN,
              (Length << MDMA_CTCR_TLEN_Pos) & MDMA_CTCR_TLEN_Msk);
}

/**
  * @brief Get Buffer Transfer Length.
  * @rmtoll CTCR          TLEN            LL_MDMA_GetBufferTransferLength
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
  * @retval Between 0 to 0x0000007F
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBufferTransferLength(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return(READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_TLEN) >> MDMA_CTCR_TLEN_Pos);
}

/**
  * @brief Set Destination burst transfer.
  * @rmtoll CTCR          DBURST            LL_MDMA_SetDestinationBurstSize
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
  * @param  Dburst This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_BURST_SINGLE
  *         @arg @ref LL_MDMA_DEST_BURST_2BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_4BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_8BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_16BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_32BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_64BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_128BEATS
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetDestinationBurstSize(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Dburst)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DBURST, Dburst);
}

/**
  * @brief Get Destination burst transfer.
  * @rmtoll CTCR          DBURST            LL_MDMA_GetDestinationBurstSize
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_BURST_SINGLE
  *         @arg @ref LL_MDMA_DEST_BURST_2BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_4BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_8BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_16BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_32BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_64BEATS
  *         @arg @ref LL_MDMA_DEST_BURST_128BEATS
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetDestinationBurstSize(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return(READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DBURST));
}

/**
  * @brief Set Source burst transfer.
  * @rmtoll CTCR          SBURST            LL_MDMA_SetSourceBurstSize
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
  * @param  Sburst This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_BURST_SINGLE
  *         @arg @ref LL_MDMA_SRC_BURST_2BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_4BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_8BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_16BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_32BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_64BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_128BEATS
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetSourceBurstSize(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Sburst)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SBURST, Sburst);
}

/**
  * @brief Get Source burst transfer.
  * @rmtoll CTCR          SBURST            LL_MDMA_GetSourceBurstSize
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_BURST_SINGLE
  *         @arg @ref LL_MDMA_SRC_BURST_2BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_4BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_8BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_16BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_32BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_64BEATS
  *         @arg @ref LL_MDMA_SRC_BURST_128BEATS
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetSourceBurstSize(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return(READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SBURST));
}

/**
  * @brief Set Destination Increment Offset Size.
  * @rmtoll CTCR          DINCOS            LL_MDMA_SetDestinationIncSize
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
  * @param  IncSize This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_BYTE
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_HALFWORD
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_WORD
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetDestinationIncSize(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t IncSize)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DINCOS, IncSize);
}

/**
  * @brief Get Destination Increment Offset Size.
  * @rmtoll CTCR          DINCOS            LL_MDMA_GetDestinationIncSize
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_BYTE
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_HALFWORD
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_WORD
  *         @arg @ref LL_MDMA_DEST_INC_OFFSET_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetDestinationIncSize(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DINCOS));
}

/**
  * @brief Set Source Increment Offset Size.
  * @rmtoll CTCR          SINCOS            LL_MDMA_SetSourceIncSize
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
  * @param  IncSize This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_BYTE
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_HALFWORD
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_WORD
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetSourceIncSize(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t IncSize)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SINCOS, IncSize);
}

/**
  * @brief Get Source Increment Offset Size.
  * @rmtoll CTCR          SINCOS            LL_MDMA_GetSourceIncSize
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_BYTE
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_HALFWORD
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_WORD
  *         @arg @ref LL_MDMA_SRC_INC_OFFSET_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetSourceIncSize(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SINCOS));
}

/**
  * @brief Set Destination Data Size.
  * @rmtoll CTCR          DSIZE            LL_MDMA_SetDestinationDataSize
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
  * @param  DestDataSize This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_BYTE
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_HALFWORD
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_WORD
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetDestinationDataSize(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t DestDataSize)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DSIZE, DestDataSize);
}

/**
  * @brief Get Destination Data Size.
  * @rmtoll CTCR          DSIZE            LL_MDMA_GetDestinationDataSize
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_BYTE
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_HALFWORD
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_WORD
  *         @arg @ref LL_MDMA_DEST_DATA_SIZE_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetDestinationDataSize(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DSIZE));
}

/**
  * @brief Set Source Data Size.
  * @rmtoll CTCR          SSIZE            LL_MDMA_SetSourceDataSize
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
  * @param  SrcDataSize This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_BYTE
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_HALFWORD
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_WORD
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetSourceDataSize(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t SrcDataSize)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SSIZE, SrcDataSize);
}

/**
  * @brief Get Source Data Size.
  * @rmtoll CTCR          SSIZE            LL_MDMA_GetSourceDataSize
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_BYTE
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_HALFWORD
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_WORD
  *         @arg @ref LL_MDMA_SRC_DATA_SIZE_DOUBLEWORD
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetSourceDataSize(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SSIZE));
}

/**
  * @brief Set Destination Increment Mode.
  * @rmtoll CTCR          DINC            LL_MDMA_SetDestinationIncMode
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
  * @param  DestIncMode This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_FIXED
  *         @arg @ref LL_MDMA_DEST_INCREMENT
  *         @arg @ref LL_MDMA_DEST_DECREMENT
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetDestinationIncMode(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t DestIncMode)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DINC, DestIncMode);
}

/**
  * @brief Get Destination Increment Mode.
  * @rmtoll CTCR          DINC            LL_MDMA_GetDestinationIncMode
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_FIXED
  *         @arg @ref LL_MDMA_DEST_INCREMENT
  *         @arg @ref LL_MDMA_DEST_DECREMENT
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetDestinationIncMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_DINC));
}

/**
  * @brief Set Source Increment Mode.
  * @rmtoll CTCR          SINC            LL_MDMA_SetSourceIncMode
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
  * @param  SrcIncMode This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_FIXED
  *         @arg @ref LL_MDMA_SRC_INCREMENT
  *         @arg @ref LL_MDMA_SRC_DECREMENT
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetSourceIncMode(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t SrcIncMode)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SINC, SrcIncMode);
}

/**
  * @brief Get Source Increment Mode.
  * @rmtoll CTCR          SINC            LL_MDMA_GetSourceIncMode
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_FIXED
  *         @arg @ref LL_MDMA_SRC_INCREMENT
  *         @arg @ref LL_MDMA_SRC_DECREMENT
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetSourceIncMode(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTCR, MDMA_CTCR_SINC));
}

/**
  * @brief Configure MDMA Block number of data and repeat Count.
  * @rmtoll CBNDTR        BRC            LL_MDMA_ConfigBlkCounters\n
  * @rmtoll CBNDTR        BNDT           LL_MDMA_ConfigBlkCounters
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
  * @param  BlockRepeatCount Between 0 to 0x00000FFF
  * @param  BlkDataLength Between 0 to 0x00010000
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigBlkCounters(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t BlockRepeatCount, uint32_t BlkDataLength)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR,
               MDMA_CBNDTR_BRC | MDMA_CBNDTR_BNDT,
              ((BlockRepeatCount << MDMA_CBNDTR_BRC_Pos) & MDMA_CBNDTR_BRC_Msk) | (BlkDataLength & MDMA_CBNDTR_BNDT_Msk));
}

/**
  * @brief Set Block Number of data bytes to transfer.
  * @rmtoll CBNDTR          BNDT            LL_MDMA_SetBlkDataLength
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
  * @param  BlkDataLength Between 0 to 0x00010000
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBlkDataLength(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t BlkDataLength)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BNDT, (BlkDataLength & MDMA_CBNDTR_BNDT_Msk));
}

/**
  * @brief Get Block Number of data bytes to transfer.
  * @rmtoll CBNDTR          BNDT            LL_MDMA_GetBlkDataLength
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
  * @retval Between 0 to 0x00010000
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBlkDataLength(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BNDT));
}

/**
  * @brief Set Block Repeat Count.
  * @rmtoll CBNDTR          BRC            LL_MDMA_SetBlkRepeatCount
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
  * @param  BlockRepeatCount Between 0 to 0x00000FFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBlkRepeatCount(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t BlockRepeatCount)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BRC,
              (BlockRepeatCount << MDMA_CBNDTR_BRC_Pos) & MDMA_CBNDTR_BRC_Msk);
}

/**
  * @brief Get Block Repeat Count.
  * @rmtoll CBNDTR          BRC            LL_MDMA_GetBlkRepeatCount
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
  * @retval Between 0 to 0x00000FFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBlkRepeatCount(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BRC) >> MDMA_CBNDTR_BRC_Pos);
}

/**
  * @brief Configure MDMA block repeat address update mode.
  * @rmtoll CBNDTR        BRDUM            LL_MDMA_ConfigBlkRepeatAddrUpdate\n
  * @rmtoll CBNDTR        BRSUM            LL_MDMA_ConfigBlkRepeatAddrUpdate
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
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_MDMA_BLK_RPT_DEST_ADDR_INCREMENT or @ref LL_MDMA_BLK_RPT_DEST_ADDR_DECREMENT
  *         @arg @ref LL_MDMA_BLK_RPT_SRC_ADDR_INCREMENT or @ref LL_MDMA_BLK_RPT_SRC_ADDR_DECREMENT
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigBlkRepeatAddrUpdate(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Configuration)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR,
               MDMA_CBNDTR_BRDUM | MDMA_CBNDTR_BRSUM,
               Configuration);
}

/**
  * @brief Set Block Repeat Destination address Update Mode.
  * @rmtoll CBNDTR          BRDUM            LL_MDMA_SetBlkRepeatDestAddrUpdate
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
  * @param  DestAdrUpdateMode This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_BLK_RPT_DEST_ADDR_INCREMENT
  *         @arg @ref LL_MDMA_BLK_RPT_DEST_ADDR_DECREMENT
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBlkRepeatDestAddrUpdate(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t DestAdrUpdateMode)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BRDUM, DestAdrUpdateMode);
}

/**
  * @brief Get Block Repeat Destination address Update Mode.
  * @rmtoll CBNDTR          BRDUM            LL_MDMA_GetBlkRepeatDestAddrUpdate
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_BLK_RPT_DEST_ADDR_INCREMENT
  *         @arg @ref LL_MDMA_BLK_RPT_DEST_ADDR_DECREMENT
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBlkRepeatDestAddrUpdate(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BRDUM));
}

/**
  * @brief Set Block Repeat Source address Update Mode.
  * @rmtoll CBNDTR          BRSUM            LL_MDMA_SetBlkRepeatSrcAddrUpdate
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
  * @param  SrcAdrUpdateMode This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_BLK_RPT_SRC_ADDR_INCREMENT
  *         @arg @ref LL_MDMA_BLK_RPT_SRC_ADDR_DECREMENT
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBlkRepeatSrcAddrUpdate(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t SrcAdrUpdateMode)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BRSUM, SrcAdrUpdateMode);
}

/**
  * @brief Get Block Repeat Source address Update Mode.
  * @rmtoll CBNDTR          BRSUM            LL_MDMA_GetBlkRepeatSrcAddrUpdate
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_BLK_RPT_SRC_ADDR_INCREMENT
  *         @arg @ref LL_MDMA_BLK_RPT_SRC_ADDR_DECREMENT
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBlkRepeatSrcAddrUpdate(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBNDTR, MDMA_CBNDTR_BRSUM));
}

/**
  * @brief  Configure the Source and Destination addresses.
  * @note   This API must not be called when the MDMA channel is enabled.
  * @rmtoll CSAR        SAR            LL_MDMA_ConfigAddresses\n
  * @rmtoll CDAR        DAR            LL_MDMA_ConfigAddresses
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
  * @param  SrcAddress Between Min_Data = 0 and Max_Data = 0xFFFFFFFF
  * @param  DstAddress Between Min_Data = 0 and Max_Data = 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigAddresses(MDMA_TypeDef *MDMAx, uint32_t Channel,  uint32_t SrcAddress, uint32_t DstAddress)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CSAR, SrcAddress);
  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CDAR, DstAddress);
}
/**
  * @brief Set transfer Source address.
  * @rmtoll CSAR          SAR            LL_MDMA_SetSourceAddress
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
  * @param  SrcAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetSourceAddress(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t SrcAddress)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CSAR, SrcAddress);
}

/**
  * @brief Get transfer Source address.
  * @rmtoll CSAR          SAR            LL_MDMA_GetSourceAddress
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
  * @retval Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetSourceAddress(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CSAR));
}

/**
  * @brief Set transfer Destination address.
  * @rmtoll CDAR          DAR            LL_MDMA_SetDestinationAddress
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
  * @param  DestAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetDestinationAddress(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t DestAddress)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CDAR, DestAddress);
}

/**
  * @brief Get transfer Destination address.
  * @rmtoll CDAR          DAR            LL_MDMA_GetDestinationAddress
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
  * @retval Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetDestinationAddress(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CDAR));
}

/**
  * @brief  Configure the Source and Destination Block repeat addresses Update value.
  * @note   This API must not be called when the MDMA channel is enabled.
  * @rmtoll CBRUR        DUV            LL_MDMA_ConfigBlkRptAddrUpdateValue\n
  * @rmtoll CBRUR        SUV            LL_MDMA_ConfigBlkRptAddrUpdateValue
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
  * @param  SrctAdrUpdateValue Min_Data = 0 and Max_Data = 0x0000FFFF
  * @param  DestAdrUpdateValue Between Min_Data = 0 and Max_Data = 0x0000FFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigBlkRptAddrUpdateValue(MDMA_TypeDef *MDMAx, uint32_t Channel,  uint32_t SrctAdrUpdateValue, uint32_t DestAdrUpdateValue)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBRUR,
              (SrctAdrUpdateValue & MDMA_CBRUR_SUV_Msk)  | ((DestAdrUpdateValue << MDMA_CBRUR_DUV_Pos) & MDMA_CBRUR_DUV_Msk));
}

/**
  * @brief Set transfer Destination address Update Value.
  * @rmtoll CBRUR          DUV            LL_MDMA_SetBlkRptDestAddrUpdateValue
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
  * @param  DestAdrUpdateValue Between 0 to 0x0000FFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBlkRptDestAddrUpdateValue(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t DestAdrUpdateValue)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBRUR, MDMA_CBRUR_DUV,
               ((DestAdrUpdateValue << MDMA_CBRUR_DUV_Pos) & MDMA_CBRUR_DUV_Msk));
}

/**
  * @brief Get transfer Destination address Update Value.
  * @rmtoll CBRUR          DUV            LL_MDMA_GetBlkRptDestAddrUpdateValue
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
  * @retval Between 0 to 0x0000FFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBlkRptDestAddrUpdateValue(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBRUR, MDMA_CBRUR_DUV) >> MDMA_CBRUR_DUV_Pos);
}

/**
  * @brief Set transfer Source address Update Value.
  * @rmtoll CBRUR          SUV            LL_MDMA_SetBlkRptSrcAddrUpdateValue
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
  * @param  SrcAdrUpdateValue Between 0 to 0x0000FFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetBlkRptSrcAddrUpdateValue(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t SrcAdrUpdateValue)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBRUR, MDMA_CBRUR_SUV, SrcAdrUpdateValue);
}

/**
  * @brief Get transfer Source address Update Value.
  * @rmtoll CBRUR          SUV            LL_MDMA_GetBlkRptSrcAddrUpdateValue
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
  * @retval Between 0 to 0x0000FFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetBlkRptSrcAddrUpdateValue(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CBRUR, MDMA_CBRUR_SUV));
}

/**
  * @brief Set transfer Link Address.
  * @rmtoll CLAR          LAR            LL_MDMA_SetLinkAddress
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
  * @param  LinkAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetLinkAddress(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t LinkAddress)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CLAR, LinkAddress);
}

/**
  * @brief Get transfer Link Address.
  * @rmtoll CLAR          LAR            LL_MDMA_GetLinkAddress
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
  * @retval Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetLinkAddress(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CLAR));
}

/**
  * @brief Configure MDMA source and destination bus selection.
  * @rmtoll CTBR        DBUS            LL_MDMA_ConfigBusSelection\n
  * @rmtoll CTBR        SBUS            LL_MDMA_ConfigBusSelection
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
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_MDMA_DEST_BUS_SYSTEM_AXI or @ref LL_MDMA_DEST_BUS_AHB_TCM
  *         @arg @ref LL_MDMA_SRC_BUS_SYSTEM_AXI or @ref LL_MDMA_SRC_BUS_AHB_TCM
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ConfigBusSelection(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t Configuration)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR,
               MDMA_CTBR_DBUS | MDMA_CTBR_SBUS,
              Configuration);
}

/**
  * @brief Set Destination Bus Selection.
  * @rmtoll CTBR          DBUS            LL_MDMA_SetDestBusSelection
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
  * @param  DestBus This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_BUS_SYSTEM_AXI
  *         @arg @ref LL_MDMA_DEST_BUS_AHB_TCM
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetDestBusSelection(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t DestBus)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR, MDMA_CTBR_DBUS, DestBus);
}

/**
  * @brief Get Destination Bus Selection.
  * @rmtoll CTBR          DBUS            LL_MDMA_GetDestBusSelection
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_DEST_BUS_SYSTEM_AXI
  *         @arg @ref LL_MDMA_DEST_BUS_AHB_TCM
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetDestBusSelection(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR, MDMA_CTBR_DBUS));
}

/**
  * @brief Set Source Bus Selection.
  * @rmtoll CTBR          SBUS            LL_MDMA_SetSrcBusSelection
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
  * @param  SrcBus This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_BUS_SYSTEM_AXI
  *         @arg @ref LL_MDMA_SRC_BUS_AHB_TCM
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetSrcBusSelection(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t SrcBus)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR, MDMA_CTBR_SBUS, SrcBus);
}

/**
  * @brief Get Source Bus Selection.
  * @rmtoll CTBR          SBUS            LL_MDMA_GetSrcBusSelection
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_SRC_BUS_SYSTEM_AXI
  *         @arg @ref LL_MDMA_SRC_BUS_AHB_TCM
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetSrcBusSelection(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR, MDMA_CTBR_SBUS));
}

/**
  * @brief Set Transfer hardware trigger (Request).
  * @rmtoll CTBR          TSEL            LL_MDMA_SetHWTrigger
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
  * @param  HWRequest This parameter can be one of the following values:
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM0_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM1_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM2_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM3_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM4_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM5_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM6_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM7_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM0_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM1_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM2_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM3_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM4_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM5_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM6_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM7_TC
  *         @arg @ref LL_MDMA_REQ_LTDC_LINE_IT (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_INFIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_INFIFO_NF (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_OUTFIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_OUTFIFO_NE (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_END_CONVERSION (*)
  *         @arg @ref LL_MDMA_REQ_QUADSPI_FIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_QUADSPI_TC (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI1_FIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI1_TC (*)
  *         @arg @ref LL_MDMA_REQ_DMA2D_CLUT_TC
  *         @arg @ref LL_MDMA_REQ_DMA2D_TC
  *         @arg @ref LL_MDMA_REQ_DMA2D_TW
  *         @arg @ref LL_MDMA_REQ_DSI_TEARING_EFFECT (*)
  *         @arg @ref LL_MDMA_REQ_DSI_END_REFRESH (*)
  *         @arg @ref LL_MDMA_REQ_SDMMC1_END_DATA
  *         @arg @ref LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER (*)
  *         @arg @ref LL_MDMA_REQ_SDMMC1_COMMAND_END (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI2_FIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI2_TC (*)
  * @note   (*) Availability depends on devices.
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetHWTrigger(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t HWRequest)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  MODIFY_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR, MDMA_CTBR_TSEL, HWRequest);
}

/**
  * @brief Get Transfer hardware trigger (Request).
  * @rmtoll CTBR          TSEL            LL_MDMA_GetHWTrigger
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM0_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM1_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM2_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM3_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM4_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM5_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM6_TC
  *         @arg @ref LL_MDMA_REQ_DMA1_STREAM7_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM0_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM1_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM2_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM3_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM4_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM5_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM6_TC
  *         @arg @ref LL_MDMA_REQ_DMA2_STREAM7_TC
  *         @arg @ref LL_MDMA_REQ_LTDC_LINE_IT (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_INFIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_INFIFO_NF (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_OUTFIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_OUTFIFO_NE (*)
  *         @arg @ref LL_MDMA_REQ_JPEG_END_CONVERSION (*)
  *         @arg @ref LL_MDMA_REQ_QUADSPI_FIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_QUADSPI_TC (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI1_FIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI1_TC (*)
  *         @arg @ref LL_MDMA_REQ_DMA2D_CLUT_TC
  *         @arg @ref LL_MDMA_REQ_DMA2D_TC
  *         @arg @ref LL_MDMA_REQ_DMA2D_TW
  *         @arg @ref LL_MDMA_REQ_DSI_TEARING_EFFECT (*)
  *         @arg @ref LL_MDMA_REQ_DSI_END_REFRESH (*)
  *         @arg @ref LL_MDMA_REQ_SDMMC1_END_DATA
  *         @arg @ref LL_MDMA_REQ_SDMMC1_DMA_ENDBUFFER (*)
  *         @arg @ref LL_MDMA_REQ_SDMMC1_COMMAND_END (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI2_FIFO_TH (*)
  *         @arg @ref LL_MDMA_REQ_OCTOSPI2_TC (*)
  * @note   (*) Availability depends on devices.
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetHWTrigger(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CTBR, MDMA_CTBR_TSEL));
}

/**
  * @brief Set Mask Address.
  * @rmtoll CMAR          MAR            LL_MDMA_SetMaskAddress
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
  * @param  MaskAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetMaskAddress(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t MaskAddress)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CMAR, MaskAddress);
}

/**
  * @brief Get Mask Address.
  * @rmtoll CMAR          MAR            LL_MDMA_GetMaskAddress
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
  * @retval Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetMaskAddress(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CMAR));
}

/**
  * @brief Set Mask Data.
  * @rmtoll CMDR          MDR            LL_MDMA_SetMaskData
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
  * @param  MaskData Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_SetMaskData(MDMA_TypeDef *MDMAx, uint32_t Channel, uint32_t MaskData)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CMDR, MaskData);
}

/**
  * @brief Get Mask Data.
  * @rmtoll CMDR          MDR            LL_MDMA_GetMaskData
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
  * @retval Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetMaskData(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_REG(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CMDR));
}

/**
  * @brief Get Transfer Error Direction.
  * @rmtoll CESR          TED            LL_MDMA_GetXferErrorDirection
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
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_MDMA_READ_ERROR
  *         @arg @ref LL_MDMA_WRITE_ERROR
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetXferErrorDirection(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CESR, MDMA_CESR_TED));
}

/**
  * @brief Get Transfer Error LSB Address.
  * @rmtoll CESR          TEA            LL_MDMA_GetXferErrorLSBAddress
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
  * @retval Between 0 to 0x0000007F
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MDMA_GetXferErrorLSBAddress(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return (READ_BIT(((MDMA_Channel_TypeDef *)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CESR, MDMA_CESR_TEA));
}

/**
  * @}
  */

/** @defgroup MDMA_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief Get MDMA Channel x Global Interrupt flag.
  * @rmtoll GISR0  GIFx    LL_MDMA_IsActiveFlag_GI
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_GI(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  return ((READ_BIT(MDMAx->GISR0 ,(MDMA_GISR0_GIF0 << (Channel & 0x0000000FU)))==(MDMA_GISR0_GIF0 << (Channel & 0x0000000FU))) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Transfer Error interrupt flag.
  * @rmtoll CISR  TEIF    LL_MDMA_IsActiveFlag_TE
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_TE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CISR, MDMA_CISR_TEIF) == (MDMA_CISR_TEIF)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Channel Transfer Complete interrupt flag.
  * @rmtoll CISR  CTCIF    LL_MDMA_IsActiveFlag_CTC
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_CTC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CISR, MDMA_CISR_CTCIF) == (MDMA_CISR_CTCIF)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Block Repeat Transfer complete interrupt flag.
  * @rmtoll CISR  BRTIF    LL_MDMA_IsActiveFlag_BRT
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_BRT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CISR, MDMA_CISR_BRTIF) == (MDMA_CISR_BRTIF)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Block Transfer complete interrupt flag.
  * @rmtoll CISR  BTIF    LL_MDMA_IsActiveFlag_BT
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_BT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CISR, MDMA_CISR_BTIF) == (MDMA_CISR_BTIF)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x buffer transfer complete interrupt flag.
  * @rmtoll CISR  TCIF    LL_MDMA_IsActiveFlag_TC
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_TC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CISR, MDMA_CISR_TCIF) == (MDMA_CISR_TCIF)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x ReQuest Active flag.
  * @rmtoll CISR  CRQA    LL_MDMA_IsActiveFlag_CRQA
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_CRQA(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CISR, MDMA_CISR_CRQA) == (MDMA_CISR_CRQA)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Block Size Error flag.
  * @rmtoll CESR  BSE    LL_MDMA_IsActiveFlag_BSE
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_BSE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CESR, MDMA_CESR_BSE) == (MDMA_CESR_BSE)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Address/Size Error flag.
  * @rmtoll CESR  ASE    LL_MDMA_IsActiveFlag_ASE
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_ASE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CESR, MDMA_CESR_ASE) == (MDMA_CESR_ASE)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Transfer Error Mask Data flag.
  * @rmtoll CESR  TEMD    LL_MDMA_IsActiveFlag_TEMD
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_TEMD(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CESR, MDMA_CESR_TEMD) == (MDMA_CESR_TEMD)) ? 1UL : 0UL);
}

/**
  * @brief Get MDMA Channel x Transfer Error Link Data flag.
  * @rmtoll CESR  TELD    LL_MDMA_IsActiveFlag_TELD
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsActiveFlag_TELD(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CESR, MDMA_CESR_TELD) == (MDMA_CESR_TELD)) ? 1UL : 0UL);
}

/**
  * @brief Clear MDMA Channel x Transfer Error interrupt flag.
  * @rmtoll CIFCR  CTEIF    LL_MDMA_ClearFlag_TE
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ClearFlag_TE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CIFCR ,MDMA_CIFCR_CTEIF);
}

/**
  * @brief Clear MDMA Channel x Channel Transfer Complete interrupt flag.
  * @rmtoll CIFCR  CCTCIF    LL_MDMA_ClearFlag_CTC
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ClearFlag_CTC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CIFCR ,MDMA_CIFCR_CCTCIF);
}

/**
  * @brief Clear MDMA Channel x Block Repeat Transfer complete interrupt flag.
  * @rmtoll CIFCR  CBRTIF    LL_MDMA_ClearFlag_BRT
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ClearFlag_BRT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CIFCR ,MDMA_CIFCR_CBRTIF);
}

/**
  * @brief Clear MDMA Channel x Block Transfer complete interrupt flag.
  * @rmtoll CIFCR  CBTIF    LL_MDMA_ClearFlag_BT
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ClearFlag_BT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CIFCR ,MDMA_CIFCR_CBTIF);
}

/**
  * @brief Clear MDMA Channel x buffer transfer Complete Interrupt Flag.
  * @rmtoll CIFCR  CLTCIF    LL_MDMA_ClearFlag_TC
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_ClearFlag_TC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  WRITE_REG(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CIFCR ,MDMA_CIFCR_CLTCIF);
}

/**
  * @}
  */

/** @defgroup MDMA_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief Enable MDMA Channel x Transfer Error interrupt.
  * @rmtoll CCR  TEIE    LL_MDMA_EnableIT_TE
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableIT_TE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_TEIE);
}

/**
  * @brief Enable MDMA Channel x Channel Transfer Complete interrupt.
  * @rmtoll CCR  CTCIE    LL_MDMA_EnableIT_CTC
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableIT_CTC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_CTCIE);
}

/**
  * @brief Enable MDMA Channel x Block Repeat Transfer interrupt.
  * @rmtoll CCR  BRTIE    LL_MDMA_EnableIT_BRT
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableIT_BRT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_BRTIE);
}

/**
  * @brief Enable MDMA Channel x Block Transfer interrupt.
  * @rmtoll CCR  BTIE    LL_MDMA_EnableIT_BT
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableIT_BT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_BTIE);
}

/**
  * @brief Enable MDMA Channel x buffer transfer complete interrupt.
  * @rmtoll CCR  TCIE    LL_MDMA_EnableIT_TC
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_EnableIT_TC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  SET_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_TCIE);
}

/**
  * @brief Disable MDMA Channel x Transfer Error interrupt.
  * @rmtoll CCR  TEIE    LL_MDMA_DisableIT_TE
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableIT_TE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_TEIE);
}

/**
  * @brief Disable MDMA Channel x Channel Transfer Complete interrupt.
  * @rmtoll CCR  CTCIE    LL_MDMA_DisableIT_CTC
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableIT_CTC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_CTCIE);
}

/**
  * @brief Disable MDMA Channel x Block Repeat Transfer interrupt.
  * @rmtoll CCR  BRTIE    LL_MDMA_DisableIT_BRT
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableIT_BRT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_BRTIE);
}

/**
  * @brief Disable MDMA Channel x Block Transfer interrupt.
  * @rmtoll CCR  BTIE    LL_MDMA_DisableIT_BT
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableIT_BT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_BTIE);
}

/**
  * @brief Disable MDMA Channel x buffer transfer complete interrupt.
  * @rmtoll CCR  TCIE    LL_MDMA_DisableIT_TC
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
  * @retval None
  */
__STATIC_INLINE void LL_MDMA_DisableIT_TC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  CLEAR_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_TCIE);
}

/**
  * @brief Check if MDMA Channel x Transfer Error interrupt is enabled.
  * @rmtoll CCR  TEIE    LL_MDMA_IsEnabledIT_TE
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledIT_TE(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_TEIE) == MDMA_CCR_TEIE) ? 1UL : 0UL);
}

/**
  * @brief Check if MDMA Channel x Channel Transfer Complete interrupt is enabled.
  * @rmtoll CCR  CTCIE    LL_MDMA_IsEnabledIT_CTC
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledIT_CTC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_CTCIE) == MDMA_CCR_CTCIE) ? 1UL : 0UL);
}

/**
  * @brief Check if MDMA Channel x Block Repeat Transfer complete interrupt is enabled.
  * @rmtoll CCR  BRTIE    LL_MDMA_IsEnabledIT_BRT
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledIT_BRT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_BRTIE) == MDMA_CCR_BRTIE) ? 1UL : 0UL);
}

/**
  * @brief Check if MDMA Channel x Block Transfer interrupt is enabled.
  * @rmtoll CCR  BTIE    LL_MDMA_IsEnabledIT_BT
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledIT_BT(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_BTIE) == MDMA_CCR_BTIE) ? 1UL : 0UL);
}

/**
  * @brief Check if MDMA Channel x buffer transfer complete interrupt is enabled.
  * @rmtoll CCR  TCIE    LL_MDMA_IsEnabledIT_TC
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
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MDMA_IsEnabledIT_TC(MDMA_TypeDef *MDMAx, uint32_t Channel)
{
  register uint32_t mdma_base_addr = (uint32_t)MDMAx;

  return ((READ_BIT(((MDMA_Channel_TypeDef*)(mdma_base_addr + LL_MDMA_CH_OFFSET_TAB[Channel]))->CCR ,MDMA_CCR_TCIE) == MDMA_CCR_TCIE) ? 1UL : 0UL);
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup MDMA_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

uint32_t LL_MDMA_Init(MDMA_TypeDef *MDMAx, uint32_t Channel, LL_MDMA_InitTypeDef *MDMA_InitStruct);
uint32_t LL_MDMA_DeInit(MDMA_TypeDef *MDMAx, uint32_t Channel);
void LL_MDMA_StructInit(LL_MDMA_InitTypeDef *MDMA_InitStruct);
void LL_MDMA_CreateLinkNode(LL_MDMA_InitTypeDef *MDMA_InitStruct, LL_MDMA_LinkNodeTypeDef *pNode);
void LL_MDMA_ConnectLinkNode(LL_MDMA_LinkNodeTypeDef *pPrevLinkNode, LL_MDMA_LinkNodeTypeDef *pNewLinkNode);
void LL_MDMA_DisconnectNextLinkNode(LL_MDMA_LinkNodeTypeDef *pLinkNode);

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

#endif /* MDMA */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_LL_MDMA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
