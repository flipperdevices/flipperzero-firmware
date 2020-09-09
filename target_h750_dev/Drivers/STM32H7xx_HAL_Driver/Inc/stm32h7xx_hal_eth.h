/**
  ******************************************************************************
  * @file    stm32h7xx_hal_eth.h
  * @author  MCD Application Team
  * @brief   Header file of ETH HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H7xx_HAL_ETH_H
#define STM32H7xx_HAL_ETH_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(ETH)
/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup ETH
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
#ifndef ETH_TX_DESC_CNT
 #define ETH_TX_DESC_CNT         4U
#endif
	 
#ifndef ETH_RX_DESC_CNT
 #define ETH_RX_DESC_CNT         4U
#endif

/*********************** Descriptors struct def section ************************/
/** @defgroup ETH_Exported_Types ETH Exported Types
  * @{
  */

/** 
  * @brief  ETH DMA Descriptor structure definition
  */
typedef struct
{
  __IO uint32_t DESC0;
  __IO uint32_t DESC1;
  __IO uint32_t DESC2;
  __IO uint32_t DESC3;
  __IO uint32_t BackupAddr0; /* used to store rx buffer 1 address */
  __IO uint32_t BackupAddr1; /* used to store rx buffer 2 address */
}ETH_DMADescTypeDef;
/** 
  * 
  */

/** 
  * @brief  ETH Buffers List structure definition
  */
typedef struct __ETH_BufferTypeDef
{
  uint8_t *buffer;                /*<! buffer address */
  
  uint32_t len;                   /*<! buffer length */
  
  struct __ETH_BufferTypeDef *next; /*<! Pointer to the next buffer in the list */	
}ETH_BufferTypeDef;
/** 
  * 
  */
  
/** 
  * @brief  DMA Transmit Descriptors Wrapper structure definition
  */
typedef struct
{
  uint32_t  TxDesc[ETH_TX_DESC_CNT];     /*<! Tx DMA descriptors addresses */
  
  uint32_t  CurTxDesc;               /*<! Current Tx descriptor index for packet transmission */
  
}ETH_TxDescListTypeDef;
/** 
  * 
  */

 /** 
  * @brief  Transmit Packet Configuration structure definition
  */
typedef struct
{
  uint32_t Attributes;              /*!< Tx packet HW features capabilities. 
                                         This parameter can be a combination of @ref ETH_Tx_Packet_Attributes*/
  
  uint32_t Length;                  /*!< Total packet length   */
  
  ETH_BufferTypeDef *TxBuffer;      /*!< Tx buffers pointers */
  
  uint32_t SrcAddrCtrl;             /*!< Specifies the source address insertion control.
                                         This parameter can be a value of @ref ETH_Tx_Packet_Source_Addr_Control */
  
  uint32_t CRCPadCtrl;             /*!< Specifies the CRC and Pad insertion and replacement control. 
                                        This parameter can be a value of @ref ETH_Tx_Packet_CRC_Pad_Control  */
  
  uint32_t ChecksumCtrl;           /*!< Specifies the checksum insertion control. 
                                        This parameter can be a value of @ref ETH_Tx_Packet_Checksum_Control  */
  
  uint32_t MaxSegmentSize;         /*!< Sets TCP maximum segment size only when TCP segmentation is enabled. 
                                        This parameter can be a value from 0x0 to 0x3FFF */
  
  uint32_t PayloadLen;             /*!< Sets Total payload length only when TCP segmentation is enabled.     
                                        This parameter can be a value from 0x0 to 0x3FFFF */
  
  uint32_t TCPHeaderLen;           /*!< Sets TCP header length only when TCP segmentation is enabled.
                                        This parameter can be a value from 0x5 to 0xF */

  uint32_t VlanTag;                /*!< Sets VLAN Tag only when VLAN is enabled. 
                                        This parameter can be a value from 0x0 to 0xFFFF*/
  
  uint32_t VlanCtrl;               /*!< Specifies VLAN Tag insertion control only when VLAN is enabled. 
                                        This parameter can be a value of @ref ETH_Tx_Packet_VLAN_Control */
  
  uint32_t InnerVlanTag;           /*!< Sets Inner VLAN Tag only when Inner VLAN is enabled.
                                        This parameter can be a value from 0x0 to 0x3FFFF */
  
  uint32_t InnerVlanCtrl;          /*!< Specifies Inner VLAN Tag insertion control only when Inner VLAN is enabled. 
                                        This parameter can be a value of @ref ETH_Tx_Packet_Inner_VLAN_Control   */
  
}ETH_TxPacketConfig;
/** 
  * 
  */

/** 
 * @brief  DMA Receive Descriptors Wrapper structure definition
 */
typedef struct
{
  uint32_t RxDesc[ETH_RX_DESC_CNT];     /*<! Rx DMA descriptors addresses. */
  
  uint32_t CurRxDesc;                   /*<! Current Rx descriptor, ready for next reception. */
  
  uint32_t FirstAppDesc;                /*<! First descriptor of last received packet. */
  
  uint32_t AppDescNbr;                  /*<! Number of descriptors of last received packet. */
 
  uint32_t AppContextDesc;              /*<! If 1 a context descriptor is present in last received packet.
                                             If 0 no context descriptor is present in last received packet. */
  
  uint32_t ItMode;                      /*<! If 1, DMA will generate the Rx complete interrupt.
                                             If 0, DMA will not generate the Rx complete interrupt. */ 
}ETH_RxDescListTypeDef;
/** 
  * 
  */

/** 
  * @brief  Received Packet Information structure definition
  */ 
typedef struct  
{  
  uint32_t SegmentCnt;      /*<! Number of Rx Descriptors */
  
  uint32_t VlanTag;         /*<! Vlan Tag value */
  
  uint32_t InnerVlanTag;    /*<! Inner Vlan Tag value */
  
  uint32_t Checksum;        /*<! Rx Checksum status. 
                                 This parameter can be a value of @ref ETH_Rx_Checksum_Status */
  
  uint32_t HeaderType;      /*<! IP header type. 
                                 This parameter can be a value of @ref ETH_Rx_IP_Header_Type */
  
  uint32_t PayloadType;     /*<! Payload type. 
                                 This parameter can be a value of @ref ETH_Rx_Payload_Type */
  
  uint32_t MacFilterStatus; /*<! MAC filter status.  
                                 This parameter can be a value of @ref ETH_Rx_MAC_Filter_Status */
  
  uint32_t L3FilterStatus;  /*<! L3 filter status
                                 This parameter can be a value of @ref ETH_Rx_L3_Filter_Status */
  
  uint32_t L4FilterStatus;  /*<! L4 filter status  
                                 This parameter can be a value of @ref ETH_Rx_L4_Filter_Status */
  
  uint32_t ErrorCode;       /*<! Rx error code  
                                 This parameter can be a combination of @ref ETH_Rx_Error_Code */

} ETH_RxPacketInfo;
/** 
  * 
  */

/** 
  * @brief  ETH MAC Configuration Structure definition  
  */
typedef struct
{
  uint32_t         SourceAddrControl;           /*!< Selects the Source Address Insertion or Replacement Control.                                                           
                                                     This parameter can be a value of @ref ETH_Source_Addr_Control */                  

  FunctionalState  ChecksumOffload;             /*!< Enables or Disable the checksum checking for received packet payloads TCP, UDP or ICMP headers */    

  uint32_t         InterPacketGapVal;           /*!< Sets the minimum IPG between Packet during transmission.
                                                     This parameter can be a value of @ref ETH_Inter_Packet_Gap */   

  FunctionalState  GiantPacketSizeLimitControl; /*!< Enables or disables the Giant Packet Size Limit Control. */  

  FunctionalState  Support2KPacket;             /*!< Enables or disables the IEEE 802.3as Support for 2K length Packets */ 

  FunctionalState  CRCStripTypePacket;          /*!< Enables or disables the CRC stripping for Type packets.*/ 

  FunctionalState  AutomaticPadCRCStrip;        /*!< Enables or disables  the Automatic MAC Pad/CRC Stripping.*/ 
																													 
  FunctionalState  Watchdog;                    /*!< Enables or disables the Watchdog timer on Rx path
                                                           When enabled, the MAC allows no more then 2048 bytes to be received.
                                                           When disabled, the MAC can receive up to 16384 bytes. */  

  FunctionalState  Jabber;                      /*!< Enables or disables Jabber timer on Tx path
                                                           When enabled, the MAC allows no more then 2048 bytes to be sent.
                                                           When disabled, the MAC can send up to 16384 bytes. */

  FunctionalState  JumboPacket;                 /*!< Enables or disables receiving Jumbo Packet
                                                           When enabled, the MAC allows jumbo packets of 9,018 bytes
                                                           without reporting a giant packet error */

  uint32_t         Speed;                       /*!< Sets the Ethernet speed: 10/100 Mbps.
                                                           This parameter can be a value of @ref ETH_Speed */

  uint32_t         DuplexMode;                  /*!< Selects the MAC duplex mode: Half-Duplex or Full-Duplex mode
                                                           This parameter can be a value of @ref ETH_Duplex_Mode */

  FunctionalState  LoopbackMode;                /*!< Enables or disables the loopback mode */

  FunctionalState  CarrierSenseBeforeTransmit;  /*!< Enables or disables the Carrier Sense Before Transmission in Full Duplex Mode. */

  FunctionalState  ReceiveOwn;                  /*!< Enables or disables the Receive Own in Half Duplex mode. */  

  FunctionalState  CarrierSenseDuringTransmit;  /*!< Enables or disables the Carrier Sense During Transmission in the Half Duplex mode */

  FunctionalState  RetryTransmission;           /*!< Enables or disables the MAC retry transmission, when a collision occurs in Half Duplex mode.*/

  uint32_t         BackOffLimit;                /*!< Selects the BackOff limit value.
                                                        This parameter can be a value of @ref ETH_Back_Off_Limit */

  FunctionalState  DeferralCheck;               /*!< Enables or disables the deferral check function in Half Duplex mode. */

  uint32_t         PreambleLength;              /*!< Selects or not the Preamble Length for Transmit packets (Full Duplex mode).
                                                           This parameter can be a value of @ref ETH_Preamble_Length */ 
                                                           
  FunctionalState  UnicastSlowProtocolPacketDetect;   /*!< Enable or disables the Detection of Slow Protocol Packets with unicast address. */   

  FunctionalState  SlowProtocolDetect;          /*!< Enable or disables the Slow Protocol Detection. */   

  FunctionalState  CRCCheckingRxPackets;        /*!< Enable or disables the CRC Checking for Received Packets. */   

  uint32_t         GiantPacketSizeLimit;        /*!< Specifies the packet size that the MAC will declare it as Giant, If it's size is 
	                                                  greater than the value programmed in this field in units of bytes 
                                                          This parameter must be a number between Min_Data = 0x618 (1518 byte) and Max_Data = 0x3FFF (32 Kbyte)*/                                                          
 
  FunctionalState  ExtendedInterPacketGap;      /*!< Enable or disables the extended inter packet gap. */ 
  
  uint32_t         ExtendedInterPacketGapVal;   /*!< Sets the Extended IPG between Packet during transmission.
                                                           This parameter can be a value from 0x0 to 0xFF */ 
  
  FunctionalState  ProgrammableWatchdog;        /*!< Enable or disables the Programmable Watchdog.*/
	
  uint32_t         WatchdogTimeout;             /*!< This field is used as watchdog timeout for a received packet
                                                        This parameter can be a value of @ref ETH_Watchdog_Timeout */                                                            

   uint32_t        PauseTime;                   /*!< This field holds the value to be used in the Pause Time field in the transmit control packet. 
                                                   This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFF */

  FunctionalState  ZeroQuantaPause;             /*!< Enable or disables the automatic generation of Zero Quanta Pause Control packets.*/  

  uint32_t         PauseLowThreshold;           /*!< This field configures the threshold of the PAUSE to be checked for automatic retransmission of PAUSE Packet.
                                                   This parameter can be a value of @ref ETH_Pause_Low_Threshold */

  FunctionalState  TransmitFlowControl;         /*!< Enables or disables the MAC to transmit Pause packets in Full Duplex mode
                                                   or the MAC back pressure operation in Half Duplex mode */

  FunctionalState  UnicastPausePacketDetect;    /*!< Enables or disables the MAC to detect Pause packets with unicast address of the station */
  
  FunctionalState  ReceiveFlowControl;          /*!< Enables or disables the MAC to decodes the received Pause packet  
                                                  and disables its transmitter for a specified (Pause) time */
																													                                                        	 
  uint32_t         TransmitQueueMode;           /*!< Specifies the Transmit Queue operating mode.
                                                      This parameter can be a value of @ref ETH_Transmit_Mode */ 

  uint32_t         ReceiveQueueMode;            /*!< Specifies the Receive Queue operating mode.
                                                             This parameter can be a value of @ref ETH_Receive_Mode */

  FunctionalState  DropTCPIPChecksumErrorPacket; /*!< Enables or disables Dropping of TCPIP Checksum Error Packets. */ 

  FunctionalState  ForwardRxErrorPacket;        /*!< Enables or disables  forwarding Error Packets. */ 

  FunctionalState  ForwardRxUndersizedGoodPacket;  /*!< Enables or disables  forwarding Undersized Good Packets.*/ 
} ETH_MACConfigTypeDef;
/** 
  * 
  */

