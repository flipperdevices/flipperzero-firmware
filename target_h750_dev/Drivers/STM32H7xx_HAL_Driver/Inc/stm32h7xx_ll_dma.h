/**
  ******************************************************************************
  * @file    stm32h7xx_ll_dma.h
  * @author  MCD Application Team
  * @brief   Header file of DMA LL module.
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
#ifndef STM32H7xx_LL_DMA_H
#define STM32H7xx_LL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"
#include "stm32h7xx_ll_dmamux.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (DMA1) || defined (DMA2)

/** @defgroup DMA_LL DMA
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup DMA_LL_Private_Variables DMA Private Variables
  * @{
  */
/* Array used to get the DMA stream register offset versus stream index LL_DMA_STREAM_x */
static const uint8_t LL_DMA_STR_OFFSET_TAB[] =
{
  (uint8_t)(DMA1_Stream0_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream1_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream2_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream3_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream4_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream5_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream6_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Stream7_BASE - DMA1_BASE)
};


/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/**
  * @brief  Helper macro to convert DMA Instance DMAx into DMAMUX channel
  * @note   DMAMUX channel 0 to 7 are mapped to DMA1 stream 0 to 7.
  *         DMAMUX channel 8 to 15 are mapped to DMA2 stream 0 to 7.
  * @param  __DMA_INSTANCE__ DMAx
  * @retval Channel_Offset (LL_DMAMUX_CHANNEL_8 or 0).
  */