/** 
  * @brief  ETH DMA Configuration Structure definition  
  */
 typedef struct
 {
   uint32_t        DMAArbitration;          /*!< Sets the arbitration scheme between DMA Tx and Rx
                                                         This parameter can be a value of @ref ETH_DMA_Arbitration */
      
   FunctionalState AddressAlignedBeats;     /*!< Enables or disables the AHB Master interface address aligned 
                                                            burst transfers on Read and Write channels  */
      
   uint32_t        BurstMode;               /*!< Sets the AHB Master interface burst transfers.
                                                     This parameter can be a value of @ref ETH_Burst_Mode */
      
   FunctionalState RebuildINCRxBurst;       /*!< Enables or disables the AHB Master to rebuild the pending beats 
                                                   of any initiated burst transfer with INCRx and SINGLE transfers. */
      
   FunctionalState PBLx8Mode;               /*!< Enables or disables the PBL multiplication by eight. */
      
   uint32_t        TxDMABurstLength;        /*!< Indicates the maximum number of beats to be transferred in one Tx DMA transaction.
                                                     This parameter can be a value of @ref ETH_Tx_DMA_Burst_Length */	
      
   FunctionalState SecondPacketOperate;     /*!< Enables or disables the Operate on second Packet mode, which allows the DMA to process a second
                                                      Packet of Transmit data even before obtaining the status for the first one. */	
      
   uint32_t        RxDMABurstLength;        /*!< Indicates the maximum number of beats to be transferred in one Rx DMA transaction.
                                                    This parameter can be a value of @ref ETH_Rx_DMA_Burst_Length */
      
   FunctionalState FlushRxPacket;           /*!< Enables or disables the Rx Packet Flush */
      
   FunctionalState TCPSegmentation;         /*!< Enables or disables the TCP Segmentation */
      
   uint32_t        MaximumSegmentSize;      /*!< Sets the maximum segment size that should be used while segmenting the packet
                                                  This parameter can be a value from 0x40 to 0x3FFF */    
} ETH_DMAConfigTypeDef;
/** 
  * 
  */

/** 
  * @brief  HAL ETH Media Interfaces enum definition  
  */ 
typedef enum
{
  HAL_ETH_MII_MODE             = 0x00U,   /*!<  Media Independent Interface               */
  HAL_ETH_RMII_MODE            = 0x01U    /*!<   Reduced Media Independent Interface       */
}ETH_MediaInterfaceTypeDef;
/** 
  * 
  */

/** 
  * @brief  ETH Init Structure definition  
  */
typedef struct
{
   
  uint8_t                     *MACAddr;                  /*!< MAC Address of used Hardware: must be pointer on an array of 6 bytes */
  	
  ETH_MediaInterfaceTypeDef   MediaInterface;            /*!< Selects the MII interface or the RMII interface. */

  ETH_DMADescTypeDef          *TxDesc;                   /*!< Provides the address of the first DMA Tx descriptor in the list */
  
  ETH_DMADescTypeDef          *RxDesc;                   /*!< Provides the address of the first DMA Rx descriptor in the list */
  
  uint32_t                    RxBuffLen;                 /*!< Provides the length of Rx buffers size */

}ETH_InitTypeDef;
/** 
  * 
  */

/** 
  * @brief  HAL State structures definition  
  */ 
typedef uint32_t HAL_ETH_StateTypeDef;
/** 
  * 
  */

/** 
  * @brief  ETH Handle Structure definition  
  */
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
typedef struct __ETH_HandleTypeDef
#else
typedef struct
#endif
{
  ETH_TypeDef                *Instance;                 /*!< Register base address       */
  
  ETH_InitTypeDef            Init;                      /*!< Ethernet Init Configuration */
	
  ETH_TxDescListTypeDef      TxDescList;                /*!< Tx descriptor wrapper: holds all Tx descriptors list 
                                                            addresses and current descriptor index  */
		
  ETH_RxDescListTypeDef      RxDescList;                /*!< Rx descriptor wrapper: holds all Rx descriptors list 
                                                            addresses and current descriptor index  */ 
  
  HAL_LockTypeDef            Lock;                      /*!< Locking object             */
 
  __IO HAL_ETH_StateTypeDef  gState;                   /*!< ETH state information related to global Handle management 
                                                              and also related to Tx operations.
                                                             This parameter can be a value of @ref HAL_ETH_StateTypeDef */
  
  __IO HAL_ETH_StateTypeDef  RxState;                   /*!< ETH state information related to Rx operations.
                                                             This parameter can be a value of @ref HAL_ETH_StateTypeDef */
  
  __IO uint32_t              ErrorCode;                 /*!< Holds the global Error code of the ETH HAL status machine 
                                                             This parameter can be a value of of @ref ETH_Error_Code */
  
  __IO uint32_t              DMAErrorCode;              /*!< Holds the DMA Rx Tx Error code when a DMA AIS interrupt occurs
                                                             This parameter can be a combination of @ref ETH_DMA_Status_Flags */
  
  __IO uint32_t              MACErrorCode;              /*!< Holds the MAC Rx Tx Error code when a MAC Rx or Tx status interrupt occurs
                                                             This parameter can be a combination of @ref ETH_MAC_Rx_Tx_Status */ 
    
  __IO uint32_t              MACWakeUpEvent;            /*!< Holds the Wake Up event when the MAC exit the power down mode
                                                             This parameter can be a value of @ref ETH_MAC_Wake_Up_Event */
    
  __IO uint32_t              MACLPIEvent;               /*!< Holds the LPI event when the an LPI status interrupt occurs.
                                                             This parameter can be a value of @ref ETHEx_LPI_Event */
                                                             
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)

  void    (* TxCpltCallback)     ( struct __ETH_HandleTypeDef * heth);   /*!< ETH Tx Complete Callback */
  void    (* RxCpltCallback)     ( struct __ETH_HandleTypeDef * heth);  /*!< ETH Rx  Complete Callback     */
  void    (* DMAErrorCallback)   ( struct __ETH_HandleTypeDef * heth);  /*!< ETH DMA Error Callback   */
  void    (* MACErrorCallback)   ( struct __ETH_HandleTypeDef * heth);  /*!< ETH MAC Error Callback     */
  void    (* PMTCallback)        ( struct __ETH_HandleTypeDef * heth);  /*!< ETH Power Management Callback            */
  void    (* EEECallback)        ( struct __ETH_HandleTypeDef * heth);  /*!< ETH EEE Callback   */
  void    (* WakeUpCallback)     ( struct __ETH_HandleTypeDef * heth);  /*!< ETH Wake UP Callback   */

  void    (* MspInitCallback)    ( struct __ETH_HandleTypeDef * heth);    /*!< ETH Msp Init callback              */
  void    (* MspDeInitCallback)  ( struct __ETH_HandleTypeDef * heth);    /*!< ETH Msp DeInit callback            */

#endif  /* USE_HAL_ETH_REGISTER_CALLBACKS */                                                             

} ETH_HandleTypeDef;
/** 
  * 
  */
  
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL ETH Callback ID enumeration definition
  */
typedef enum
{
  HAL_ETH_MSPINIT_CB_ID            = 0x00U,    /*!< ETH MspInit callback ID           */
  HAL_ETH_MSPDEINIT_CB_ID          = 0x01U,    /*!< ETH MspDeInit callback ID         */
	
  HAL_ETH_TX_COMPLETE_CB_ID        = 0x02U,    /*!< ETH Tx Complete Callback ID       */
  HAL_ETH_RX_COMPLETE_CB_ID        = 0x03U,    /*!< ETH Rx Complete Callback ID       */
  HAL_ETH_DMA_ERROR_CB_ID          = 0x04U,    /*!< ETH DMA Error Callback ID         */
  HAL_ETH_MAC_ERROR_CB_ID          = 0x05U,    /*!< ETH MAC Error Callback ID         */
  HAL_ETH_PMT_CB_ID                = 0x06U,     /*!< ETH Power Management Callback ID  */
  HAL_ETH_EEE_CB_ID                = 0x07U,     /*!< ETH EEE Callback ID               */
  HAL_ETH_WAKEUP_CB_ID             = 0x08U     /*!< ETH Wake UP Callback ID           */


}HAL_ETH_CallbackIDTypeDef;

/**
  * @brief  HAL ETH Callback pointer definition
  */
typedef  void (*pETH_CallbackTypeDef)(ETH_HandleTypeDef * heth); /*!< pointer to an ETH callback function */

#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */

/** 
  * @brief  ETH MAC filter structure definition
  */
typedef struct{
  FunctionalState PromiscuousMode;          /*!< Enable or Disable Promiscuous Mode */
  
  FunctionalState ReceiveAllMode;           /*!< Enable or Disable Receive All Mode */
  
  FunctionalState HachOrPerfectFilter;      /*!< Enable or Disable Perfect filtering in addition to Hash filtering */
  
  FunctionalState HashUnicast;              /*!< Enable or Disable Hash filtering on unicast packets */
  
  FunctionalState HashMulticast;            /*!< Enable or Disable Hash filtering on multicast packets */
  
  FunctionalState PassAllMulticast;         /*!< Enable or Disable passing all multicast packets */
  
  FunctionalState SrcAddrFiltering;         /*!< Enable or Disable source address filtering module */
  
  FunctionalState SrcAddrInverseFiltering;  /*!< Enable or Disable source address inverse filtering */
  
  FunctionalState DestAddrInverseFiltering; /*!< Enable or Disable destination address inverse filtering */
  
  FunctionalState BroadcastFilter;          /*!< Enable or Disable broadcast filter */
  
  uint32_t        ControlPacketsFilter;     /*!< Set the control packets filter
                                                 This parameter can be a value of @ref ETH_Control_Packets_Filter */
}ETH_MACFilterConfigTypeDef;
/** 
  * 
  */
    
/** 
  * @brief  ETH Power Down structure definition
  */