#define LL_DMA_INSTANCE_TO_DMAMUX_CHANNEL(__DMA_INSTANCE__)   \
(((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) ? 0UL : 8UL)

/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup DMA_LL_ES_INIT DMA Exported Init structure
  * @{
  */
typedef struct
{
  uint32_t PeriphOrM2MSrcAddress;  /*!< Specifies the peripheral base address for DMA transfer
                                        or as Source base address in case of memory to memory transfer direction.

                                        This parameter must be a value between Min_Data = 0 and Max_Data = 0xFFFFFFFF. */

  uint32_t MemoryOrM2MDstAddress;  /*!< Specifies the memory base address for DMA transfer
                                        or as Destination base address in case of memory to memory transfer direction.

                                        This parameter must be a value between Min_Data = 0 and Max_Data = 0xFFFFFFFF. */

  uint32_t Direction;              /*!< Specifies if the data will be transferred from memory to peripheral,
                                        from memory to memory or from peripheral to memory.
                                        This parameter can be a value of @ref DMA_LL_EC_DIRECTION

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetDataTransferDirection(). */

  uint32_t Mode;                   /*!< Specifies the normal or circular operation mode.
                                        This parameter can be a value of @ref DMA_LL_EC_MODE
                                        @note The circular buffer mode cannot be used if the memory to memory
                                              data transfer direction is configured on the selected Stream

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetMode(). */

  uint32_t PeriphOrM2MSrcIncMode;  /*!< Specifies whether the Peripheral address or Source address in case of memory to memory transfer direction
                                        is incremented or not.
                                        This parameter can be a value of @ref DMA_LL_EC_PERIPH

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetPeriphIncMode(). */

  uint32_t MemoryOrM2MDstIncMode;  /*!< Specifies whether the Memory address or Destination address in case of memory to memory transfer direction
                                        is incremented or not.
                                        This parameter can be a value of @ref DMA_LL_EC_MEMORY

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetMemoryIncMode(). */

  uint32_t PeriphOrM2MSrcDataSize; /*!< Specifies the Peripheral data size alignment or Source data size alignment (byte, half word, word)
                                        in case of memory to memory transfer direction.
                                        This parameter can be a value of @ref DMA_LL_EC_PDATAALIGN

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetPeriphSize(). */

  uint32_t MemoryOrM2MDstDataSize; /*!< Specifies the Memory data size alignment or Destination data size alignment (byte, half word, word)
                                        in case of memory to memory transfer direction.
                                        This parameter can be a value of @ref DMA_LL_EC_MDATAALIGN

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetMemorySize(). */

  uint32_t NbData;                 /*!< Specifies the number of data to transfer, in data unit.
                                        The data unit is equal to the source buffer configuration set in PeripheralSize
                                        or MemorySize parameters depending in the transfer direction.
                                        This parameter must be a value between Min_Data = 0 and Max_Data = 0x0000FFFF

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetDataLength(). */

  uint32_t PeriphRequest;          /*!< Specifies the peripheral request.
                                        This parameter can be a value of @ref DMAMUX_LL_EC_REQUEST

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetPeriphRequest(). */

  uint32_t Priority;               /*!< Specifies the channel priority level.
                                        This parameter can be a value of @ref DMA_LL_EC_PRIORITY

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetStreamPriorityLevel(). */

  uint32_t FIFOMode;               /*!< Specifies if the FIFO mode or Direct mode will be used for the specified stream.
                                        This parameter can be a value of @ref DMA_LL_FIFOMODE
                                        @note The Direct mode (FIFO mode disabled) cannot be used if the
                                        memory-to-memory data transfer is configured on the selected stream

                                        This feature can be modified afterwards using unitary functions @ref LL_DMA_EnableFifoMode() or @ref LL_DMA_EnableFifoMode() . */

  uint32_t FIFOThreshold;          /*!< Specifies the FIFO threshold level.
                                        This parameter can be a value of @ref DMA_LL_EC_FIFOTHRESHOLD

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetFIFOThreshold(). */

  uint32_t MemBurst;               /*!< Specifies the Burst transfer configuration for the memory transfers.
                                        It specifies the amount of data to be transferred in a single non interruptible
                                        transaction.
                                        This parameter can be a value of @ref DMA_LL_EC_MBURST
                                        @note The burst mode is possible only if the address Increment mode is enabled.

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetMemoryBurstxfer(). */

  uint32_t PeriphBurst;            /*!< Specifies the Burst transfer configuration for the peripheral transfers.
                                        It specifies the amount of data to be transferred in a single non interruptible
                                        transaction.
                                        This parameter can be a value of @ref DMA_LL_EC_PBURST
                                        @note The burst mode is possible only if the address Increment mode is enabled.

                                        This feature can be modified afterwards using unitary function @ref LL_DMA_SetPeriphBurstxfer(). */

} LL_DMA_InitTypeDef;
/**
  * @}
  */
#endif /*USE_FULL_LL_DRIVER*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup DMA_LL_Exported_Constants DMA Exported Constants
  * @{
  */

/** @defgroup DMA_LL_EC_STREAM STREAM
  * @{
  */
#define LL_DMA_STREAM_0                   0x00000000U
#define LL_DMA_STREAM_1                   0x00000001U
#define LL_DMA_STREAM_2                   0x00000002U
#define LL_DMA_STREAM_3                   0x00000003U
#define LL_DMA_STREAM_4                   0x00000004U
#define LL_DMA_STREAM_5                   0x00000005U
#define LL_DMA_STREAM_6                   0x00000006U
#define LL_DMA_STREAM_7                   0x00000007U
#define LL_DMA_STREAM_ALL                 0xFFFF0000U
/**
  * @}
  */


/** @defgroup DMA_LL_EC_DIRECTION DIRECTION
  * @{
  */
#define LL_DMA_DIRECTION_PERIPH_TO_MEMORY 0x00000000U               /*!< Peripheral to memory direction */
#define LL_DMA_DIRECTION_MEMORY_TO_PERIPH DMA_SxCR_DIR_0            /*!< Memory to peripheral direction */
#define LL_DMA_DIRECTION_MEMORY_TO_MEMORY DMA_SxCR_DIR_1            /*!< Memory to memory direction     */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_MODE MODE
  * @{
  */
#define LL_DMA_MODE_NORMAL                0x00000000U               /*!< Normal Mode                  */
#define LL_DMA_MODE_CIRCULAR              DMA_SxCR_CIRC             /*!< Circular Mode                */
#define LL_DMA_MODE_PFCTRL                DMA_SxCR_PFCTRL           /*!< Peripheral flow control mode */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DOUBLEBUFFER_MODE DOUBLE BUFFER MODE
  * @{
  */
#define LL_DMA_DOUBLEBUFFER_MODE_DISABLE  0x00000000U               /*!< Disable double buffering mode */
#define LL_DMA_DOUBLEBUFFER_MODE_ENABLE   DMA_SxCR_DBM              /*!< Enable double buffering mode  */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_PERIPH PERIPH
  * @{
  */
#define LL_DMA_PERIPH_NOINCREMENT         0x00000000U               /*!< Peripheral increment mode Disable */
#define LL_DMA_PERIPH_INCREMENT           DMA_SxCR_PINC             /*!< Peripheral increment mode Enable  */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_MEMORY MEMORY
  * @{
  */
#define LL_DMA_MEMORY_NOINCREMENT         0x00000000U               /*!< Memory increment mode Disable */
#define LL_DMA_MEMORY_INCREMENT           DMA_SxCR_MINC             /*!< Memory increment mode Enable  */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_PDATAALIGN PDATAALIGN
  * @{
  */
#define LL_DMA_PDATAALIGN_BYTE            0x00000000U               /*!< Peripheral data alignment : Byte     */
#define LL_DMA_PDATAALIGN_HALFWORD        DMA_SxCR_PSIZE_0          /*!< Peripheral data alignment : HalfWord */
#define LL_DMA_PDATAALIGN_WORD            DMA_SxCR_PSIZE_1          /*!< Peripheral data alignment : Word     */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_MDATAALIGN MDATAALIGN
  * @{
  */
#define LL_DMA_MDATAALIGN_BYTE            0x00000000U               /*!< Memory data alignment : Byte     */
#define LL_DMA_MDATAALIGN_HALFWORD        DMA_SxCR_MSIZE_0          /*!< Memory data alignment : HalfWord */
#define LL_DMA_MDATAALIGN_WORD            DMA_SxCR_MSIZE_1          /*!< Memory data alignment : Word     */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_OFFSETSIZE OFFSETSIZE
  * @{
  */
#define LL_DMA_OFFSETSIZE_PSIZE           0x00000000U               /*!< Peripheral increment offset size is linked to the PSIZE           */
#define LL_DMA_OFFSETSIZE_FIXEDTO4        DMA_SxCR_PINCOS           /*!< Peripheral increment offset size is fixed to 4 (32-bit alignment) */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_PRIORITY PRIORITY
  * @{
  */
#define LL_DMA_PRIORITY_LOW               0x00000000U               /*!< Priority level : Low       */
#define LL_DMA_PRIORITY_MEDIUM            DMA_SxCR_PL_0             /*!< Priority level : Medium    */
#define LL_DMA_PRIORITY_HIGH              DMA_SxCR_PL_1             /*!< Priority level : High      */
#define LL_DMA_PRIORITY_VERYHIGH          DMA_SxCR_PL               /*!< Priority level : Very_High */
/**
  * @}
  */


/** @defgroup DMA_LL_EC_MBURST MBURST
  * @{
  */
#define LL_DMA_MBURST_SINGLE              0x00000000U                             /*!< Memory burst single transfer configuration      */
#define LL_DMA_MBURST_INC4                DMA_SxCR_MBURST_0                       /*!< Memory burst of 4 beats transfer configuration  */
#define LL_DMA_MBURST_INC8                DMA_SxCR_MBURST_1                       /*!< Memory burst of 8 beats transfer configuration  */
#define LL_DMA_MBURST_INC16               (DMA_SxCR_MBURST_0 | DMA_SxCR_MBURST_1) /*!< Memory burst of 16 beats transfer configuration */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_PBURST PBURST
  * @{
  */
#define LL_DMA_PBURST_SINGLE              0x00000000U                             /*!< Peripheral burst single transfer configuration      */
#define LL_DMA_PBURST_INC4                DMA_SxCR_PBURST_0                       /*!< Peripheral burst of 4 beats transfer configuration  */
#define LL_DMA_PBURST_INC8                DMA_SxCR_PBURST_1                       /*!< Peripheral burst of 8 beats transfer configuration  */
#define LL_DMA_PBURST_INC16               (DMA_SxCR_PBURST_0 | DMA_SxCR_PBURST_1) /*!< Peripheral burst of 16 beats transfer configuration */
/**
  * @}
  */

/** @defgroup DMA_LL_FIFOMODE DMA_LL_FIFOMODE
  * @{
  */
#define LL_DMA_FIFOMODE_DISABLE           0x00000000U                             /*!< FIFO mode disable (direct mode is enabled) */
#define LL_DMA_FIFOMODE_ENABLE            DMA_SxFCR_DMDIS                         /*!< FIFO mode enable                           */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_FIFOSTATUS_0 FIFOSTATUS 0
  * @{
  */
#define LL_DMA_FIFOSTATUS_0_25            0x00000000U                             /*!< 0 < fifo_level < 1/4    */
#define LL_DMA_FIFOSTATUS_25_50           DMA_SxFCR_FS_0                          /*!< 1/4 < fifo_level < 1/2  */
#define LL_DMA_FIFOSTATUS_50_75           DMA_SxFCR_FS_1                          /*!< 1/2 < fifo_level < 3/4  */
#define LL_DMA_FIFOSTATUS_75_100          (DMA_SxFCR_FS_1 | DMA_SxFCR_FS_0)       /*!< 3/4 < fifo_level < full */
#define LL_DMA_FIFOSTATUS_EMPTY           DMA_SxFCR_FS_2                          /*!< FIFO is empty           */
#define LL_DMA_FIFOSTATUS_FULL            (DMA_SxFCR_FS_2 | DMA_SxFCR_FS_0)       /*!< FIFO is full            */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_FIFOTHRESHOLD FIFOTHRESHOLD
  * @{
  */
#define LL_DMA_FIFOTHRESHOLD_1_4          0x00000000U                             /*!< FIFO threshold 1 quart full configuration  */
#define LL_DMA_FIFOTHRESHOLD_1_2          DMA_SxFCR_FTH_0                         /*!< FIFO threshold half full configuration     */
#define LL_DMA_FIFOTHRESHOLD_3_4          DMA_SxFCR_FTH_1                         /*!< FIFO threshold 3 quarts full configuration */
#define LL_DMA_FIFOTHRESHOLD_FULL         DMA_SxFCR_FTH                           /*!< FIFO threshold full configuration          */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_CURRENTTARGETMEM CURRENTTARGETMEM
  * @{
  */
#define LL_DMA_CURRENTTARGETMEM0          0x00000000U                             /*!< Set CurrentTarget Memory to Memory 0  */
#define LL_DMA_CURRENTTARGETMEM1          DMA_SxCR_CT                             /*!< Set CurrentTarget Memory to Memory 1  */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup DMA_LL_Exported_Macros DMA Exported Macros
  * @{
  */

/** @defgroup DMA_LL_EM_WRITE_READ Common Write and read registers macros
  * @{
  */
/**
  * @brief  Write a value in DMA register
  * @param  __INSTANCE__ DMA Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_DMA_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in DMA register
  * @param  __INSTANCE__ DMA Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_DMA_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/** @defgroup DMA_LL_EM_CONVERT_DMAxCHANNELy Convert DMAxStreamy
  * @{
  */
/**
  * @brief  Convert DMAx_Streamy into DMAx
  * @param  __STREAM_INSTANCE__ DMAx_Streamy
  * @retval DMAx
  */
#define __LL_DMA_GET_INSTANCE(__STREAM_INSTANCE__)   \
(((uint32_t)(__STREAM_INSTANCE__) > ((uint32_t)DMA1_Stream7)) ?  DMA2 : DMA1)

/**
  * @brief  Convert DMAx_Streamy into LL_DMA_STREAM_y
  * @param  __STREAM_INSTANCE__ DMAx_Streamy
  * @retval LL_DMA_STREAM_y
  */
#define __LL_DMA_GET_STREAM(__STREAM_INSTANCE__)   \
(((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream0)) ? LL_DMA_STREAM_0 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream0)) ? LL_DMA_STREAM_0 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream1)) ? LL_DMA_STREAM_1 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream1)) ? LL_DMA_STREAM_1 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream2)) ? LL_DMA_STREAM_2 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream2)) ? LL_DMA_STREAM_2 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream3)) ? LL_DMA_STREAM_3 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream3)) ? LL_DMA_STREAM_3 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream4)) ? LL_DMA_STREAM_4 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream4)) ? LL_DMA_STREAM_4 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream5)) ? LL_DMA_STREAM_5 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream5)) ? LL_DMA_STREAM_5 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA1_Stream6)) ? LL_DMA_STREAM_6 : \
 ((uint32_t)(__STREAM_INSTANCE__) == ((uint32_t)DMA2_Stream6)) ? LL_DMA_STREAM_6 : \
 LL_DMA_STREAM_7)

/**
  * @brief  Convert DMA Instance DMAx and LL_DMA_STREAM_y into DMAx_Streamy
  * @param  __DMA_INSTANCE__ DMAx
  * @param  __STREAM__ LL_DMA_STREAM_y
  * @retval DMAx_Streamy
  */
#define __LL_DMA_GET_STREAM_INSTANCE(__DMA_INSTANCE__, __STREAM__)   \
((((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_0))) ? DMA1_Stream0 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_0))) ? DMA2_Stream0 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_1))) ? DMA1_Stream1 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_1))) ? DMA2_Stream1 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_2))) ? DMA1_Stream2 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_2))) ? DMA2_Stream2 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_3))) ? DMA1_Stream3 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_3))) ? DMA2_Stream3 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_4))) ? DMA1_Stream4 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_4))) ? DMA2_Stream4 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_5))) ? DMA1_Stream5 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_5))) ? DMA2_Stream5 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_6))) ? DMA1_Stream6 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA2)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_6))) ? DMA2_Stream6 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__STREAM__) == ((uint32_t)LL_DMA_STREAM_7))) ? DMA1_Stream7 : \
 DMA2_Stream7)

/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup DMA_LL_Exported_Functions DMA Exported Functions
 * @{
 */

/** @defgroup DMA_LL_EF_Configuration Configuration
  * @{
  */
/**
  * @brief Enable DMA stream.
  * @rmtoll CR          EN            LL_DMA_EnableStream
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableStream(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_EN);
}

/**
  * @brief Disable DMA stream.
  * @rmtoll CR          EN            LL_DMA_DisableStream
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableStream(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_EN);
}

/**
  * @brief Check if DMA stream is enabled or disabled.
  * @rmtoll CR          EN            LL_DMA_IsEnabledStream
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledStream(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return ((READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_EN) == (DMA_SxCR_EN)) ? 1UL : 0UL);
}

/**
  * @brief  Configure all parameters linked to DMA transfer.
  * @rmtoll CR          DIR           LL_DMA_ConfigTransfer\n
  *         CR          CIRC          LL_DMA_ConfigTransfer\n
  *         CR          PINC          LL_DMA_ConfigTransfer\n
  *         CR          MINC          LL_DMA_ConfigTransfer\n
  *         CR          PSIZE         LL_DMA_ConfigTransfer\n
  *         CR          MSIZE         LL_DMA_ConfigTransfer\n
  *         CR          PL            LL_DMA_ConfigTransfer\n
  *         CR          PFCTRL        LL_DMA_ConfigTransfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY or @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH or @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  *         @arg @ref LL_DMA_MODE_NORMAL or @ref LL_DMA_MODE_CIRCULAR  or @ref LL_DMA_MODE_PFCTRL
  *         @arg @ref LL_DMA_PERIPH_INCREMENT or @ref LL_DMA_PERIPH_NOINCREMENT
  *         @arg @ref LL_DMA_MEMORY_INCREMENT or @ref LL_DMA_MEMORY_NOINCREMENT
  *         @arg @ref LL_DMA_PDATAALIGN_BYTE or @ref LL_DMA_PDATAALIGN_HALFWORD or @ref LL_DMA_PDATAALIGN_WORD
  *         @arg @ref LL_DMA_MDATAALIGN_BYTE or @ref LL_DMA_MDATAALIGN_HALFWORD or @ref LL_DMA_MDATAALIGN_WORD
  *         @arg @ref LL_DMA_PRIORITY_LOW or @ref LL_DMA_PRIORITY_MEDIUM or @ref LL_DMA_PRIORITY_HIGH or @ref LL_DMA_PRIORITY_VERYHIGH
  *@retval None
  */
__STATIC_INLINE void LL_DMA_ConfigTransfer(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Configuration)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR,
             DMA_SxCR_DIR | DMA_SxCR_CIRC | DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE | DMA_SxCR_MSIZE | DMA_SxCR_PL | DMA_SxCR_PFCTRL,
             Configuration);
}

/**
  * @brief Set Data transfer direction (read from peripheral or from memory).
  * @rmtoll CR          DIR           LL_DMA_SetDataTransferDirection
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Direction This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetDataTransferDirection(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Direction)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DIR, Direction);
}

/**
  * @brief Get Data transfer direction (read from peripheral or from memory).
  * @rmtoll CR          DIR           LL_DMA_GetDataTransferDirection
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  */
__STATIC_INLINE uint32_t LL_DMA_GetDataTransferDirection(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DIR));
}

/**
  * @brief Set DMA mode normal, circular or peripheral flow control.
  * @rmtoll CR          CIRC           LL_DMA_SetMode\n
  *         CR          PFCTRL         LL_DMA_SetMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_MODE_NORMAL
  *         @arg @ref LL_DMA_MODE_CIRCULAR
  *         @arg @ref LL_DMA_MODE_PFCTRL
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetMode(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Mode)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_CIRC | DMA_SxCR_PFCTRL, Mode);
}

/**
  * @brief Get DMA mode normal, circular or peripheral flow control.
  * @rmtoll CR          CIRC           LL_DMA_GetMode\n
  *         CR          PFCTRL         LL_DMA_GetMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_MODE_NORMAL
  *         @arg @ref LL_DMA_MODE_CIRCULAR
  *         @arg @ref LL_DMA_MODE_PFCTRL
  */
__STATIC_INLINE uint32_t LL_DMA_GetMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_CIRC | DMA_SxCR_PFCTRL));
}

/**
  * @brief Set Peripheral increment mode.
  * @rmtoll CR          PINC           LL_DMA_SetPeriphIncMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  IncrementMode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_PERIPH_NOINCREMENT
  *         @arg @ref LL_DMA_PERIPH_INCREMENT
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetPeriphIncMode(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t IncrementMode)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PINC, IncrementMode);
}

/**
  * @brief Get Peripheral increment mode.
  * @rmtoll CR          PINC           LL_DMA_GetPeriphIncMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_PERIPH_NOINCREMENT
  *         @arg @ref LL_DMA_PERIPH_INCREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetPeriphIncMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PINC));
}

/**
  * @brief Set Memory increment mode.
  * @rmtoll CR          MINC           LL_DMA_SetMemoryIncMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  IncrementMode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_MEMORY_NOINCREMENT
  *         @arg @ref LL_DMA_MEMORY_INCREMENT
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetMemoryIncMode(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t IncrementMode)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_MINC, IncrementMode);
}

/**
  * @brief Get Memory increment mode.
  * @rmtoll CR          MINC           LL_DMA_GetMemoryIncMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_MEMORY_NOINCREMENT
  *         @arg @ref LL_DMA_MEMORY_INCREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetMemoryIncMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_MINC));
}

/**
  * @brief Set Peripheral size.
  * @rmtoll CR          PSIZE           LL_DMA_SetPeriphSize
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Size This parameter can be one of the following values:
  *         @arg @ref LL_DMA_PDATAALIGN_BYTE
  *         @arg @ref LL_DMA_PDATAALIGN_HALFWORD
  *         @arg @ref LL_DMA_PDATAALIGN_WORD
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetPeriphSize(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Size)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PSIZE, Size);
}

/**
  * @brief Get Peripheral size.
  * @rmtoll CR          PSIZE           LL_DMA_GetPeriphSize
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_PDATAALIGN_BYTE
  *         @arg @ref LL_DMA_PDATAALIGN_HALFWORD
  *         @arg @ref LL_DMA_PDATAALIGN_WORD
  */