typedef struct{
  FunctionalState WakeUpPacket;    /*!< Enable or Disable Wake up packet detection in power down mode */
  
  FunctionalState MagicPacket;     /*!< Enable or Disable Magic packet detection in power down mode */
  
  FunctionalState GlobalUnicast;    /*!< Enable or Disable Global unicast packet detection in power down mode */
  
  FunctionalState WakeUpForward;    /*!< Enable or Disable Forwarding Wake up packets */
  	
}ETH_PowerDownConfigTypeDef;
/** 
  * 
  */
  
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ETH_Exported_Constants ETH Exported Constants
  * @{
  */

/** @defgroup ETH_DMA_Tx_Descriptor_Bit_Definition ETH DMA Tx Descriptor Bit Definition
  * @{
  */

/*
   DMA Tx Normal Desciptor Read Format
  -----------------------------------------------------------------------------------------------
  TDES0 |                         Buffer1 or Header Address  [31:0]                              |
  -----------------------------------------------------------------------------------------------
  TDES1 |                   Buffer2 Address [31:0] / Next Descriptor Address [31:0]              |
  -----------------------------------------------------------------------------------------------
  TDES2 | IOC(31) | TTSE(30) | Buff2 Length[29:16] | VTIR[15:14] | Header or Buff1 Length[13:0]  |
  -----------------------------------------------------------------------------------------------
  TDES3 | OWN(31) | CTRL[30:26] | Reserved[25:24] | CTRL[23:20] | Reserved[19:17] | Status[16:0] |
  -----------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of TDES0 RF register
  */ 
#define ETH_DMATXNDESCRF_B1AP  ((uint32_t)0xFFFFFFFFU)  /*!< Transmit Packet Timestamp Low */

/** 
  * @brief  Bit definition of TDES1 RF register
  */ 
#define ETH_DMATXNDESCRF_B2AP  ((uint32_t)0xFFFFFFFFU)  /*!< Transmit Packet Timestamp High */

/** 
  * @brief  Bit definition of TDES2 RF register
  */ 
#define ETH_DMATXNDESCRF_IOC          ((uint32_t)0x80000000U)  /*!< Interrupt on Completion */
#define ETH_DMATXNDESCRF_TTSE         ((uint32_t)0x40000000U)  /*!< Transmit Timestamp Enable */
#define ETH_DMATXNDESCRF_B2L          ((uint32_t)0x3FFF0000U)  /*!< Buffer 2 Length */
#define ETH_DMATXNDESCRF_VTIR         ((uint32_t)0x0000C000U)  /*!< VLAN Tag Insertion or Replacement mask */
#define ETH_DMATXNDESCRF_VTIR_DISABLE ((uint32_t)0x00000000U)  /*!< Do not add a VLAN tag. */
#define ETH_DMATXNDESCRF_VTIR_REMOVE  ((uint32_t)0x00004000U)  /*!< Remove the VLAN tag from the packets before transmission. */
#define ETH_DMATXNDESCRF_VTIR_INSERT  ((uint32_t)0x00008000U)  /*!< Insert a VLAN tag. */
#define ETH_DMATXNDESCRF_VTIR_REPLACE ((uint32_t)0x0000C000U)  /*!< Replace the VLAN tag. */
#define ETH_DMATXNDESCRF_B1L          ((uint32_t)0x00003FFFU)  /*!< Buffer 1 Length */
#define ETH_DMATXNDESCRF_HL           ((uint32_t)0x000003FFU)  /*!< Header Length */

/** 
  * @brief  Bit definition of TDES3 RF register
  */ 
#define ETH_DMATXNDESCRF_OWN                                 ((uint32_t)0x80000000U)  /*!< OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATXNDESCRF_CTXT                                ((uint32_t)0x40000000U)  /*!< Context Type */
#define ETH_DMATXNDESCRF_FD                                  ((uint32_t)0x20000000U)  /*!< First Descriptor */
#define ETH_DMATXNDESCRF_LD                                  ((uint32_t)0x10000000U)  /*!< Last Descriptor */
#define ETH_DMATXNDESCRF_CPC                                 ((uint32_t)0x0C000000U)  /*!< CRC Pad Control mask */
#define ETH_DMATXNDESCRF_CPC_CRCPAD_INSERT                   ((uint32_t)0x00000000U)  /*!< CRC Pad Control: CRC and Pad Insertion */
#define ETH_DMATXNDESCRF_CPC_CRC_INSERT                      ((uint32_t)0x04000000U)  /*!< CRC Pad Control: CRC Insertion (Disable Pad Insertion) */
#define ETH_DMATXNDESCRF_CPC_DISABLE                         ((uint32_t)0x08000000U)  /*!< CRC Pad Control: Disable CRC Insertion */
#define ETH_DMATXNDESCRF_CPC_CRC_REPLACE                     ((uint32_t)0x0C000000U)  /*!< CRC Pad Control: CRC Replacement */
#define ETH_DMATXNDESCRF_SAIC                                ((uint32_t)0x03800000U)  /*!< SA Insertion Control mask*/
#define ETH_DMATXNDESCRF_SAIC_DISABLE                        ((uint32_t)0x00000000U)  /*!< SA Insertion Control: Do not include the source address */
#define ETH_DMATXNDESCRF_SAIC_INSERT                         ((uint32_t)0x00800000U)  /*!< SA Insertion Control: Include or insert the source address */
#define ETH_DMATXNDESCRF_SAIC_REPLACE                        ((uint32_t)0x01000000U)  /*!< SA Insertion Control: Replace the source address */
#define ETH_DMATXNDESCRF_THL                                 ((uint32_t)0x00780000U)  /*!< TCP Header Length */
#define ETH_DMATXNDESCRF_TSE                                 ((uint32_t)0x00040000U)  /*!< TCP segmentation enable */
#define ETH_DMATXNDESCRF_CIC                                 ((uint32_t)0x00030000U)  /*!< Checksum Insertion Control: 4 cases */ 
#define ETH_DMATXNDESCRF_CIC_DISABLE                         ((uint32_t)0x00000000U)  /*!< Do Nothing: Checksum Engine is disabled */ 
#define ETH_DMATXNDESCRF_CIC_IPHDR_INSERT                    ((uint32_t)0x00010000U)  /*!< Only IP header checksum calculation and insertion are enabled. */ 
#define ETH_DMATXNDESCRF_CIC_IPHDR_PAYLOAD_INSERT            ((uint32_t)0x00020000U)  /*!< IP header checksum and payload checksum calculation and insertion are
                                                                                          enabled, but pseudo header checksum is not calculated in hardware */ 
#define ETH_DMATXNDESCRF_CIC_IPHDR_PAYLOAD_INSERT_PHDR_CALC  ((uint32_t)0x00030000U)  /*!< IP Header checksum and payload checksum calculation and insertion are
                                                                                          enabled, and pseudo header checksum is calculated in hardware. */ 
#define ETH_DMATXNDESCRF_TPL                                 ((uint32_t)0x0003FFFFU)  /*!< TCP Payload Length */
#define ETH_DMATXNDESCRF_FL                                  ((uint32_t)0x00007FFFU)  /*!< Transmit End of Ring */

/*
   DMA Tx Normal Descriptor Write Back Format
  -----------------------------------------------------------------------------------------------
  TDES0 |                         Timestamp Low                                                  |
  -----------------------------------------------------------------------------------------------
  TDES1 |                         Timestamp High                                                 |
  -----------------------------------------------------------------------------------------------
  TDES2 |                           Reserved[31:0]                                               |
  -----------------------------------------------------------------------------------------------
  TDES3 | OWN(31) |                          Status[30:0]                                        |
  -----------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of TDES0 WBF register
  */ 
#define ETH_DMATXNDESCWBF_TTSL  ((uint32_t)0xFFFFFFFFU)  /*!< Buffer1 Address Pointer or TSO Header Address Pointer */

/** 
  * @brief  Bit definition of TDES1 WBF register
  */ 
#define ETH_DMATXNDESCWBF_TTSH  ((uint32_t)0xFFFFFFFFU)  /*!< Buffer2 Address Pointer */

/** 
  * @brief  Bit definition of TDES3 WBF register
  */ 
#define ETH_DMATXNDESCWBF_OWN                     ((uint32_t)0x80000000U)  /*!< OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATXNDESCWBF_CTXT                    ((uint32_t)0x40000000U)  /*!< Context Type */
#define ETH_DMATXNDESCWBF_FD                      ((uint32_t)0x20000000U)  /*!< First Descriptor */
#define ETH_DMATXNDESCWBF_LD                      ((uint32_t)0x10000000U)  /*!< Last Descriptor */
#define ETH_DMATXNDESCWBF_TTSS                    ((uint32_t)0x00020000U)  /*!< Tx Timestamp Status */
#define ETH_DMATXNDESCWBF_DP                      ((uint32_t)0x04000000U)  /*!< Disable Padding */
#define ETH_DMATXNDESCWBF_TTSE                    ((uint32_t)0x02000000U)  /*!< Transmit Timestamp Enable */
#define ETH_DMATXNDESCWBF_ES                      ((uint32_t)0x00008000U)  /*!< Error summary: OR of the following bits: IHE || UF || ED || EC || LCO || PCE || NC || LCA || FF || JT */
#define ETH_DMATXNDESCWBF_JT                      ((uint32_t)0x00004000U)  /*!< Jabber Timeout */
#define ETH_DMATXNDESCWBF_FF                      ((uint32_t)0x00002000U)  /*!< Packet Flushed: DMA/MTL flushed the packet due to SW flush */
#define ETH_DMATXNDESCWBF_PCE                     ((uint32_t)0x00001000U)  /*!< Payload Checksum Error */
#define ETH_DMATXNDESCWBF_LCA                     ((uint32_t)0x00000800U)  /*!< Loss of Carrier: carrier lost during transmission */
#define ETH_DMATXNDESCWBF_NC                      ((uint32_t)0x00000400U)  /*!< No Carrier: no carrier signal from the transceiver */
#define ETH_DMATXNDESCWBF_LCO                     ((uint32_t)0x00000200U)  /*!< Late Collision: transmission aborted due to collision */
#define ETH_DMATXNDESCWBF_EC                      ((uint32_t)0x00000100U)  /*!< Excessive Collision: transmission aborted after 16 collisions */
#define ETH_DMATXNDESCWBF_CC                      ((uint32_t)0x000000F0U)  /*!< Collision Count */
#define ETH_DMATXNDESCWBF_ED                      ((uint32_t)0x00000008U)  /*!< Excessive Deferral */
#define ETH_DMATXNDESCWBF_UF                      ((uint32_t)0x00000004U)  /*!< Underflow Error: late data arrival from the memory */
#define ETH_DMATXNDESCWBF_DB                      ((uint32_t)0x00000002U)  /*!< Deferred Bit */
#define ETH_DMATXNDESCWBF_IHE                     ((uint32_t)0x00000004U)  /*!< IP Header Error */


/*
   DMA Tx Context Desciptor
  -----------------------------------------------------------------------------------------------
  TDES0 |                               Timestamp Low                                            |
  -----------------------------------------------------------------------------------------------
  TDES1 |                               Timestamp High                                           |
  -----------------------------------------------------------------------------------------------
  TDES2 |      Inner VLAN Tag[31:16]    | Reserved(15) |     Maximum Segment Size [14:0]         |
  -----------------------------------------------------------------------------------------------
  TDES3 | OWN(31) |                          Status[30:0]                                        |
  -----------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of Tx context descriptor register 0
  */ 
#define ETH_DMATXCDESC_TTSL  ((uint32_t)0xFFFFFFFFU)  /*!< Transmit Packet Timestamp Low */

/** 
  * @brief  Bit definition of Tx context descriptor register 1
  */ 
#define ETH_DMATXCDESC_TTSH  ((uint32_t)0xFFFFFFFFU)  /*!< Transmit Packet Timestamp High */

/** 
  * @brief  Bit definition of Tx context descriptor register 2
  */ 
#define ETH_DMATXCDESC_IVT   ((uint32_t)0xFFFF0000U)  /*!< Inner VLAN Tag */
#define ETH_DMATXCDESC_MSS   ((uint32_t)0x00003FFFU)  /*!< Maximum Segment Size */

/** 
  * @brief  Bit definition of Tx context descriptor register 3
  */
#define ETH_DMATXCDESC_OWN                     ((uint32_t)0x80000000U)     /*!< OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATXCDESC_CTXT                    ((uint32_t)0x40000000U)     /*!< Context Type */
#define ETH_DMATXCDESC_OSTC                    ((uint32_t)0x08000000U)     /*!< One-Step Timestamp Correction Enable */
#define ETH_DMATXCDESC_TCMSSV                  ((uint32_t)0x04000000U)     /*!< One-Step Timestamp Correction Input or MSS Valid */
#define ETH_DMATXCDESC_CDE                     ((uint32_t)0x00800000U)     /*!< Context Descriptor Error */
#define ETH_DMATXCDESC_IVTIR                   ((uint32_t)0x000C0000U)     /*!< Inner VLAN Tag Insert or Replace Mask */
#define ETH_DMATXCDESC_IVTIR_DISABLE           ((uint32_t)0x00000000U)     /*!< Do not add the inner VLAN tag. */
#define ETH_DMATXCDESC_IVTIR_REMOVE            ((uint32_t)0x00040000U)     /*!< Remove the inner VLAN tag from the packets before transmission. */
#define ETH_DMATXCDESC_IVTIR_INSERT            ((uint32_t)0x00080000U)     /*!< Insert the inner VLAN tag. */
#define ETH_DMATXCDESC_IVTIR_REPLACE           ((uint32_t)0x000C0000U)     /*!< Replace the inner VLAN tag. */
#define ETH_DMATXCDESC_IVLTV                   ((uint32_t)0x00020000U)     /*!< Inner VLAN Tag Valid */
#define ETH_DMATXCDESC_VLTV                    ((uint32_t)0x00010000U)     /*!< VLAN Tag Valid */
#define ETH_DMATXCDESC_VT                      ((uint32_t)0x0000FFFFU)     /*!< VLAN Tag */

/**
  * @}
  */ 


/** @defgroup ETH_DMA_Rx_Descriptor_Bit_Definition ETH DMA Rx Descriptor Bit Definition
  * @{
  */

/*
  DMA Rx Normal Descriptor read format
  -----------------------------------------------------------------------------------------------------------
  RDES0 |                                  Buffer1 or Header Address [31:0]                                 |
  -----------------------------------------------------------------------------------------------------------
  RDES1 |                                            Reserved                                               |
  -----------------------------------------------------------------------------------------------------------
  RDES2 |                                      Payload or Buffer2 Address[31:0]                             |
  -----------------------------------------------------------------------------------------------------------
  RDES3 | OWN(31) | IOC(30) | Reserved [29:26] | BUF2V(25) | BUF1V(24) |           Reserved [23:0]          |
  -----------------------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of Rx normal descriptor register 0 read format
  */
#define ETH_DMARXNDESCRF_BUF1AP        ((uint32_t)0xFFFFFFFFU)  /*!< Header or Buffer 1 Address Pointer  */

/** 
  * @brief  Bit definition of Rx normal descriptor register 2 read format
  */
#define ETH_DMARXNDESCRF_BUF2AP        ((uint32_t)0xFFFFFFFFU)  /*!< Buffer 2 Address Pointer  */

/** 
  * @brief  Bit definition of Rx normal descriptor register 3 read format
  */
#define ETH_DMARXNDESCRF_OWN         ((uint32_t)0x80000000U)  /*!< OWN bit: descriptor is owned by DMA engine  */
#define ETH_DMARXNDESCRF_IOC         ((uint32_t)0x40000000U)  /*!< Interrupt Enabled on Completion  */
#define ETH_DMARXNDESCRF_BUF2V       ((uint32_t)0x02000000U)  /*!< Buffer 2 Address Valid */
#define ETH_DMARXNDESCRF_BUF1V       ((uint32_t)0x01000000U)  /*!< Buffer 1 Address Valid */

/*
  DMA Rx Normal Descriptor write back format
  ---------------------------------------------------------------------------------------------------------------------
  RDES0 |                 Inner VLAN Tag[31:16]                 |                 Outer VLAN Tag[15:0]                | 
	---------------------------------------------------------------------------------------------------------------------
  RDES1 |       OAM code, or MAC Control Opcode [31:16]         |               Extended Status                       |
  ---------------------------------------------------------------------------------------------------------------------
  RDES2 |      MAC Filter Status[31:16]        | VF(15) | Reserved [14:12] | ARP Status [11:10] | Header Length [9:0] |
  ---------------------------------------------------------------------------------------------------------------------
  RDES3 | OWN(31) | CTXT(30) |  FD(29) | LD(28) |   Status[27:16]     | ES(15) |        Packet Length[14:0]           |
  ---------------------------------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of Rx normal descriptor register 0 write back format
  */
#define ETH_DMARXNDESCWBF_IVT        ((uint32_t)0xFFFF0000U)  /*!< Inner VLAN Tag  */
#define ETH_DMARXNDESCWBF_OVT        ((uint32_t)0x0000FFFFU)  /*!< Outer VLAN Tag  */

/** 
  * @brief  Bit definition of Rx normal descriptor register 1 write back format
  */
#define ETH_DMARXNDESCWBF_OPC             ((uint32_t)0xFFFF0000U)  /*!< OAM Sub-Type Code, or MAC Control Packet opcode  */
#define ETH_DMARXNDESCWBF_TD              ((uint32_t)0x00008000U)  /*!< Timestamp Dropped  */
#define ETH_DMARXNDESCWBF_TSA             ((uint32_t)0x00004000U)  /*!< Timestamp Available  */
#define ETH_DMARXNDESCWBF_PV              ((uint32_t)0x00002000U)  /*!< PTP Version  */
#define ETH_DMARXNDESCWBF_PFT             ((uint32_t)0x00001000U)  /*!< PTP Packet Type  */
#define ETH_DMARXNDESCWBF_PMT_NO          ((uint32_t)0x00000000U)  /*!< PTP Message Type: No PTP message received  */
#define ETH_DMARXNDESCWBF_PMT_SYNC        ((uint32_t)0x00000100U)  /*!< PTP Message Type: SYNC (all clock types)  */
#define ETH_DMARXNDESCWBF_PMT_FUP         ((uint32_t)0x00000200U)  /*!< PTP Message Type: Follow_Up (all clock types)  */
#define ETH_DMARXNDESCWBF_PMT_DREQ        ((uint32_t)0x00000300U)  /*!< PTP Message Type: Delay_Req (all clock types)  */
#define ETH_DMARXNDESCWBF_PMT_DRESP       ((uint32_t)0x00000400U)  /*!< PTP Message Type: Delay_Resp (all clock types)  */
#define ETH_DMARXNDESCWBF_PMT_PDREQ       ((uint32_t)0x00000500U)  /*!< PTP Message Type: Pdelay_Req (in peer-to-peer transparent clock)  */
#define ETH_DMARXNDESCWBF_PMT_PDRESP      ((uint32_t)0x00000600U)  /*!< PTP Message Type: Pdelay_Resp (in peer-to-peer transparent clock)  */
#define ETH_DMARXNDESCWBF_PMT_PDRESPFUP   ((uint32_t)0x00000700U)  /*!< PTP Message Type: Pdelay_Resp_Follow_Up (in peer-to-peer transparent clock)  */
#define ETH_DMARXNDESCWBF_PMT_ANNOUNCE    ((uint32_t)0x00000800U)  /*!< PTP Message Type: Announce  */
#define ETH_DMARXNDESCWBF_PMT_MANAG       ((uint32_t)0x00000900U)  /*!< PTP Message Type: Management  */
#define ETH_DMARXNDESCWBF_PMT_SIGN        ((uint32_t)0x00000A00U)  /*!< PTP Message Type: Signaling  */
#define ETH_DMARXNDESCWBF_PMT_RESERVED    ((uint32_t)0x00000F00U)  /*!< PTP Message Type: PTP packet with Reserved message type  */
#define ETH_DMARXNDESCWBF_IPCE            ((uint32_t)0x00000080U)  /*!< IP Payload Error */
#define ETH_DMARXNDESCWBF_IPCB            ((uint32_t)0x00000040U)  /*!< IP Checksum Bypassed */
#define ETH_DMARXNDESCWBF_IPV6            ((uint32_t)0x00000020U)  /*!< IPv6 header Present */
#define ETH_DMARXNDESCWBF_IPV4            ((uint32_t)0x00000010U)  /*!< IPv4 header Present */
#define ETH_DMARXNDESCWBF_IPHE            ((uint32_t)0x00000008U)  /*!< IP Header Error */
#define ETH_DMARXNDESCWBF_PT              ((uint32_t)0x00000003U)  /*!< Payload Type mask */
#define ETH_DMARXNDESCWBF_PT_UNKNOWN      ((uint32_t)0x00000000U)  /*!< Payload Type: Unknown type or IP/AV payload not processed */
#define ETH_DMARXNDESCWBF_PT_UDP          ((uint32_t)0x00000001U)  /*!< Payload Type: UDP */
#define ETH_DMARXNDESCWBF_PT_TCP          ((uint32_t)0x00000002U)  /*!< Payload Type: TCP  */
#define ETH_DMARXNDESCWBF_PT_ICMP         ((uint32_t)0x00000003U)  /*!< Payload Type: ICMP */

/** 
  * @brief  Bit definition of Rx normal descriptor register 2 write back format
  */
#define ETH_DMARXNDESCWBF_L3L4FM          ((uint32_t)0x20000000U)  /*!< L3 and L4 Filter Number Matched: if reset filter 0 is matched , if set filter 1 is matched */
#define ETH_DMARXNDESCWBF_L4FM            ((uint32_t)0x10000000U)  /*!< Layer 4 Filter Match                  */
#define ETH_DMARXNDESCWBF_L3FM            ((uint32_t)0x08000000U)  /*!< Layer 3 Filter Match                  */
#define ETH_DMARXNDESCWBF_MADRM           ((uint32_t)0x07F80000U)  /*!< MAC Address Match or Hash Value       */
#define ETH_DMARXNDESCWBF_HF              ((uint32_t)0x00040000U)  /*!< Hash Filter Status                    */
#define ETH_DMARXNDESCWBF_DAF             ((uint32_t)0x00020000U)  /*!< Destination Address Filter Fail       */
#define ETH_DMARXNDESCWBF_SAF             ((uint32_t)0x00010000U)  /*!< SA Address Filter Fail                */
#define ETH_DMARXNDESCWBF_VF              ((uint32_t)0x00008000U)  /*!< VLAN Filter Status                    */
#define ETH_DMARXNDESCWBF_ARPNR           ((uint32_t)0x00000400U)  /*!< ARP Reply Not Generated               */


/** 
  * @brief  Bit definition of Rx normal descriptor register 3 write back format
  */
#define ETH_DMARXNDESCWBF_OWN        ((uint32_t)0x80000000U)  /*!< Own Bit */
#define ETH_DMARXNDESCWBF_CTXT       ((uint32_t)0x40000000U)  /*!< Receive Context Descriptor */
#define ETH_DMARXNDESCWBF_FD         ((uint32_t)0x20000000U)  /*!< First Descriptor */
#define ETH_DMARXNDESCWBF_LD         ((uint32_t)0x10000000U)  /*!< Last Descriptor */
#define ETH_DMARXNDESCWBF_RS2V       ((uint32_t)0x08000000U)  /*!< Receive Status RDES2 Valid */
#define ETH_DMARXNDESCWBF_RS1V       ((uint32_t)0x04000000U)  /*!< Receive Status RDES1 Valid */
#define ETH_DMARXNDESCWBF_RS0V       ((uint32_t)0x02000000U)  /*!< Receive Status RDES0 Valid */
#define ETH_DMARXNDESCWBF_CE         ((uint32_t)0x01000000U)  /*!< CRC Error */
#define ETH_DMARXNDESCWBF_GP         ((uint32_t)0x00800000U)  /*!< Giant Packet */
#define ETH_DMARXNDESCWBF_RWT        ((uint32_t)0x00400000U)  /*!< Receive Watchdog Timeout */
#define ETH_DMARXNDESCWBF_OE         ((uint32_t)0x00200000U)  /*!< Overflow Error */
#define ETH_DMARXNDESCWBF_RE         ((uint32_t)0x00100000U)  /*!< Receive Error */
#define ETH_DMARXNDESCWBF_DE         ((uint32_t)0x00080000U)  /*!< Dribble Bit Error */
#define ETH_DMARXNDESCWBF_LT         ((uint32_t)0x00070000U)  /*!< Length/Type Field */
#define ETH_DMARXNDESCWBF_LT_LP      ((uint32_t)0x00000000U)  /*!< The packet is a length packet */
#define ETH_DMARXNDESCWBF_LT_TP      ((uint32_t)0x00010000U)  /*!< The packet is a type packet */
#define ETH_DMARXNDESCWBF_LT_ARP     ((uint32_t)0x00030000U)  /*!< The packet is a ARP Request packet type */
#define ETH_DMARXNDESCWBF_LT_VLAN    ((uint32_t)0x00040000U)  /*!< The packet is a type packet with VLAN Tag */
#define ETH_DMARXNDESCWBF_LT_DVLAN   ((uint32_t)0x00050000U)  /*!< The packet is a type packet with Double VLAN Tag */
#define ETH_DMARXNDESCWBF_LT_MAC     ((uint32_t)0x00060000U)  /*!< The packet is a MAC Control packet type */
#define ETH_DMARXNDESCWBF_LT_OAM     ((uint32_t)0x00070000U)  /*!< The packet is a OAM packet type */
#define ETH_DMARXNDESCWBF_ES         ((uint32_t)0x00008000U)  /*!< Error Summary */
#define ETH_DMARXNDESCWBF_PL         ((uint32_t)0x00007FFFU)  /*!< Packet Length */

/*
  DMA Rx context Descriptor
  ---------------------------------------------------------------------------------------------------------------------
  RDES0 |                                     Timestamp Low[31:0]                                                     | 
	---------------------------------------------------------------------------------------------------------------------
  RDES1 |                                     Timestamp High[31:0]                                                    |
  ---------------------------------------------------------------------------------------------------------------------
  RDES2 |                                          Reserved                                                           |
  ---------------------------------------------------------------------------------------------------------------------
  RDES3 | OWN(31) | CTXT(30) |                                Reserved[29:0]                                          |
  ---------------------------------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of Rx context descriptor register 0 
  */
#define ETH_DMARXCDESC_RTSL        ((uint32_t)0xFFFFFFFFU)  /*!< Receive Packet Timestamp Low  */

/** 
  * @brief  Bit definition of Rx context descriptor register 1
  */
#define ETH_DMARXCDESC_RTSH        ((uint32_t)0xFFFFFFFFU)  /*!< Receive Packet Timestamp High  */

/** 
  * @brief  Bit definition of Rx context descriptor register 3
  */
#define ETH_DMARXCDESC_OWN        ((uint32_t)0x80000000U)  /*!< Own Bit  */
#define ETH_DMARXCDESC_CTXT       ((uint32_t)0x40000000U)  /*!< Receive Context Descriptor  */

/**
  * @}
  */

/** @defgroup ETH_Frame_settings ETH frame settings
  * @{
  */ 
#define ETH_MAX_PACKET_SIZE      ((uint32_t)1528U)    /*!< ETH_HEADER + 2*VLAN_TAG + MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER               ((uint32_t)14U)    /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC                  ((uint32_t)4U)    /*!< Ethernet CRC */
#define ETH_VLAN_TAG             ((uint32_t)4U)    /*!< optional 802.1q VLAN Tag */
#define ETH_MIN_PAYLOAD          ((uint32_t)46U)    /*!< Minimum Ethernet payload size */
#define ETH_MAX_PAYLOAD          ((uint32_t)1500U)    /*!< Maximum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD  ((uint32_t)9000U)    /*!< Jumbo frame payload size */
/**
  * @}
  */

/** @defgroup ETH_Error_Code ETH Error Code
  * @{
  */ 
#define HAL_ETH_ERROR_NONE         ((uint32_t)0x00000000U)   /*!< No error            */
#define HAL_ETH_ERROR_PARAM        ((uint32_t)0x00000001U)   /*!< Busy error          */
#define HAL_ETH_ERROR_BUSY         ((uint32_t)0x00000002U)   /*!< Parameter error     */
#define HAL_ETH_ERROR_TIMEOUT      ((uint32_t)0x00000004U)   /*!< Timeout error       */
#define HAL_ETH_ERROR_DMA          ((uint32_t)0x00000008U)   /*!< DMA transfer error  */
#define HAL_ETH_ERROR_MAC          ((uint32_t)0x00000010U)   /*!< MAC transfer error  */
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
#define HAL_ETH_ERROR_INVALID_CALLBACK ((uint32_t)0x00000020U)    /*!< Invalid Callback error  */
#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup ETH_Tx_Packet_Attributes ETH Tx Packet Attributes
  * @{
  */
#define ETH_TX_PACKETS_FEATURES_CSUM          ((uint32_t)0x00000001U)
#define ETH_TX_PACKETS_FEATURES_SAIC          ((uint32_t)0x00000002U)
#define ETH_TX_PACKETS_FEATURES_VLANTAG       ((uint32_t)0x00000004U)
#define ETH_TX_PACKETS_FEATURES_INNERVLANTAG  ((uint32_t)0x00000008U)
#define ETH_TX_PACKETS_FEATURES_TSO           ((uint32_t)0x00000010U)
#define ETH_TX_PACKETS_FEATURES_CRCPAD        ((uint32_t)0x00000020U)
/**
  * @}
  */

/** @defgroup ETH_Tx_Packet_Source_Addr_Control ETH Tx Packet Source Addr Control
  * @{
  */
#define ETH_SRC_ADDR_CONTROL_DISABLE          ETH_DMATXNDESCRF_SAIC_DISABLE
#define ETH_SRC_ADDR_INSERT                   ETH_DMATXNDESCRF_SAIC_INSERT
#define ETH_SRC_ADDR_REPLACE                  ETH_DMATXNDESCRF_SAIC_REPLACE
/**
  * @}
  */

/** @defgroup ETH_Tx_Packet_CRC_Pad_Control ETH Tx Packet CRC Pad Control
  * @{
  */
#define ETH_CRC_PAD_DISABLE      ETH_DMATXNDESCRF_CPC_DISABLE
#define ETH_CRC_PAD_INSERT       ETH_DMATXNDESCRF_CPC_CRCPAD_INSERT
#define ETH_CRC_INSERT           ETH_DMATXNDESCRF_CPC_CRC_INSERT
#define ETH_CRC_REPLACE          ETH_DMATXNDESCRF_CPC_CRC_REPLACE
/**
  * @}
  */

/** @defgroup ETH_Tx_Packet_Checksum_Control ETH Tx Packet Checksum Control
  * @{
  */
#define ETH_CHECKSUM_DISABLE                         ETH_DMATXNDESCRF_CIC_DISABLE
#define ETH_CHECKSUM_IPHDR_INSERT                    ETH_DMATXNDESCRF_CIC_IPHDR_INSERT     
#define ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT            ETH_DMATXNDESCRF_CIC_IPHDR_PAYLOAD_INSERT              
#define ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC  ETH_DMATXNDESCRF_CIC_IPHDR_PAYLOAD_INSERT_PHDR_CALC         
/**
  * @}
  */

/** @defgroup ETH_Tx_Packet_VLAN_Control ETH Tx Packet VLAN Control
  * @{
  */
#define ETH_VLAN_DISABLE  ETH_DMATXNDESCRF_VTIR_DISABLE
#define ETH_VLAN_REMOVE   ETH_DMATXNDESCRF_VTIR_REMOVE
#define ETH_VLAN_INSERT   ETH_DMATXNDESCRF_VTIR_INSERT 
#define ETH_VLAN_REPLACE  ETH_DMATXNDESCRF_VTIR_REPLACE
/**
  * @}
  */

/** @defgroup ETH_Tx_Packet_Inner_VLAN_Control ETH Tx Packet Inner VLAN Control
  * @{
  */
#define ETH_INNER_VLAN_DISABLE  ETH_DMATXCDESC_IVTIR_DISABLE
#define ETH_INNER_VLAN_REMOVE   ETH_DMATXCDESC_IVTIR_REMOVE
#define ETH_INNER_VLAN_INSERT   ETH_DMATXCDESC_IVTIR_INSERT 
#define ETH_INNER_VLAN_REPLACE  ETH_DMATXCDESC_IVTIR_REPLACE
/**
  * @}
  */

/** @defgroup ETH_Rx_Checksum_Status ETH Rx Checksum Status
  * @{
  */
#define ETH_CHECKSUM_BYPASSED           ETH_DMARXNDESCWBF_IPCB
#define ETH_CHECKSUM_IP_HEADER_ERROR    ETH_DMARXNDESCWBF_IPHE
#define ETH_CHECKSUM_IP_PAYLOAD_ERROR   ETH_DMARXNDESCWBF_IPCE
/**
  * @}
  */

/** @defgroup ETH_Rx_IP_Header_Type ETH Rx IP Header Type
  * @{
  */
#define ETH_IP_HEADER_IPV4   ETH_DMARXNDESCWBF_IPV4
#define ETH_IP_HEADER_IPV6   ETH_DMARXNDESCWBF_IPV6
/**
  * @}
  */

/** @defgroup ETH_Rx_Payload_Type ETH Rx Payload Type
  * @{
  */
#define ETH_IP_PAYLOAD_UNKNOWN   ETH_DMARXNDESCWBF_PT_UNKNOWN
#define ETH_IP_PAYLOAD_UDP       ETH_DMARXNDESCWBF_PT_UDP
#define ETH_IP_PAYLOAD_TCP       ETH_DMARXNDESCWBF_PT_TCP
#define ETH_IP_PAYLOAD_ICMPN     ETH_DMARXNDESCWBF_PT_ICMP
/**
  * @}
  */

/** @defgroup ETH_Rx_MAC_Filter_Status ETH Rx MAC Filter Status
  * @{
  */
#define ETH_HASH_FILTER_PASS        ETH_DMARXNDESCWBF_HF
#define ETH_VLAN_FILTER_PASS        ETH_DMARXNDESCWBF_VF
#define ETH_DEST_ADDRESS_FAIL       ETH_DMARXNDESCWBF_DAF
#define ETH_SOURCE_ADDRESS_FAIL     ETH_DMARXNDESCWBF_SAF
/**
  * @}
  */

/** @defgroup ETH_Rx_L3_Filter_Status ETH Rx L3 Filter Status
  * @{
  */
#define ETH_L3_FILTER0_MATCH        ETH_DMARXNDESCWBF_L3FM
#define ETH_L3_FILTER1_MATCH        (ETH_DMARXNDESCWBF_L3FM | ETH_DMARXNDESCWBF_L3L4FM)
/**
  * @}
  */

/** @defgroup ETH_Rx_L4_Filter_Status ETH Rx L4 Filter Status
  * @{
  */
#define ETH_L4_FILTER0_MATCH        ETH_DMARXNDESCWBF_L4FM
#define ETH_L4_FILTER1_MATCH        (ETH_DMARXNDESCWBF_L4FM | ETH_DMARXNDESCWBF_L3L4FM)
/**
  * @}
  */

/** @defgroup ETH_Rx_Error_Code ETH Rx Error Code
  * @{
  */
#define ETH_DRIBBLE_BIT_ERROR   ETH_DMARXNDESCWBF_DE
#define ETH_RECEIVE_ERROR       ETH_DMARXNDESCWBF_RE
#define ETH_RECEIVE_OVERFLOW    ETH_DMARXNDESCWBF_OE
#define ETH_WATCHDOG_TIMEOUT    ETH_DMARXNDESCWBF_RWT
#define ETH_GIANT_PACKET        ETH_DMARXNDESCWBF_GP
#define ETH_CRC_ERROR           ETH_DMARXNDESCWBF_CE
/**
  * @}
  */

/** @defgroup ETH_DMA_Arbitration ETH DMA Arbitration
  * @{
  */ 
#define ETH_DMAARBITRATION_RX        ETH_DMAMR_DA
#define ETH_DMAARBITRATION_RX1_TX1   ((uint32_t)0x00000000U)
#define ETH_DMAARBITRATION_RX2_TX1   ETH_DMAMR_PR_2_1
#define ETH_DMAARBITRATION_RX3_TX1   ETH_DMAMR_PR_3_1
#define ETH_DMAARBITRATION_RX4_TX1   ETH_DMAMR_PR_4_1
#define ETH_DMAARBITRATION_RX5_TX1   ETH_DMAMR_PR_5_1
#define ETH_DMAARBITRATION_RX6_TX1   ETH_DMAMR_PR_6_1
#define ETH_DMAARBITRATION_RX7_TX1   ETH_DMAMR_PR_7_1
#define ETH_DMAARBITRATION_RX8_TX1   ETH_DMAMR_PR_8_1
#define ETH_DMAARBITRATION_TX        (ETH_DMAMR_TXPR | ETH_DMAMR_DA)
#define ETH_DMAARBITRATION_TX1_RX1   ((uint32_t)0x00000000U)
#define ETH_DMAARBITRATION_TX2_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_2_1)
#define ETH_DMAARBITRATION_TX3_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_3_1)
#define ETH_DMAARBITRATION_TX4_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_4_1)
#define ETH_DMAARBITRATION_TX5_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_5_1)
#define ETH_DMAARBITRATION_TX6_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_6_1)
#define ETH_DMAARBITRATION_TX7_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_7_1)
#define ETH_DMAARBITRATION_TX8_RX1   (ETH_DMAMR_TXPR | ETH_DMAMR_PR_8_1)
/**
  * @}
  */

 /** @defgroup ETH_Burst_Mode ETH Burst Mode  
  * @{
  */ 
#define ETH_BURSTLENGTH_FIXED           ETH_DMASBMR_FB
#define ETH_BURSTLENGTH_MIXED           ETH_DMASBMR_MB
#define ETH_BURSTLENGTH_UNSPECIFIED     ((uint32_t)0x00000000U)
/**
  * @}
  */
	
/** @defgroup ETH_Tx_DMA_Burst_Length ETH Tx DMA Burst Length
  * @{
  */ 
#define ETH_TXDMABURSTLENGTH_1BEAT          ETH_DMACTCR_TPBL_1PBL   
#define ETH_TXDMABURSTLENGTH_2BEAT          ETH_DMACTCR_TPBL_2PBL   
#define ETH_TXDMABURSTLENGTH_4BEAT          ETH_DMACTCR_TPBL_4PBL   
#define ETH_TXDMABURSTLENGTH_8BEAT          ETH_DMACTCR_TPBL_8PBL   
#define ETH_TXDMABURSTLENGTH_16BEAT         ETH_DMACTCR_TPBL_16PBL  
#define ETH_TXDMABURSTLENGTH_32BEAT         ETH_DMACTCR_TPBL_32PBL                  
/**
  * @}
  */

/** @defgroup ETH_Rx_DMA_Burst_Length ETH Rx DMA Burst Length
  * @{
  */ 
#define ETH_RXDMABURSTLENGTH_1BEAT          ETH_DMACRCR_RPBL_1PBL   
#define ETH_RXDMABURSTLENGTH_2BEAT          ETH_DMACRCR_RPBL_2PBL   
#define ETH_RXDMABURSTLENGTH_4BEAT          ETH_DMACRCR_RPBL_4PBL   
#define ETH_RXDMABURSTLENGTH_8BEAT          ETH_DMACRCR_RPBL_8PBL   
#define ETH_RXDMABURSTLENGTH_16BEAT         ETH_DMACRCR_RPBL_16PBL  
#define ETH_RXDMABURSTLENGTH_32BEAT         ETH_DMACRCR_RPBL_32PBL                  
/**
  * @}
  */	
	
/** @defgroup ETH_DMA_Interrupts ETH DMA Interrupts
  * @{
  */	
#define ETH_DMA_NORMAL_IT                 ETH_DMACIER_NIE 
#define ETH_DMA_ABNORMAL_IT               ETH_DMACIER_AIE 
#define ETH_DMA_CONTEXT_DESC_ERROR_IT     ETH_DMACIER_CDEE
#define ETH_DMA_FATAL_BUS_ERROR_IT        ETH_DMACIER_FBEE
#define ETH_DMA_EARLY_RX_IT               ETH_DMACIER_ERIE
#define ETH_DMA_EARLY_TX_IT               ETH_DMACIER_ETIE
#define ETH_DMA_RX_WATCHDOG_TIMEOUT_IT    ETH_DMACIER_RWTE
#define ETH_DMA_RX_PROCESS_STOPPED_IT     ETH_DMACIER_RSE 
#define ETH_DMA_RX_BUFFER_UNAVAILABLE_IT  ETH_DMACIER_RBUE
#define ETH_DMA_RX_IT                     ETH_DMACIER_RIE 
#define ETH_DMA_TX_BUFFER_UNAVAILABLE_IT  ETH_DMACIER_TBUE
#define ETH_DMA_TX_PROCESS_STOPPED_IT     ETH_DMACIER_TXSE
#define ETH_DMA_TX_IT                     ETH_DMACIER_TIE 
/**
  * @}
  */

/** @defgroup ETH_DMA_Status_Flags ETH DMA Status Flags
  * @{
  */	
#define ETH_DMA_RX_NO_ERROR_FLAG                 ((uint32_t)0x00000000U)
#define ETH_DMA_RX_DESC_READ_ERROR_FLAG          (ETH_DMACSR_REB_BIT_2 | ETH_DMACSR_REB_BIT_1 | ETH_DMACSR_REB_BIT_0)     
#define ETH_DMA_RX_DESC_WRITE_ERROR_FLAG         (ETH_DMACSR_REB_BIT_2 | ETH_DMACSR_REB_BIT_1)       
#define ETH_DMA_RX_BUFFER_READ_ERROR_FLAG        (ETH_DMACSR_REB_BIT_2 | ETH_DMACSR_REB_BIT_0)      
#define ETH_DMA_RX_BUFFER_WRITE_ERROR_FLAG        ETH_DMACSR_REB_BIT_2
#define ETH_DMA_TX_NO_ERROR_FLAG                 ((uint32_t)0x00000000U)
#define ETH_DMA_TX_DESC_READ_ERROR_FLAG          (ETH_DMACSR_TEB_BIT_2 | ETH_DMACSR_TEB_BIT_1 | ETH_DMACSR_TEB_BIT_0)     
#define ETH_DMA_TX_DESC_WRITE_ERROR_FLAG         (ETH_DMACSR_TEB_BIT_2 | ETH_DMACSR_TEB_BIT_1)      
#define ETH_DMA_TX_BUFFER_READ_ERROR_FLAG        (ETH_DMACSR_TEB_BIT_2 | ETH_DMACSR_TEB_BIT_0)     
#define ETH_DMA_TX_BUFFER_WRITE_ERROR_FLAG        ETH_DMACSR_TEB_BIT_2
#define ETH_DMA_CONTEXT_DESC_ERROR_FLAG           ETH_DMACSR_CDE
#define ETH_DMA_FATAL_BUS_ERROR_FLAG              ETH_DMACSR_FBE
#define ETH_DMA_EARLY_TX_IT_FLAG                  ETH_DMACSR_ERI
#define ETH_DMA_RX_WATCHDOG_TIMEOUT_FLAG          ETH_DMACSR_RWT
#define ETH_DMA_RX_PROCESS_STOPPED_FLAG           ETH_DMACSR_RPS
#define ETH_DMA_RX_BUFFER_UNAVAILABLE_FLAG        ETH_DMACSR_RBU
#define ETH_DMA_TX_PROCESS_STOPPED_FLAG           ETH_DMACSR_TPS
/**
  * @}
  */	
	
/** @defgroup ETH_Transmit_Mode ETH Transmit Mode 
  * @{
  */ 
#define ETH_TRANSMITSTOREFORWARD       ETH_MTLTQOMR_TSF
#define ETH_TRANSMITTHRESHOLD_32       ETH_MTLTQOMR_TTC_32BITS  
#define ETH_TRANSMITTHRESHOLD_64       ETH_MTLTQOMR_TTC_64BITS  
#define ETH_TRANSMITTHRESHOLD_96       ETH_MTLTQOMR_TTC_96BITS  
#define ETH_TRANSMITTHRESHOLD_128      ETH_MTLTQOMR_TTC_128BITS 
#define ETH_TRANSMITTHRESHOLD_192      ETH_MTLTQOMR_TTC_192BITS 
#define ETH_TRANSMITTHRESHOLD_256      ETH_MTLTQOMR_TTC_256BITS 
#define ETH_TRANSMITTHRESHOLD_384      ETH_MTLTQOMR_TTC_384BITS 
#define ETH_TRANSMITTHRESHOLD_512      ETH_MTLTQOMR_TTC_512BITS 
/**
  * @}
  */

/** @defgroup ETH_Receive_Mode ETH Receive Mode 
  * @{
  */ 
#define ETH_RECEIVESTOREFORWARD        ETH_MTLRQOMR_RSF
#define ETH_RECEIVETHRESHOLD8_64       ETH_MTLRQOMR_RTC_64BITS  
#define ETH_RECEIVETHRESHOLD8_32       ETH_MTLRQOMR_RTC_32BITS  
#define ETH_RECEIVETHRESHOLD8_96       ETH_MTLRQOMR_RTC_96BITS  
#define ETH_RECEIVETHRESHOLD8_128      ETH_MTLRQOMR_RTC_128BITS 
/**
  * @}
  */
	
/** @defgroup ETH_Pause_Low_Threshold  ETH Pause Low Threshold
  * @{
  */ 
#define ETH_PAUSELOWTHRESHOLD_MINUS_4        ETH_MACTFCR_PLT_MINUS4  
#define ETH_PAUSELOWTHRESHOLD_MINUS_28       ETH_MACTFCR_PLT_MINUS28 
#define ETH_PAUSELOWTHRESHOLD_MINUS_36       ETH_MACTFCR_PLT_MINUS36  
#define ETH_PAUSELOWTHRESHOLD_MINUS_144      ETH_MACTFCR_PLT_MINUS144 
#define ETH_PAUSELOWTHRESHOLD_MINUS_256      ETH_MACTFCR_PLT_MINUS256 
#define ETH_PAUSELOWTHRESHOLD_MINUS_512      ETH_MACTFCR_PLT_MINUS512 
/**
  * @}
  */
	
/** @defgroup ETH_Watchdog_Timeout ETH Watchdog Timeout
  * @{
  */ 
#define ETH_WATCHDOGTIMEOUT_2KB      ETH_MACWTR_WTO_2KB
#define ETH_WATCHDOGTIMEOUT_3KB      ETH_MACWTR_WTO_3KB
#define ETH_WATCHDOGTIMEOUT_4KB      ETH_MACWTR_WTO_4KB
#define ETH_WATCHDOGTIMEOUT_5KB      ETH_MACWTR_WTO_5KB
#define ETH_WATCHDOGTIMEOUT_6KB      ETH_MACWTR_WTO_6KB
#define ETH_WATCHDOGTIMEOUT_7KB      ETH_MACWTR_WTO_7KB
#define ETH_WATCHDOGTIMEOUT_8KB      ETH_MACWTR_WTO_8KB
#define ETH_WATCHDOGTIMEOUT_9KB      ETH_MACWTR_WTO_9KB
#define ETH_WATCHDOGTIMEOUT_10KB     ETH_MACWTR_WTO_10KB
#define ETH_WATCHDOGTIMEOUT_11KB     ETH_MACWTR_WTO_12KB
#define ETH_WATCHDOGTIMEOUT_12KB     ETH_MACWTR_WTO_12KB
#define ETH_WATCHDOGTIMEOUT_13KB     ETH_MACWTR_WTO_13KB
#define ETH_WATCHDOGTIMEOUT_14KB     ETH_MACWTR_WTO_14KB
#define ETH_WATCHDOGTIMEOUT_15KB     ETH_MACWTR_WTO_15KB
#define ETH_WATCHDOGTIMEOUT_16KB     ETH_MACWTR_WTO_16KB
/**
  * @}
  */  

/** @defgroup ETH_Inter_Packet_Gap ETH Inter Packet Gap
  * @{
  */ 
#define ETH_INTERPACKETGAP_96BIT   ETH_MACCR_IPG_96BIT 
#define ETH_INTERPACKETGAP_88BIT   ETH_MACCR_IPG_88BIT 
#define ETH_INTERPACKETGAP_80BIT   ETH_MACCR_IPG_80BIT  
#define ETH_INTERPACKETGAP_72BIT   ETH_MACCR_IPG_72BIT 
#define ETH_INTERPACKETGAP_64BIT   ETH_MACCR_IPG_64BIT  
#define ETH_INTERPACKETGAP_56BIT   ETH_MACCR_IPG_56BIT  
#define ETH_INTERPACKETGAP_48BIT   ETH_MACCR_IPG_48BIT 
#define ETH_INTERPACKETGAP_40BIT   ETH_MACCR_IPG_40BIT 
/**
  * @}
  */

/** @defgroup ETH_Speed  ETH Speed
  * @{
  */ 
#define ETH_SPEED_10M        ((uint32_t)0x00000000U)
#define ETH_SPEED_100M       ETH_MACCR_FES
/**
  * @}
  */

/** @defgroup ETH_Duplex_Mode ETH Duplex Mode 
  * @{
  */ 
#define ETH_FULLDUPLEX_MODE       ETH_MACCR_DM
#define ETH_HALFDUPLEX_MODE       ((uint32_t)0x00000000U)
/**
  * @}
  */
	
/** @defgroup ETH_Back_Off_Limit ETH Back Off Limit
  * @{
  */ 
#define ETH_BACKOFFLIMIT_10  ETH_MACCR_BL_10
#define ETH_BACKOFFLIMIT_8   ETH_MACCR_BL_8 
#define ETH_BACKOFFLIMIT_4   ETH_MACCR_BL_4 
#define ETH_BACKOFFLIMIT_1   ETH_MACCR_BL_1 
/**
  * @}
  */

/** @defgroup ETH_Preamble_Length ETH Preamble Length
  * @{
  */ 
#define ETH_PREAMBLELENGTH_7      ETH_MACCR_PRELEN_7
#define ETH_PREAMBLELENGTH_5      ETH_MACCR_PRELEN_5
#define ETH_PREAMBLELENGTH_3      ETH_MACCR_PRELEN_3
/**
  * @}
  */

/** @defgroup ETH_Source_Addr_Control ETH Source Addr Control
  * @{
  */ 
#define ETH_SOURCEADDRESS_DISABLE           ((uint32_t)0x00000000U)
#define ETH_SOURCEADDRESS_INSERT_ADDR0      ETH_MACCR_SARC_INSADDR0
#define ETH_SOURCEADDRESS_INSERT_ADDR1      ETH_MACCR_SARC_INSADDR1
#define ETH_SOURCEADDRESS_REPLACE_ADDR0     ETH_MACCR_SARC_REPADDR0
#define ETH_SOURCEADDRESS_REPLACE_ADDR1     ETH_MACCR_SARC_REPADDR1
/**
  * @}
  */
  
/** @defgroup ETH_Control_Packets_Filter ETH Control Packets Filter
  * @{
  */ 
#define ETH_CTRLPACKETS_BLOCK_ALL                      ETH_MACPFR_PCF_BLOCKALL
#define ETH_CTRLPACKETS_FORWARD_ALL_EXCEPT_PA          ETH_MACPFR_PCF_FORWARDALLEXCEPTPA
#define ETH_CTRLPACKETS_FORWARD_ALL                    ETH_MACPFR_PCF_FORWARDALL
#define ETH_CTRLPACKETS_FORWARD_PASSED_ADDR_FILTER     ETH_MACPFR_PCF_FORWARDPASSEDADDRFILTER
/**
  * @}
  */
	
/** @defgroup ETH_VLAN_Tag_Comparison ETH VLAN Tag Comparison
  * @{
  */ 
#define ETH_VLANTAGCOMPARISON_16BIT          ((uint32_t)0x00000000U)
#define ETH_VLANTAGCOMPARISON_12BIT          ETH_MACVTR_ETV
/**
  * @}
  */

/** @defgroup ETH_MAC_addresses ETH MAC addresses
  * @{
  */ 
#define ETH_MAC_ADDRESS0     ((uint32_t)0x00000000U)
#define ETH_MAC_ADDRESS1     ((uint32_t)0x00000008U)
#define ETH_MAC_ADDRESS2     ((uint32_t)0x00000010U)
#define ETH_MAC_ADDRESS3     ((uint32_t)0x00000018U)
/**
  * @}
  */    
 
/** @defgroup ETH_MAC_Interrupts ETH MAC Interrupts
  * @{
  */	
#define ETH_MAC_RX_STATUS_IT     ETH_MACIER_RXSTSIE 
#define ETH_MAC_TX_STATUS_IT     ETH_MACIER_TXSTSIE 
#define ETH_MAC_TIMESTAMP_IT     ETH_MACIER_TSIE
#define ETH_MAC_LPI_IT           ETH_MACIER_LPIIE
#define ETH_MAC_PMT_IT           ETH_MACIER_PMTIE
#define ETH_MAC_PHY_IT           ETH_MACIER_PHYIE
/**
  * @}
  */

/** @defgroup ETH_MAC_Wake_Up_Event ETH MAC Wake Up Event
  * @{
  */	
#define ETH_WAKEUP_PACKET_RECIEVED    ETH_MACPCSR_RWKPRCVD 
#define ETH_MAGIC_PACKET_RECIEVED     ETH_MACPCSR_MGKPRCVD 
/**
  * @}
  */

/** @defgroup ETH_MAC_Rx_Tx_Status ETH MAC Rx Tx Status
  * @{
  */
#define ETH_RECEIVE_WATCHDOG_TIMEOUT        ETH_MACRXTXSR_RWT
#define ETH_EXECESSIVE_COLLISIONS           ETH_MACRXTXSR_EXCOL
#define ETH_LATE_COLLISIONS                 ETH_MACRXTXSR_LCOL 
#define ETH_EXECESSIVE_DEFERRAL             ETH_MACRXTXSR_EXDEF
#define ETH_LOSS_OF_CARRIER                 ETH_MACRXTXSR_LCARR
#define ETH_NO_CARRIER                      ETH_MACRXTXSR_NCARR
#define ETH_TRANSMIT_JABBR_TIMEOUT          ETH_MACRXTXSR_TJT  	
/**
  * @}
  */

/** @defgroup HAL_ETH_StateTypeDef ETH States
  * @{
  */
#define HAL_ETH_STATE_RESET       ((uint32_t)0x00000000U)    /*!< Peripheral not yet Initialized or disabled */
#define HAL_ETH_STATE_READY       ((uint32_t)0x00000010U)    /*!< Peripheral Communication started           */
#define HAL_ETH_STATE_BUSY        ((uint32_t)0x00000023U)    /*!< an internal process is ongoing             */
#define HAL_ETH_STATE_BUSY_TX     ((uint32_t)0x00000021U)    /*!< Transmission process is ongoing            */
#define HAL_ETH_STATE_BUSY_RX     ((uint32_t)0x00000022U)    /*!< Reception process is ongoing               */
#define HAL_ETH_STATE_ERROR       ((uint32_t)0x000000E0U)    /*!< Error State                                */
/**
  * @}
  */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup ETH_Exported_Macros ETH Exported Macros
  * @{
  */

/** @brief Reset ETH handle state
  * @param  __HANDLE__: specifies the ETH handle.
  * @retval None
  */
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
#define __HAL_ETH_RESET_HANDLE_STATE(__HANDLE__)  do{                                                   \
                                                       (__HANDLE__)->gState = HAL_ETH_STATE_RESET;      \
                                                       (__HANDLE__)->RxState = HAL_ETH_STATE_RESET;     \
                                                       (__HANDLE__)->MspInitCallback = NULL;             \
                                                       (__HANDLE__)->MspDeInitCallback = NULL;           \
                                                     } while(0)
#else
#define __HAL_ETH_RESET_HANDLE_STATE(__HANDLE__)  do{                                                   \
                                                       (__HANDLE__)->gState = HAL_ETH_STATE_RESET;      \
                                                       (__HANDLE__)->RxState = HAL_ETH_STATE_RESET;     \
                                                     } while(0)
#endif /*USE_HAL_ETH_REGISTER_CALLBACKS */

/** 
  * @brief  Enables the specified ETHERNET DMA interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET DMA interrupt sources to be
  *   enabled @ref ETH_DMA_Interrupts
  * @retval None
  */
#define __HAL_ETH_DMA_ENABLE_IT(__HANDLE__, __INTERRUPT__)                 ((__HANDLE__)->Instance->DMACIER |= (__INTERRUPT__))

/**
  * @brief  Disables the specified ETHERNET DMA interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET DMA interrupt sources to be
  *   disabled. @ref ETH_DMA_Interrupts
  * @retval None
  */
#define __HAL_ETH_DMA_DISABLE_IT(__HANDLE__, __INTERRUPT__)                ((__HANDLE__)->Instance->DMACIER &= ~(__INTERRUPT__))

/**
  * @brief  Gets the ETHERNET DMA IT source enabled or disabled.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the interrupt source to get . @ref ETH_DMA_Interrupts
  * @retval The ETH DMA IT Source enabled or disabled
  */
#define __HAL_ETH_DMA_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)      (((__HANDLE__)->Instance->DMACIER &  (__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Gets the ETHERNET DMA IT pending bit.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the interrupt source to get . @ref ETH_DMA_Interrupts
  * @retval The state of ETH DMA IT (SET or RESET)
  */
#define __HAL_ETH_DMA_GET_IT(__HANDLE__, __INTERRUPT__)      (((__HANDLE__)->Instance->DMACSR &  (__INTERRUPT__)) == (__INTERRUPT__))
    
/**
  * @brief  Clears the ETHERNET DMA IT pending bit.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear. @ref ETH_DMA_Interrupts
  * @retval None
  */
#define __HAL_ETH_DMA_CLEAR_IT(__HANDLE__, __INTERRUPT__)      ((__HANDLE__)->Instance->DMACSR = (__INTERRUPT__))

/**
  * @brief  Checks whether the specified ETHERNET DMA flag is set or not.
* @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to check. @ref ETH_DMA_Status_Flags
  * @retval The state of ETH DMA FLAG (SET or RESET).
  */
#define __HAL_ETH_DMA_GET_FLAG(__HANDLE__, __FLAG__)                   (((__HANDLE__)->Instance->DMACSR &( __FLAG__)) == ( __FLAG__))  

/**
  * @brief  Clears the specified ETHERNET DMA flag.
* @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to check. @ref ETH_DMA_Status_Flags
  * @retval The state of ETH DMA FLAG (SET or RESET).
  */
#define __HAL_ETH_DMA_CLEAR_FLAG(__HANDLE__, __FLAG__)                   ((__HANDLE__)->Instance->DMACSR = ( __FLAG__))
    
/** 
  * @brief  Enables the specified ETHERNET MAC interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET MAC interrupt sources to be
  *   enabled @ref ETH_MAC_Interrupts
  * @retval None
  */
#define __HAL_ETH_MAC_ENABLE_IT(__HANDLE__, __INTERRUPT__)                 ((__HANDLE__)->Instance->MACIER |= (__INTERRUPT__))
    
/** 
  * @brief  Disables the specified ETHERNET MAC interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET MAC interrupt sources to be
  *   enabled @ref ETH_MAC_Interrupts
  * @retval None
  */
#define __HAL_ETH_MAC_DISABLE_IT(__HANDLE__, __INTERRUPT__)                 ((__HANDLE__)->Instance->MACIER &= ~(__INTERRUPT__))
  
/**
  * @brief  Checks whether the specified ETHERNET MAC flag is set or not.
  * @param  __HANDLE__: ETH Handle
  * @param  __INTERRUPT__: specifies the flag to check. @ref ETH_MAC_Interrupts
  * @retval The state of ETH MAC IT (SET or RESET).
  */
#define __HAL_ETH_MAC_GET_IT(__HANDLE__, __INTERRUPT__)                   (((__HANDLE__)->Instance->MACISR &( __INTERRUPT__)) == ( __INTERRUPT__)) 

/*!< External interrupt line 86 Connected to the ETH wakeup EXTI Line */
#define ETH_WAKEUP_EXTI_LINE  ((uint32_t)0x00400000U)  /* !<  86 - 64 = 22 */

/**
  * @brief Enable the ETH WAKEUP Exti Line.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP Exti sources to be enabled.
  *   @arg ETH_WAKEUP_EXTI_LINE     
  * @retval None.
  */
#define __HAL_ETH_WAKEUP_EXTI_ENABLE_IT(__EXTI_LINE__)   (EXTI_D1->IMR3 |= (__EXTI_LINE__))

/**
  * @brief checks whether the specified ETH WAKEUP Exti interrupt flag is set or not.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP Exti sources to be cleared.
  *   @arg ETH_WAKEUP_EXTI_LINE  
  * @retval EXTI ETH WAKEUP Line Status.
  */
#define __HAL_ETH_WAKEUP_EXTI_GET_FLAG(__EXTI_LINE__)  (EXTI_D1->PR3 & (__EXTI_LINE__))

/**
  * @brief Clear the ETH WAKEUP Exti flag.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP Exti sources to be cleared.
  *   @arg ETH_WAKEUP_EXTI_LINE  
  * @retval None.
  */
#define __HAL_ETH_WAKEUP_EXTI_CLEAR_FLAG(__EXTI_LINE__) (EXTI_D1->PR3 = (__EXTI_LINE__))

#if defined(DUAL_CORE)
/**
  * @brief Enable the ETH WAKEUP Exti Line by Core2.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP Exti sources to be enabled.
  *   @arg ETH_WAKEUP_EXTI_LINE     
  * @retval None.
  */
#define __HAL_ETH_WAKEUP_EXTID2_ENABLE_IT(__EXTI_LINE__)   (EXTI_D2->IMR3 |= (__EXTI_LINE__)) 

/**
  * @brief checks whether the specified ETH WAKEUP Exti interrupt flag is set or not.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP Exti sources to be cleared.
  *   @arg ETH_WAKEUP_EXTI_LINE  
  * @retval EXTI ETH WAKEUP Line Status.
  */
#define __HAL_ETH_WAKEUP_EXTID2_GET_FLAG(__EXTI_LINE__)  (EXTI_D2->PR3 & (__EXTI_LINE__))

/**
  * @brief Clear the ETH WAKEUP Exti flag.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP Exti sources to be cleared.
  *   @arg ETH_WAKEUP_EXTI_LINE  
  * @retval None.
  */
#define __HAL_ETH_WAKEUP_EXTID2_CLEAR_FLAG(__EXTI_LINE__) (EXTI_D2->PR3 = (__EXTI_LINE__))
#endif

/**
  * @brief  enable rising edge interrupt on selected EXTI line.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP EXTI sources to be disabled.
  *  @arg ETH_WAKEUP_EXTI_LINE
  * @retval None
  */
#define __HAL_ETH_WAKEUP_EXTI_ENABLE_RISING_EDGE(__EXTI_LINE__) (EXTI->FTSR3 &= ~(__EXTI_LINE__)); \
                                                                (EXTI->RTSR3 |= (__EXTI_LINE__)) 

/**
  * @brief  enable falling edge interrupt on selected EXTI line.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP EXTI sources to be disabled.
  *  @arg ETH_WAKEUP_EXTI_LINE
  * @retval None
  */
#define __HAL_ETH_WAKEUP_EXTI_ENABLE_FALLING_EDGE(__EXTI_LINE__) (EXTI->RTSR3 &= ~(__EXTI_LINE__));\
                                                                 (EXTI->FTSR3 |= (__EXTI_LINE__))

/**
  * @brief  enable falling edge interrupt on selected EXTI line.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP EXTI sources to be disabled.
  *  @arg ETH_WAKEUP_EXTI_LINE
  * @retval None
  */
#define __HAL_ETH_WAKEUP_EXTI_ENABLE_RISING_FALLING_EDGE(__EXTI_LINE__) (EXTI->RTSR3 |= (__EXTI_LINE__));\
                                                                        (EXTI->FTSR3 |= (__EXTI_LINE__))

/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param  __EXTI_LINE__: specifies the ETH WAKEUP EXTI sources to be disabled.
  *  @arg ETH_WAKEUP_EXTI_LINE
  * @retval None
  */
#define __HAL_ETH_WAKEUP_EXTI_GENERATE_SWIT(__EXTI_LINE__) (EXTI->SWIER3 |= (__EXTI_LINE__))

/**
  * @}
  */

/* Include ETH HAL Extension module */
#include "stm32h7xx_hal_eth_ex.h"

/* Exported functions --------------------------------------------------------*/

/** @addtogroup ETH_Exported_Functions
  * @{
  */
  
/** @addtogroup ETH_Exported_Functions_Group1
  * @{
  */    
/* Initialization and de initialization functions  **********************************/
HAL_StatusTypeDef HAL_ETH_Init(ETH_HandleTypeDef *heth);                        
HAL_StatusTypeDef HAL_ETH_DeInit(ETH_HandleTypeDef *heth);                      
void              HAL_ETH_MspInit(ETH_HandleTypeDef *heth);                    
void              HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth);  
HAL_StatusTypeDef HAL_ETH_DescAssignMemory(ETH_HandleTypeDef *heth, uint32_t Index, uint8_t *pBuffer1,uint8_t *pBuffer2);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_ETH_RegisterCallback(ETH_HandleTypeDef *heth, HAL_ETH_CallbackIDTypeDef CallbackID, pETH_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_ETH_UnRegisterCallback(ETH_HandleTypeDef *heth, HAL_ETH_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @addtogroup ETH_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_ETH_Start(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_Start_IT(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_Stop(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_Stop_IT(ETH_HandleTypeDef *heth);

uint8_t           HAL_ETH_IsRxDataAvailable(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_GetRxDataBuffer(ETH_HandleTypeDef *heth, ETH_BufferTypeDef *RxBuffer);
HAL_StatusTypeDef HAL_ETH_GetRxDataLength(ETH_HandleTypeDef *heth, uint32_t *Length);
HAL_StatusTypeDef HAL_ETH_GetRxDataInfo(ETH_HandleTypeDef *heth, ETH_RxPacketInfo *RxPacketInfo);
HAL_StatusTypeDef HAL_ETH_BuildRxDescriptors(ETH_HandleTypeDef *heth);

HAL_StatusTypeDef HAL_ETH_Transmit(ETH_HandleTypeDef *heth, ETH_TxPacketConfig *pTxConfig, uint32_t Timeout);
HAL_StatusTypeDef HAL_ETH_Transmit_IT(ETH_HandleTypeDef *heth, ETH_TxPacketConfig *pTxConfig);

HAL_StatusTypeDef HAL_ETH_WritePHYRegister(ETH_HandleTypeDef *heth, uint32_t PHYAddr, uint32_t PHYReg, uint32_t RegValue);  
HAL_StatusTypeDef HAL_ETH_ReadPHYRegister(ETH_HandleTypeDef *heth, uint32_t PHYAddr, uint32_t PHYReg, uint32_t *pRegValue); 

void              HAL_ETH_IRQHandler(ETH_HandleTypeDef *heth);
void              HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth);
void              HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth);
void              HAL_ETH_DMAErrorCallback(ETH_HandleTypeDef *heth);
void              HAL_ETH_MACErrorCallback(ETH_HandleTypeDef *heth);
void              HAL_ETH_PMTCallback(ETH_HandleTypeDef *heth);
void              HAL_ETH_EEECallback(ETH_HandleTypeDef *heth);
void              HAL_ETH_WakeUpCallback(ETH_HandleTypeDef *heth);
/**
  * @}
  */

/** @addtogroup ETH_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control functions  **********************************************/
/* MAC & DMA Configuration APIs  **********************************************/
HAL_StatusTypeDef HAL_ETH_GetMACConfig(ETH_HandleTypeDef *heth, ETH_MACConfigTypeDef *macconf);
HAL_StatusTypeDef HAL_ETH_GetDMAConfig(ETH_HandleTypeDef *heth, ETH_DMAConfigTypeDef *dmaconf);
HAL_StatusTypeDef HAL_ETH_SetMACConfig(ETH_HandleTypeDef *heth, ETH_MACConfigTypeDef *macconf);
HAL_StatusTypeDef HAL_ETH_SetDMAConfig(ETH_HandleTypeDef *heth, ETH_DMAConfigTypeDef *dmaconf);
void              HAL_ETH_SetMDIOClockRange(ETH_HandleTypeDef *heth);

/* MAC VLAN Processing APIs    ************************************************/
void              HAL_ETH_SetRxVLANIdentifier(ETH_HandleTypeDef *heth, uint32_t ComparisonBits, uint32_t VLANIdentifier);

/* MAC L2 Packet Filtering APIs  **********************************************/
HAL_StatusTypeDef HAL_ETH_GetMACFilterConfig(ETH_HandleTypeDef *heth, ETH_MACFilterConfigTypeDef *pFilterConfig);
HAL_StatusTypeDef HAL_ETH_SetMACFilterConfig(ETH_HandleTypeDef *heth, ETH_MACFilterConfigTypeDef *pFilterConfig);
HAL_StatusTypeDef HAL_ETH_SetHashTable(ETH_HandleTypeDef *heth, uint32_t *pHashTable);
HAL_StatusTypeDef HAL_ETH_SetSourceMACAddrMatch(ETH_HandleTypeDef *heth, uint32_t AddrNbr, uint8_t *pMACAddr);

/* MAC Power Down APIs    *****************************************************/
void              HAL_ETH_EnterPowerDownMode(ETH_HandleTypeDef *heth, ETH_PowerDownConfigTypeDef *pPowerDownConfig);
void              HAL_ETH_ExitPowerDownMode(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_SetWakeUpFilter(ETH_HandleTypeDef *heth, uint32_t *pFilter, uint32_t Count);

/**
  * @}
  */

/** @addtogroup ETH_Exported_Functions_Group4
  * @{
  */
/* Peripheral State functions  **************************************************/
HAL_ETH_StateTypeDef HAL_ETH_GetState(ETH_HandleTypeDef *heth);
uint32_t             HAL_ETH_GetError(ETH_HandleTypeDef *heth);
uint32_t             HAL_ETH_GetDMAError(ETH_HandleTypeDef *heth);
uint32_t             HAL_ETH_GetMACError(ETH_HandleTypeDef *heth);
uint32_t             HAL_ETH_GetMACWakeUpSource(ETH_HandleTypeDef *heth);
/**
  * @}
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

#endif /* ETH */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_ETH_H */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