__STATIC_INLINE uint32_t LL_DMA_GetPeriphSize(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PSIZE));
}

/**
  * @brief Set Memory size.
  * @rmtoll CR          MSIZE           LL_DMA_SetMemorySize
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Size This parameter can be one of the following values:
  *         @arg @ref LL_DMA_MDATAALIGN_BYTE
  *         @arg @ref LL_DMA_MDATAALIGN_HALFWORD
  *         @arg @ref LL_DMA_MDATAALIGN_WORD
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetMemorySize(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Size)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_MSIZE, Size);
}

/**
  * @brief Get Memory size.
  * @rmtoll CR          MSIZE           LL_DMA_GetMemorySize
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_MDATAALIGN_BYTE
  *         @arg @ref LL_DMA_MDATAALIGN_HALFWORD
  *         @arg @ref LL_DMA_MDATAALIGN_WORD
  */
__STATIC_INLINE uint32_t LL_DMA_GetMemorySize(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_MSIZE));
}

/**
  * @brief Set Peripheral increment offset size.
  * @rmtoll CR          PINCOS           LL_DMA_SetIncOffsetSize
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  OffsetSize This parameter can be one of the following values:
  *         @arg @ref LL_DMA_OFFSETSIZE_PSIZE
  *         @arg @ref LL_DMA_OFFSETSIZE_FIXEDTO4
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetIncOffsetSize(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t OffsetSize)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PINCOS, OffsetSize);
}

/**
  * @brief Get Peripheral increment offset size.
  * @rmtoll CR          PINCOS           LL_DMA_GetIncOffsetSize
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_OFFSETSIZE_PSIZE
  *         @arg @ref LL_DMA_OFFSETSIZE_FIXEDTO4
  */
__STATIC_INLINE uint32_t LL_DMA_GetIncOffsetSize(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PINCOS));
}

/**
  * @brief Set Stream priority level.
  * @rmtoll CR          PL           LL_DMA_SetStreamPriorityLevel
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Priority This parameter can be one of the following values:
  *         @arg @ref LL_DMA_PRIORITY_LOW
  *         @arg @ref LL_DMA_PRIORITY_MEDIUM
  *         @arg @ref LL_DMA_PRIORITY_HIGH
  *         @arg @ref LL_DMA_PRIORITY_VERYHIGH
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetStreamPriorityLevel(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Priority)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PL, Priority);
}

/**
  * @brief Get Stream priority level.
  * @rmtoll CR          PL           LL_DMA_GetStreamPriorityLevel
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_PRIORITY_LOW
  *         @arg @ref LL_DMA_PRIORITY_MEDIUM
  *         @arg @ref LL_DMA_PRIORITY_HIGH
  *         @arg @ref LL_DMA_PRIORITY_VERYHIGH
  */
__STATIC_INLINE uint32_t LL_DMA_GetStreamPriorityLevel(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PL));
}

/**
  * @brief Enable DMA stream bufferable transfer.
  * @rmtoll CR          TRBUFF            LL_DMA_EnableBufferableTransfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableBufferableTransfer(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TRBUFF);
}

/**
  * @brief Disable DMA stream bufferable transfer.
  * @rmtoll CR          TRBUFF            LL_DMA_DisableBufferableTransfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableBufferableTransfer(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TRBUFF);
}

/**
  * @brief Set Number of data to transfer.
  * @rmtoll NDTR          NDT           LL_DMA_SetDataLength
  * @note   This action has no effect if
  *         stream is enabled.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  NbData Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetDataLength(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t NbData)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->NDTR, DMA_SxNDT, NbData);
}

/**
  * @brief Get Number of data to transfer.
  * @rmtoll NDTR          NDT           LL_DMA_GetDataLength
  * @note   Once the stream is enabled, the return value indicate the
  *         remaining bytes to be transmitted.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetDataLength(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->NDTR, DMA_SxNDT));
}
/**
  * @brief  Set DMA request for DMA Streams on DMAMUX Channel x.
  * @note   DMAMUX channel 0 to 7 are mapped to DMA1 stream 0 to 7.
  *         DMAMUX channel 8 to 15 are mapped to DMA2 stream 0 to 7.
  * @rmtoll CxCR         DMAREQ_ID     LL_DMA_SetPeriphRequest
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Request This parameter can be one of the following values:
  *         @arg @ref LL_DMAMUX1_REQ_MEM2MEM
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR0
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR1
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR2
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR3
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR4
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR5
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR6
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR7
  *         @arg @ref LL_DMAMUX1_REQ_ADC1
  *         @arg @ref LL_DMAMUX1_REQ_ADC2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_UP
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_TX
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_TX
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM6_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM7_UP
  *         @arg @ref LL_DMAMUX1_REQ_USART6_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART6_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_TX
  *         @arg @ref LL_DMAMUX1_REQ_DCMI_PSSI
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_IN
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_OUT
  *         @arg @ref LL_DMAMUX1_REQ_HASH_IN
  *         @arg @ref LL_DMAMUX1_REQ_UART7_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART7_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_TX
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_B
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_B
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_RX
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_DT
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_CS
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_MASTER (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_C (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_D (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_E (*)
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT0
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT1
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT2
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT3
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_UP
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_ADC3 (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_RX (*)
  * @note   (*) Availability depends on devices.
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetPeriphRequest(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Request)
{
  MODIFY_REG(((DMAMUX_Channel_TypeDef *)(uint32_t)((uint32_t)DMAMUX1_Channel0 + (DMAMUX_CCR_SIZE * (Stream)) + (uint32_t)(DMAMUX_CCR_SIZE * LL_DMA_INSTANCE_TO_DMAMUX_CHANNEL(DMAx))))->CCR, DMAMUX_CxCR_DMAREQ_ID, Request);
}

/**
  * @brief  Get DMA request for DMA Channels on DMAMUX Channel x.
  * @note   DMAMUX channel 0 to 7 are mapped to DMA1 stream 0 to 7.
  *         DMAMUX channel 8 to 15 are mapped to DMA2 stream 0 to 7.
  * @rmtoll CxCR         DMAREQ_ID     LL_DMA_GetPeriphRequest
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMAMUX1_REQ_MEM2MEM
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR0
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR1
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR2
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR3
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR4
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR5
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR6
  *         @arg @ref LL_DMAMUX1_REQ_GENERATOR7
  *         @arg @ref LL_DMAMUX1_REQ_ADC1
  *         @arg @ref LL_DMAMUX1_REQ_ADC2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM1_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM2_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM3_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM4_UP
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C1_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C2_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI1_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART1_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART2_TX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART3_TX
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM8_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH3
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_CH4
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM5_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI3_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART4_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART5_TX
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH1
  *         @arg @ref LL_DMAMUX1_REQ_DAC1_CH2
  *         @arg @ref LL_DMAMUX1_REQ_TIM6_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM7_UP
  *         @arg @ref LL_DMAMUX1_REQ_USART6_RX
  *         @arg @ref LL_DMAMUX1_REQ_USART6_TX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_RX
  *         @arg @ref LL_DMAMUX1_REQ_I2C3_TX
  *         @arg @ref LL_DMAMUX1_REQ_DCMI_PSSI
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_IN
  *         @arg @ref LL_DMAMUX1_REQ_CRYP_OUT
  *         @arg @ref LL_DMAMUX1_REQ_HASH_IN
  *         @arg @ref LL_DMAMUX1_REQ_UART7_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART7_TX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_RX
  *         @arg @ref LL_DMAMUX1_REQ_UART8_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI4_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_RX
  *         @arg @ref LL_DMAMUX1_REQ_SPI5_TX
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI1_B
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_A
  *         @arg @ref LL_DMAMUX1_REQ_SAI2_B
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_RX
  *         @arg @ref LL_DMAMUX1_REQ_SWPMI_TX
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_DT
  *         @arg @ref LL_DMAMUX1_REQ_SPDIF_RX_CS
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_MASTER (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_C (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_D (*)
  *         @arg @ref LL_DMAMUX1_REQ_HRTIM_TIMER_E (*)
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT0
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT1
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT2
  *         @arg @ref LL_DMAMUX1_REQ_DFSDM1_FLT3
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_TRIG
  *         @arg @ref LL_DMAMUX1_REQ_TIM15_COM
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM16_UP
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_CH1
  *         @arg @ref LL_DMAMUX1_REQ_TIM17_UP
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_A (*)
  *         @arg @ref LL_DMAMUX1_REQ_SAI3_B (*)
  *         @arg @ref LL_DMAMUX1_REQ_ADC3 (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_UART9_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_RX (*)
  *         @arg @ref LL_DMAMUX1_REQ_USART10_RX (*)
  * @note   (*) Availability depends on devices.
  */
__STATIC_INLINE  uint32_t LL_DMA_GetPeriphRequest(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (READ_BIT(((DMAMUX_Channel_TypeDef *)((uint32_t)((uint32_t)DMAMUX1_Channel0 + (DMAMUX_CCR_SIZE * (Stream)) + (uint32_t)(DMAMUX_CCR_SIZE * LL_DMA_INSTANCE_TO_DMAMUX_CHANNEL(DMAx)))))->CCR, DMAMUX_CxCR_DMAREQ_ID));
}

/**
  * @brief Set Memory burst transfer configuration.
  * @rmtoll CR          MBURST           LL_DMA_SetMemoryBurstxfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Mburst This parameter can be one of the following values:
  *         @arg @ref LL_DMA_MBURST_SINGLE
  *         @arg @ref LL_DMA_MBURST_INC4
  *         @arg @ref LL_DMA_MBURST_INC8
  *         @arg @ref LL_DMA_MBURST_INC16
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetMemoryBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Mburst)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_MBURST, Mburst);
}

/**
  * @brief Get Memory burst transfer configuration.
  * @rmtoll CR          MBURST           LL_DMA_GetMemoryBurstxfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_MBURST_SINGLE
  *         @arg @ref LL_DMA_MBURST_INC4
  *         @arg @ref LL_DMA_MBURST_INC8
  *         @arg @ref LL_DMA_MBURST_INC16
  */
__STATIC_INLINE uint32_t LL_DMA_GetMemoryBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_MBURST));
}

/**
  * @brief Set  Peripheral burst transfer configuration.
  * @rmtoll CR          PBURST           LL_DMA_SetPeriphBurstxfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Pburst This parameter can be one of the following values:
  *         @arg @ref LL_DMA_PBURST_SINGLE
  *         @arg @ref LL_DMA_PBURST_INC4
  *         @arg @ref LL_DMA_PBURST_INC8
  *         @arg @ref LL_DMA_PBURST_INC16
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetPeriphBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Pburst)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PBURST, Pburst);
}

/**
  * @brief Get Peripheral burst transfer configuration.
  * @rmtoll CR          PBURST           LL_DMA_GetPeriphBurstxfer
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_PBURST_SINGLE
  *         @arg @ref LL_DMA_PBURST_INC4
  *         @arg @ref LL_DMA_PBURST_INC8
  *         @arg @ref LL_DMA_PBURST_INC16
  */
__STATIC_INLINE uint32_t LL_DMA_GetPeriphBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_PBURST));
}

/**
  * @brief Set Current target (only in double buffer mode) to Memory 1 or Memory 0.
  * @rmtoll CR          CT           LL_DMA_SetCurrentTargetMem
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param CurrentMemory This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CURRENTTARGETMEM0
  *         @arg @ref LL_DMA_CURRENTTARGETMEM1
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetCurrentTargetMem(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t CurrentMemory)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_CT, CurrentMemory);
}

/**
  * @brief Set Current target (only in double buffer mode) to Memory 1 or Memory 0.
  * @rmtoll CR          CT           LL_DMA_GetCurrentTargetMem
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_CURRENTTARGETMEM0
  *         @arg @ref LL_DMA_CURRENTTARGETMEM1
  */
__STATIC_INLINE uint32_t LL_DMA_GetCurrentTargetMem(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_CT));
}

/**
  * @brief Enable the double buffer mode.
  * @rmtoll CR          DBM           LL_DMA_EnableDoubleBufferMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableDoubleBufferMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DBM);
}

/**
  * @brief Disable the double buffer mode.
  * @rmtoll CR          DBM           LL_DMA_DisableDoubleBufferMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableDoubleBufferMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DBM);
}

/**
  * @brief Get FIFO status.
  * @rmtoll FCR          FS          LL_DMA_GetFIFOStatus
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_FIFOSTATUS_0_25
  *         @arg @ref LL_DMA_FIFOSTATUS_25_50
  *         @arg @ref LL_DMA_FIFOSTATUS_50_75
  *         @arg @ref LL_DMA_FIFOSTATUS_75_100
  *         @arg @ref LL_DMA_FIFOSTATUS_EMPTY
  *         @arg @ref LL_DMA_FIFOSTATUS_FULL
  */
__STATIC_INLINE uint32_t LL_DMA_GetFIFOStatus(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FS));
}

/**
  * @brief Disable Fifo mode.
  * @rmtoll FCR          DMDIS          LL_DMA_DisableFifoMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableFifoMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_DMDIS);
}

/**
  * @brief Enable Fifo mode.
  * @rmtoll FCR          DMDIS          LL_DMA_EnableFifoMode
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableFifoMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_DMDIS);
}

/**
  * @brief Select FIFO threshold.
  * @rmtoll FCR         FTH          LL_DMA_SetFIFOThreshold
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Threshold This parameter can be one of the following values:
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_1_4
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_1_2
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_3_4
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_FULL
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetFIFOThreshold(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Threshold)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FTH, Threshold);
}

/**
  * @brief Get FIFO threshold.
  * @rmtoll FCR         FTH          LL_DMA_GetFIFOThreshold
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_1_4
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_1_2
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_3_4
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_FULL
  */
__STATIC_INLINE uint32_t LL_DMA_GetFIFOThreshold(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FTH));
}

/**
  * @brief Configure the FIFO .
  * @rmtoll FCR         FTH          LL_DMA_ConfigFifo\n
  *         FCR         DMDIS        LL_DMA_ConfigFifo
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  FifoMode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_FIFOMODE_ENABLE
  *         @arg @ref LL_DMA_FIFOMODE_DISABLE
  * @param  FifoThreshold This parameter can be one of the following values:
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_1_4
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_1_2
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_3_4
  *         @arg @ref LL_DMA_FIFOTHRESHOLD_FULL
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ConfigFifo(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t FifoMode, uint32_t FifoThreshold)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FTH | DMA_SxFCR_DMDIS, FifoMode | FifoThreshold);
}

/**
  * @brief Configure the Source and Destination addresses.
  * @note   This API must not be called when the DMA stream is enabled.
  * @rmtoll M0AR        M0A         LL_DMA_ConfigAddresses\n
  *         PAR         PA          LL_DMA_ConfigAddresses
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  SrcAddress Between 0 to 0xFFFFFFFF
  * @param  DstAddress Between 0 to 0xFFFFFFFF
  * @param  Direction This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ConfigAddresses(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t SrcAddress, uint32_t DstAddress, uint32_t Direction)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  /* Direction Memory to Periph */
  if (Direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH)
  {
    WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M0AR, SrcAddress);
    WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->PAR, DstAddress);
  }
  /* Direction Periph to Memory and Memory to Memory */
  else
  {
    WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->PAR, SrcAddress);
    WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M0AR, DstAddress);
  }
}

/**
  * @brief  Set the Memory address.
  * @rmtoll M0AR        M0A         LL_DMA_SetMemoryAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_PERIPH_TO_MEMORY or LL_DMA_DIRECTION_MEMORY_TO_PERIPH only.
  * @note   This API must not be called when the DMA stream is enabled.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  MemoryAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetMemoryAddress(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t MemoryAddress)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M0AR, MemoryAddress);
}

/**
  * @brief  Set the Peripheral address.
  * @rmtoll PAR        PA         LL_DMA_SetPeriphAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_PERIPH_TO_MEMORY or LL_DMA_DIRECTION_MEMORY_TO_PERIPH only.
  * @note   This API must not be called when the DMA stream is enabled.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  PeriphAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetPeriphAddress(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t PeriphAddress)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->PAR, PeriphAddress);
}

/**
  * @brief  Get the Memory address.
  * @rmtoll M0AR        M0A         LL_DMA_GetMemoryAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_PERIPH_TO_MEMORY or LL_DMA_DIRECTION_MEMORY_TO_PERIPH only.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetMemoryAddress(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M0AR));
}

/**
  * @brief  Get the Peripheral address.
  * @rmtoll PAR        PA         LL_DMA_GetPeriphAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_PERIPH_TO_MEMORY or LL_DMA_DIRECTION_MEMORY_TO_PERIPH only.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetPeriphAddress(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->PAR));
}

/**
  * @brief  Set the Memory to Memory Source address.
  * @rmtoll PAR        PA         LL_DMA_SetM2MSrcAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_MEMORY_TO_MEMORY only.
  * @note   This API must not be called when the DMA stream is enabled.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  MemoryAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetM2MSrcAddress(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t MemoryAddress)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->PAR, MemoryAddress);
}

/**
  * @brief  Set the Memory to Memory Destination address.
  * @rmtoll M0AR        M0A         LL_DMA_SetM2MDstAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_MEMORY_TO_MEMORY only.
  * @note   This API must not be called when the DMA stream is enabled.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  MemoryAddress Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetM2MDstAddress(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t MemoryAddress)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  WRITE_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M0AR, MemoryAddress);
}

/**
  * @brief  Get the Memory to Memory Source address.
  * @rmtoll PAR        PA         LL_DMA_GetM2MSrcAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_MEMORY_TO_MEMORY only.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetM2MSrcAddress(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->PAR));
}

/**
  * @brief  Get the Memory to Memory Destination address.
  * @rmtoll M0AR        M0A         LL_DMA_GetM2MDstAddress
  * @note   Interface used for direction LL_DMA_DIRECTION_MEMORY_TO_MEMORY only.
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetM2MDstAddress(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (READ_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M0AR));
}

/**
  * @brief Set Memory 1 address (used in case of Double buffer mode).
  * @rmtoll M1AR        M1A         LL_DMA_SetMemory1Address
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @param  Address Between 0 to 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_DMA_SetMemory1Address(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Address)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  MODIFY_REG(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M1AR, DMA_SxM1AR_M1A, Address);
}

/**
  * @brief Get Memory 1 address (used in case of Double buffer mode).
  * @rmtoll M1AR        M1A         LL_DMA_GetMemory1Address
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetMemory1Address(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return (((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->M1AR);
}

/**
  * @}
  */

/** @defgroup DMA_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief Get Stream 0 half transfer flag.
  * @rmtoll LISR  HTIF0    LL_DMA_IsActiveFlag_HT0
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT0(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_HTIF0) == (DMA_LISR_HTIF0)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 1 half transfer flag.
  * @rmtoll LISR  HTIF1    LL_DMA_IsActiveFlag_HT1
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT1(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_HTIF1) == (DMA_LISR_HTIF1)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 2 half transfer flag.
  * @rmtoll LISR  HTIF2    LL_DMA_IsActiveFlag_HT2
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT2(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_HTIF2) == (DMA_LISR_HTIF2)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 3 half transfer flag.
  * @rmtoll LISR  HTIF3    LL_DMA_IsActiveFlag_HT3
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT3(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_HTIF3) == (DMA_LISR_HTIF3)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 4 half transfer flag.
  * @rmtoll HISR  HTIF4    LL_DMA_IsActiveFlag_HT4
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT4(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_HTIF4) == (DMA_HISR_HTIF4)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 5 half transfer flag.
  * @rmtoll HISR  HTIF0    LL_DMA_IsActiveFlag_HT5
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT5(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_HTIF5) == (DMA_HISR_HTIF5)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 6 half transfer flag.
  * @rmtoll HISR  HTIF6    LL_DMA_IsActiveFlag_HT6
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT6(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_HTIF6) == (DMA_HISR_HTIF6)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 7 half transfer flag.
  * @rmtoll HISR  HTIF7    LL_DMA_IsActiveFlag_HT7
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT7(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_HTIF7) == (DMA_HISR_HTIF7)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 0 transfer complete flag.
  * @rmtoll LISR  TCIF0    LL_DMA_IsActiveFlag_TC0
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC0(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TCIF0) == (DMA_LISR_TCIF0)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 1 transfer complete flag.
  * @rmtoll LISR  TCIF1    LL_DMA_IsActiveFlag_TC1
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC1(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TCIF1) == (DMA_LISR_TCIF1)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 2 transfer complete flag.
  * @rmtoll LISR  TCIF2    LL_DMA_IsActiveFlag_TC2
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC2(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TCIF2) == (DMA_LISR_TCIF2)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 3 transfer complete flag.
  * @rmtoll LISR  TCIF3    LL_DMA_IsActiveFlag_TC3
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC3(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TCIF3) == (DMA_LISR_TCIF3)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 4 transfer complete flag.
  * @rmtoll HISR  TCIF4    LL_DMA_IsActiveFlag_TC4
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC4(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TCIF4) == (DMA_HISR_TCIF4)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 5 transfer complete flag.
  * @rmtoll HISR  TCIF0    LL_DMA_IsActiveFlag_TC5
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC5(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TCIF5) == (DMA_HISR_TCIF5)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 6 transfer complete flag.
  * @rmtoll HISR  TCIF6    LL_DMA_IsActiveFlag_TC6
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC6(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TCIF6) == (DMA_HISR_TCIF6)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 7 transfer complete flag.
  * @rmtoll HISR  TCIF7    LL_DMA_IsActiveFlag_TC7
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC7(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TCIF7) == (DMA_HISR_TCIF7)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 0 transfer error flag.
  * @rmtoll LISR  TEIF0    LL_DMA_IsActiveFlag_TE0
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE0(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TEIF0) == (DMA_LISR_TEIF0)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 1 transfer error flag.
  * @rmtoll LISR  TEIF1    LL_DMA_IsActiveFlag_TE1
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE1(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TEIF1) == (DMA_LISR_TEIF1)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 2 transfer error flag.
  * @rmtoll LISR  TEIF2    LL_DMA_IsActiveFlag_TE2
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE2(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TEIF2) == (DMA_LISR_TEIF2)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 3 transfer error flag.
  * @rmtoll LISR  TEIF3    LL_DMA_IsActiveFlag_TE3
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE3(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_TEIF3) == (DMA_LISR_TEIF3)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 4 transfer error flag.
  * @rmtoll HISR  TEIF4    LL_DMA_IsActiveFlag_TE4
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE4(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TEIF4) == (DMA_HISR_TEIF4)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 5 transfer error flag.
  * @rmtoll HISR  TEIF0    LL_DMA_IsActiveFlag_TE5
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE5(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TEIF5) == (DMA_HISR_TEIF5)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 6 transfer error flag.
  * @rmtoll HISR  TEIF6    LL_DMA_IsActiveFlag_TE6
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE6(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TEIF6) == (DMA_HISR_TEIF6)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 7 transfer error flag.
  * @rmtoll HISR  TEIF7    LL_DMA_IsActiveFlag_TE7
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE7(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_TEIF7) == (DMA_HISR_TEIF7)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 0 direct mode error flag.
  * @rmtoll LISR  DMEIF0    LL_DMA_IsActiveFlag_DME0
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME0(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_DMEIF0) == (DMA_LISR_DMEIF0)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 1 direct mode error flag.
  * @rmtoll LISR  DMEIF1    LL_DMA_IsActiveFlag_DME1
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME1(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_DMEIF1) == (DMA_LISR_DMEIF1)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 2 direct mode error flag.
  * @rmtoll LISR  DMEIF2    LL_DMA_IsActiveFlag_DME2
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME2(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_DMEIF2) == (DMA_LISR_DMEIF2)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 3 direct mode error flag.
  * @rmtoll LISR  DMEIF3    LL_DMA_IsActiveFlag_DME3
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME3(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_DMEIF3) == (DMA_LISR_DMEIF3)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 4 direct mode error flag.
  * @rmtoll HISR  DMEIF4    LL_DMA_IsActiveFlag_DME4
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME4(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_DMEIF4) == (DMA_HISR_DMEIF4)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 5 direct mode error flag.
  * @rmtoll HISR  DMEIF0    LL_DMA_IsActiveFlag_DME5
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME5(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_DMEIF5) == (DMA_HISR_DMEIF5)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 6 direct mode error flag.
  * @rmtoll HISR  DMEIF6    LL_DMA_IsActiveFlag_DME6
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME6(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_DMEIF6) == (DMA_HISR_DMEIF6)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 7 direct mode error flag.
  * @rmtoll HISR  DMEIF7    LL_DMA_IsActiveFlag_DME7
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DME7(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_DMEIF7) == (DMA_HISR_DMEIF7)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 0 FIFO error flag.
  * @rmtoll LISR  FEIF0    LL_DMA_IsActiveFlag_FE0
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE0(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_FEIF0) == (DMA_LISR_FEIF0)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 1 FIFO error flag.
  * @rmtoll LISR  FEIF1    LL_DMA_IsActiveFlag_FE1
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE1(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_FEIF1) == (DMA_LISR_FEIF1)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 2 FIFO error flag.
  * @rmtoll LISR  FEIF2    LL_DMA_IsActiveFlag_FE2
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE2(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_FEIF2) == (DMA_LISR_FEIF2)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 3 FIFO error flag.
  * @rmtoll LISR  FEIF3    LL_DMA_IsActiveFlag_FE3
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE3(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->LISR, DMA_LISR_FEIF3) == (DMA_LISR_FEIF3)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 4 FIFO error flag.
  * @rmtoll HISR  FEIF4    LL_DMA_IsActiveFlag_FE4
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE4(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_FEIF4) == (DMA_HISR_FEIF4)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 5 FIFO error flag.
  * @rmtoll HISR  FEIF0    LL_DMA_IsActiveFlag_FE5
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE5(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_FEIF5) == (DMA_HISR_FEIF5)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 6 FIFO error flag.
  * @rmtoll HISR  FEIF6    LL_DMA_IsActiveFlag_FE6
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE6(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_FEIF6) == (DMA_HISR_FEIF6)) ? 1UL : 0UL);
}

/**
  * @brief Get Stream 7 FIFO error flag.
  * @rmtoll HISR  FEIF7    LL_DMA_IsActiveFlag_FE7
  * @param  DMAx DMAx Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_FE7(DMA_TypeDef *DMAx)
{
  return ((READ_BIT(DMAx->HISR, DMA_HISR_FEIF7) == (DMA_HISR_FEIF7)) ? 1UL : 0UL);
}

/**
  * @brief Clear Stream 0 half transfer flag.
  * @rmtoll LIFCR  CHTIF0    LL_DMA_ClearFlag_HT0
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT0(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CHTIF0);
}

/**
  * @brief Clear Stream 1 half transfer flag.
  * @rmtoll LIFCR  CHTIF1    LL_DMA_ClearFlag_HT1
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CHTIF1);
}

/**
  * @brief Clear Stream 2 half transfer flag.
  * @rmtoll LIFCR  CHTIF2    LL_DMA_ClearFlag_HT2
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CHTIF2);
}

/**
  * @brief Clear Stream 3 half transfer flag.
  * @rmtoll LIFCR  CHTIF3    LL_DMA_ClearFlag_HT3
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CHTIF3);
}

/**
  * @brief Clear Stream 4 half transfer flag.
  * @rmtoll HIFCR  CHTIF4    LL_DMA_ClearFlag_HT4
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT4(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CHTIF4);
}

/**
  * @brief Clear Stream 5 half transfer flag.
  * @rmtoll HIFCR  CHTIF5    LL_DMA_ClearFlag_HT5
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT5(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CHTIF5);
}

/**
  * @brief Clear Stream 6 half transfer flag.
  * @rmtoll HIFCR  CHTIF6    LL_DMA_ClearFlag_HT6
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT6(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CHTIF6);
}

/**
  * @brief Clear Stream 7 half transfer flag.
  * @rmtoll HIFCR  CHTIF7    LL_DMA_ClearFlag_HT7
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT7(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CHTIF7);
}

/**
  * @brief Clear Stream 0 transfer complete flag.
  * @rmtoll LIFCR  CTCIF0    LL_DMA_ClearFlag_TC0
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC0(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTCIF0);
}

/**
  * @brief Clear Stream 1 transfer complete flag.
  * @rmtoll LIFCR  CTCIF1    LL_DMA_ClearFlag_TC1
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTCIF1);
}

/**
  * @brief Clear Stream 2 transfer complete flag.
  * @rmtoll LIFCR  CTCIF2    LL_DMA_ClearFlag_TC2
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTCIF2);
}

/**
  * @brief Clear Stream 3 transfer complete flag.
  * @rmtoll LIFCR  CTCIF3    LL_DMA_ClearFlag_TC3
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTCIF3);
}

/**
  * @brief Clear Stream 4 transfer complete flag.
  * @rmtoll HIFCR  CTCIF4    LL_DMA_ClearFlag_TC4
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC4(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTCIF4);
}

/**
  * @brief Clear Stream 5 transfer complete flag.
  * @rmtoll HIFCR  CTCIF5    LL_DMA_ClearFlag_TC5
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC5(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTCIF5);
}

/**
  * @brief Clear Stream 6 transfer complete flag.
  * @rmtoll HIFCR  CTCIF6    LL_DMA_ClearFlag_TC6
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC6(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTCIF6);
}

/**
  * @brief Clear Stream 7 transfer complete flag.
  * @rmtoll HIFCR  CTCIF7    LL_DMA_ClearFlag_TC7
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC7(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTCIF7);
}

/**
  * @brief Clear Stream 0 transfer error flag.
  * @rmtoll LIFCR  CTEIF0    LL_DMA_ClearFlag_TE0
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE0(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTEIF0);
}

/**
  * @brief Clear Stream 1 transfer error flag.
  * @rmtoll LIFCR  CTEIF1    LL_DMA_ClearFlag_TE1
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTEIF1);
}

/**
  * @brief Clear Stream 2 transfer error flag.
  * @rmtoll LIFCR  CTEIF2    LL_DMA_ClearFlag_TE2
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTEIF2);
}

/**
  * @brief Clear Stream 3 transfer error flag.
  * @rmtoll LIFCR  CTEIF3    LL_DMA_ClearFlag_TE3
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CTEIF3);
}

/**
  * @brief Clear Stream 4 transfer error flag.
  * @rmtoll HIFCR  CTEIF4    LL_DMA_ClearFlag_TE4
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE4(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTEIF4);
}

/**
  * @brief Clear Stream 5 transfer error flag.
  * @rmtoll HIFCR  CTEIF5    LL_DMA_ClearFlag_TE5
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE5(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTEIF5);
}

/**
  * @brief Clear Stream 6 transfer error flag.
  * @rmtoll HIFCR  CTEIF6    LL_DMA_ClearFlag_TE6
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE6(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTEIF6);
}

/**
  * @brief Clear Stream 7 transfer error flag.
  * @rmtoll HIFCR  CTEIF7    LL_DMA_ClearFlag_TE7
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TE7(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CTEIF7);
}

/**
  * @brief Clear Stream 0 direct mode error flag.
  * @rmtoll LIFCR  CDMEIF0    LL_DMA_ClearFlag_DME0
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME0(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CDMEIF0);
}

/**
  * @brief Clear Stream 1 direct mode error flag.
  * @rmtoll LIFCR  CDMEIF1    LL_DMA_ClearFlag_DME1
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CDMEIF1);
}

/**
  * @brief Clear Stream 2 direct mode error flag.
  * @rmtoll LIFCR  CDMEIF2    LL_DMA_ClearFlag_DME2
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CDMEIF2);
}

/**
  * @brief Clear Stream 3 direct mode error flag.
  * @rmtoll LIFCR  CDMEIF3    LL_DMA_ClearFlag_DME3
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CDMEIF3);
}

/**
  * @brief Clear Stream 4 direct mode error flag.
  * @rmtoll HIFCR  CDMEIF4    LL_DMA_ClearFlag_DME4
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME4(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CDMEIF4);
}

/**
  * @brief Clear Stream 5 direct mode error flag.
  * @rmtoll HIFCR  CDMEIF5    LL_DMA_ClearFlag_DME5
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME5(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CDMEIF5);
}

/**
  * @brief Clear Stream 6 direct mode error flag.
  * @rmtoll HIFCR  CDMEIF6    LL_DMA_ClearFlag_DME6
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME6(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CDMEIF6);
}

/**
  * @brief Clear Stream 7 direct mode error flag.
  * @rmtoll HIFCR  CDMEIF7    LL_DMA_ClearFlag_DME7
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DME7(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CDMEIF7);
}

/**
  * @brief Clear Stream 0 FIFO error flag.
  * @rmtoll LIFCR  CFEIF0    LL_DMA_ClearFlag_FE0
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE0(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CFEIF0);
}

/**
  * @brief Clear Stream 1 FIFO error flag.
  * @rmtoll LIFCR  CFEIF1    LL_DMA_ClearFlag_FE1
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CFEIF1);
}

/**
  * @brief Clear Stream 2 FIFO error flag.
  * @rmtoll LIFCR  CFEIF2    LL_DMA_ClearFlag_FE2
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CFEIF2);
}

/**
  * @brief Clear Stream 3 FIFO error flag.
  * @rmtoll LIFCR  CFEIF3    LL_DMA_ClearFlag_FE3
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->LIFCR, DMA_LIFCR_CFEIF3);
}

/**
  * @brief Clear Stream 4 FIFO error flag.
  * @rmtoll HIFCR  CFEIF4    LL_DMA_ClearFlag_FE4
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE4(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CFEIF4);
}

/**
  * @brief Clear Stream 5 FIFO error flag.
  * @rmtoll HIFCR  CFEIF5    LL_DMA_ClearFlag_FE5
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE5(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CFEIF5);
}

/**
  * @brief Clear Stream 6 FIFO error flag.
  * @rmtoll HIFCR  CFEIF6    LL_DMA_ClearFlag_FE6
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE6(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CFEIF6);
}

/**
  * @brief Clear Stream 7 FIFO error flag.
  * @rmtoll HIFCR  CFEIF7    LL_DMA_ClearFlag_FE7
  * @param  DMAx DMAx Instance
  * @retval None
  */
__STATIC_INLINE void LL_DMA_ClearFlag_FE7(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->HIFCR, DMA_HIFCR_CFEIF7);
}

/**
  * @}
  */

/** @defgroup DMA_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief Enable Half transfer interrupt.
  * @rmtoll CR        HTIE         LL_DMA_EnableIT_HT
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableIT_HT(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_HTIE);
}

/**
  * @brief Enable Transfer error interrupt.
  * @rmtoll CR        TEIE         LL_DMA_EnableIT_TE
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableIT_TE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TEIE);
}

/**
  * @brief Enable Transfer complete interrupt.
  * @rmtoll CR        TCIE         LL_DMA_EnableIT_TC
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableIT_TC(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TCIE);
}

/**
  * @brief Enable Direct mode error interrupt.
  * @rmtoll CR        DMEIE         LL_DMA_EnableIT_DME
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableIT_DME(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DMEIE);
}

/**
  * @brief Enable FIFO error interrupt.
  * @rmtoll FCR        FEIE         LL_DMA_EnableIT_FE
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_EnableIT_FE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  SET_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FEIE);
}

/**
  * @brief Disable Half transfer interrupt.
  * @rmtoll CR        HTIE         LL_DMA_DisableIT_HT
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableIT_HT(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_HTIE);
}

/**
  * @brief Disable Transfer error interrupt.
  * @rmtoll CR        TEIE         LL_DMA_DisableIT_TE
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableIT_TE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TEIE);
}

/**
  * @brief Disable Transfer complete interrupt.
  * @rmtoll CR        TCIE         LL_DMA_DisableIT_TC
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableIT_TC(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TCIE);
}

/**
  * @brief Disable Direct mode error interrupt.
  * @rmtoll CR        DMEIE         LL_DMA_DisableIT_DME
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableIT_DME(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DMEIE);
}

/**
  * @brief Disable FIFO error interrupt.
  * @rmtoll FCR        FEIE         LL_DMA_DisableIT_FE
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval None
  */
__STATIC_INLINE void LL_DMA_DisableIT_FE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  CLEAR_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FEIE);
}

/**
  * @brief Check if Half transfer interrup is enabled.
  * @rmtoll CR        HTIE         LL_DMA_IsEnabledIT_HT
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_HT(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return ((READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_HTIE) == DMA_SxCR_HTIE) ? 1UL : 0UL);
}

/**
  * @brief Check if Transfer error nterrup is enabled.
  * @rmtoll CR        TEIE         LL_DMA_IsEnabledIT_TE
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_TE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return ((READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TEIE) == DMA_SxCR_TEIE) ? 1UL : 0UL);
}

/**
  * @brief Check if Transfer complete interrup is enabled.
  * @rmtoll CR        TCIE         LL_DMA_IsEnabledIT_TC
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_TC(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return ((READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_TCIE) == DMA_SxCR_TCIE) ? 1UL : 0UL);
}

/**
  * @brief Check if Direct mode error interrupt is enabled.
  * @rmtoll CR        DMEIE         LL_DMA_IsEnabledIT_DME
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_DME(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return ((READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->CR, DMA_SxCR_DMEIE) == DMA_SxCR_DMEIE) ? 1UL : 0UL);
}

/**
  * @brief Check if FIFO error interrup is enabled.
  * @rmtoll FCR        FEIE         LL_DMA_IsEnabledIT_FE
  * @param  DMAx DMAx Instance
  * @param  Stream This parameter can be one of the following values:
  *         @arg @ref LL_DMA_STREAM_0
  *         @arg @ref LL_DMA_STREAM_1
  *         @arg @ref LL_DMA_STREAM_2
  *         @arg @ref LL_DMA_STREAM_3
  *         @arg @ref LL_DMA_STREAM_4
  *         @arg @ref LL_DMA_STREAM_5
  *         @arg @ref LL_DMA_STREAM_6
  *         @arg @ref LL_DMA_STREAM_7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_FE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  register uint32_t dma_base_addr = (uint32_t)DMAx;

  return ((READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + LL_DMA_STR_OFFSET_TAB[Stream]))->FCR, DMA_SxFCR_FEIE) == DMA_SxFCR_FEIE) ? 1UL : 0UL);
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup DMA_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

uint32_t LL_DMA_Init(DMA_TypeDef *DMAx, uint32_t Stream, LL_DMA_InitTypeDef *DMA_InitStruct);
uint32_t LL_DMA_DeInit(DMA_TypeDef *DMAx, uint32_t Stream);
void LL_DMA_StructInit(LL_DMA_InitTypeDef *DMA_InitStruct);

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

#endif /* DMA1 || DMA2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_LL_DMA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
