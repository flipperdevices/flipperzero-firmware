/**
  ******************************************************************************
  * @file    stm32h7xx_hal_eth_ex.h
  * @author  MCD Application Team
  * @brief   Header file of ETH HAL Extended module.
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
#ifndef STM32H7xx_HAL_ETH_EX_H
#define STM32H7xx_HAL_ETH_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(ETH)

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup ETHEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** @defgroup ETHEx_Exported_Types ETHEx Exported Types
  * @{
  */
  
/** 
  * @brief  ETH RX VLAN structure definition
  */
typedef struct{
  FunctionalState InnerVLANTagInStatus;      /*!< Enables or disables Inner VLAN Tag in Rx Status  */
	
  uint32_t StripInnerVLANTag;                /*!< Sets the Inner VLAN Tag Stripping on Receive 
                                                  This parameter can be a value of @ref ETHEx_Rx_Inner_VLAN_Tag_Stripping */
	
  FunctionalState InnerVLANTag;              /*!< Enables or disables Inner VLAN Tag */

  FunctionalState DoubleVLANProcessing;      /*!< Enable or Disable double VLAN processing */
	
  FunctionalState VLANTagHashTableMatch;     /*!< Enable or Disable VLAN Tag Hash Table Match */
	
  FunctionalState VLANTagInStatus;           /*!< Enable or Disable VLAN Tag in Rx status */
	
  uint32_t StripVLANTag;                     /*!< Set the VLAN Tag Stripping on Receive 
                                                  This parameter can be a value of @ref ETHEx_Rx_VLAN_Tag_Stripping */
	
  uint32_t VLANTypeCheck;                    /*!< Enable or Disable VLAN Type Check
                                                  This parameter can be a value of @ref ETHEx_VLAN_Type_Check */
																				 
  FunctionalState VLANTagInverceMatch;       /*!< Enable or disable VLAN Tag Inverse Match */																			 
}ETH_RxVLANConfigTypeDef;
/** 
  * 
  */
  
/** 
  * @brief  ETH TX VLAN structure definition
  */
typedef struct{
  FunctionalState SourceTxDesc;   /*!< Enable or Disable VLAN tag source from DMA tx descriptors */
  
  FunctionalState SVLANType;      /*!< Enable or Disable insertion of SVLAN type */
	
  uint32_t VLANTagControl;        /*!< Sets the VLAN tag control in tx packets
                                      This parameter can be a value of @ref ETHEx_VLAN_Tag_Control */
}ETH_TxVLANConfigTypeDef;
/** 
  * 
  */

/** 
  * @brief  ETH L3 filter structure definition
  */
typedef struct{
  uint32_t Protocol;                /*!< Sets the L3 filter protocol to IPv4 or IPv6
                                         This parameter can be a value of @ref ETHEx_L3_Protocol */
  
  uint32_t SrcAddrFilterMatch;      /*!< Sets the L3 filter source address match
                                         This parameter can be a value of @ref ETHEx_L3_Source_Match */
  
  uint32_t DestAddrFilterMatch;     /*!< Sets the L3 filter destination address match
                                         This parameter can be a value of @ref ETHEx_L3_Destination_Match */
  
  uint32_t SrcAddrHigherBitsMatch;  /*!< Sets the L3 filter source address higher bits match
                                         This parameter can be a value from 0 to 31 */
  
  uint32_t DestAddrHigherBitsMatch; /*!< Sets the L3 filter destination address higher bits match
                                         This parameter can be a value from 0 to 31 */
  
  uint32_t Ip4SrcAddr;              /*!< Sets the L3 filter IPv4 source address if IPv4 protocol is used
                                         This parameter can be a value from 0x0 to 0xFFFFFFFF */
  
  uint32_t Ip4DestAddr;             /*!< Sets the L3 filter IPv4 destination  address if IPv4 protocol is used
                                         This parameter can be a value from 0 to 0xFFFFFFFF  */
  
  uint32_t Ip6Addr[4];                 /*!< Sets the L3 filter IPv6 address if IPv6 protocol is used
                                          This parameter must be a table of 4 words (4* 32 bits) */
}ETH_L3FilterConfigTypeDef;
/** 
  * 
  */

/** 
  * @brief  ETH L4 filter structure definition
  */
typedef struct{
  uint32_t Protocol;               /*!< Sets the L4 filter protocol to TCP or UDP
                                        This parameter can be a value of @ref ETHEx_L4_Protocol */
  
  uint32_t SrcPortFilterMatch;     /*!< Sets the L4 filter source port match
                                        This parameter can be a value of @ref ETHEx_L4_Source_Match */
  
  uint32_t DestPortFilterMatch;    /*!< Sets the L4 filter destination port match
                                        This parameter can be a value of @ref ETHEx_L4_Destination_Match */
  
  uint32_t SourcePort;             /*!< Sets the L4 filter source port 
                                        This parameter must be a value from 0x0 to 0xFFFF */
  
  uint32_t DestinationPort;        /*!< Sets the L4 filter destination port 
                                        This parameter must be a value from 0x0 to 0xFFFF */	
}ETH_L4FilterConfigTypeDef;
/** 
  * 
  */
  
/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup ETHEx_Exported_Constants ETHEx Exported Constants
  * @{
  */
    
/** @defgroup ETHEx_LPI_Event ETHEx LPI Event
  * @{
  */	
#define ETH_TX_LPI_ENTRY    ETH_MACLCSR_TLPIEN 
#define ETH_TX_LPI_EXIT     ETH_MACLCSR_TLPIEX 
#define ETH_RX_LPI_ENTRY    ETH_MACLCSR_RLPIEN
#define ETH_RX_LPI_EXIT     ETH_MACLCSR_RLPIEX
/**
  * @}
  */
  
/** @defgroup ETHEx_L3_Filter ETHEx L3 Filter
  * @{
  */
#define ETH_L3_FILTER_0                 ((uint32_t)0x00000000)
#define ETH_L3_FILTER_1                 ((uint32_t)0x0000000C)
/**
  * @}
  */

/** @defgroup ETHEx_L4_Filter ETHEx L4 Filter
  * @{
  */
#define ETH_L4_FILTER_0                 ((uint32_t)0x00000000)
#define ETH_L4_FILTER_1                 ((uint32_t)0x0000000C)
/**
  * @}
  */

/** @defgroup ETHEx_L3_Protocol ETHEx L3 Protocol
  * @{
  */
#define ETH_L3_IPV6_MATCH                       ETH_MACL3L4CR_L3PEN
#define ETH_L3_IPV4_MATCH                       ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup ETHEx_L3_Source_Match ETHEx L3 Source Match
  * @{
  */
#define ETH_L3_SRC_ADDR_PERFECT_MATCH_ENABLE    ETH_MACL3L4CR_L3SAM
#define ETH_L3_SRC_ADDR_INVERSE_MATCH_ENABLE    (ETH_MACL3L4CR_L3SAM | ETH_MACL3L4CR_L3SAIM)
#define ETH_L3_SRC_ADDR_MATCH_DISABLE           ((uint32_t)0x00000000)
/**
  * @}
  */
	
/** @defgroup ETHEx_L3_Destination_Match ETHEx L3 Destination Match
  * @{
  */
#define ETH_L3_DEST_ADDR_PERFECT_MATCH_ENABLE   ETH_MACL3L4CR_L3DAM
#define ETH_L3_DEST_ADDR_INVERSE_MATCH_ENABLE   (ETH_MACL3L4CR_L3DAM | ETH_MACL3L4CR_L3DAIM)
#define ETH_L3_DEST_ADDR_MATCH_DISABLE          ((uint32_t)0x00000000)
/**
  * @}
  */
	
/** @defgroup ETHEx_L4_Protocol ETHEx L4 Protocol
  * @{
  */
#define ETH_L4_UDP_MATCH                        ETH_MACL3L4CR_L4PEN
#define ETH_L4_TCP_MATCH                        ((uint32_t)0x00000000)
/**
  * @}
  */
	
/** @defgroup ETHEx_L4_Source_Match ETHEx L4 Source Match
  * @{
  */
#define ETH_L4_SRC_PORT_PERFECT_MATCH_ENABLE    ETH_MACL3L4CR_L4SPM
#define ETH_L4_SRC_PORT_INVERSE_MATCH_ENABLE    (ETH_MACL3L4CR_L4SPM |ETH_MACL3L4CR_L4SPIM) 
#define ETH_L4_SRC_PORT_MATCH_DISABLE           ((uint32_t)0x00000000)
/**
  * @}
  */
	
/** @defgroup ETHEx_L4_Destination_Match ETHEx L4 Destination Match
  * @{
  */
#define ETH_L4_DEST_PORT_PERFECT_MATCH_ENABLE   ETH_MACL3L4CR_L4DPM
#define ETH_L4_DEST_PORT_INVERSE_MATCH_ENABLE   (ETH_MACL3L4CR_L4DPM | ETH_MACL3L4CR_L4DPIM)
#define ETH_L4_DEST_PORT_MATCH_DISABLE          ((uint32_t)0x00000000)
/**
  * @}
  */
  
/** @defgroup ETHEx_Rx_Inner_VLAN_Tag_Stripping ETHEx Rx Inner VLAN Tag Stripping
  * @{
  */ 
#define ETH_INNERVLANTAGRXSTRIPPING_NONE      ETH_MACVTR_EIVLS_DONOTSTRIP
#define ETH_INNERVLANTAGRXSTRIPPING_IFPASS    ETH_MACVTR_EIVLS_STRIPIFPASS
#define ETH_INNERVLANTAGRXSTRIPPING_IFFAILS   ETH_MACVTR_EIVLS_STRIPIFFAILS
#define ETH_INNERVLANTAGRXSTRIPPING_ALWAYS    ETH_MACVTR_EIVLS_ALWAYSSTRIP
/**
  * @}
  */

/** @defgroup ETHEx_Rx_VLAN_Tag_Stripping ETHEx Rx VLAN Tag Stripping
  * @{
  */ 
#define ETH_VLANTAGRXSTRIPPING_NONE      ETH_MACVTR_EVLS_DONOTSTRIP
#define ETH_VLANTAGRXSTRIPPING_IFPASS    ETH_MACVTR_EVLS_STRIPIFPASS
#define ETH_VLANTAGRXSTRIPPING_IFFAILS   ETH_MACVTR_EVLS_STRIPIFFAILS
#define ETH_VLANTAGRXSTRIPPING_ALWAYS    ETH_MACVTR_EVLS_ALWAYSSTRIP
/**
  * @}
  */
  
/** @defgroup ETHEx_VLAN_Type_Check ETHEx VLAN Type Check
  * @{
  */ 
#define ETH_VLANTYPECHECK_DISABLE    ETH_MACVTR_DOVLTC
#define ETH_VLANTYPECHECK_SVLAN      (ETH_MACVTR_ERSVLM | ETH_MACVTR_ESVL)
#define ETH_VLANTYPECHECK_CVLAN      ((uint32_t)0x00000000)
/**
  * @}
  */
  
/** @defgroup ETHEx_VLAN_Tag_Control ETHEx_VLAN_Tag_Control
  * @{
  */
#define ETH_VLANTAGCONTROL_NONE       (ETH_MACVIR_VLP | ETH_MACVIR_VLC_NOVLANTAG)
#define ETH_VLANTAGCONTROL_DELETE     (ETH_MACVIR_VLP | ETH_MACVIR_VLC_VLANTAGDELETE)
#define ETH_VLANTAGCONTROL_INSERT     (ETH_MACVIR_VLP | ETH_MACVIR_VLC_VLANTAGINSERT)
#define ETH_VLANTAGCONTROL_REPLACE    (ETH_MACVIR_VLP | ETH_MACVIR_VLC_VLANTAGREPLACE)
/**
  * @}
  */	
 
/** @defgroup ETHEx_Tx_VLAN_Tag ETHEx Tx VLAN Tag
  * @{
  */ 
#define ETH_INNER_TX_VLANTAG    ((uint32_t)0x00000001U)
#define ETH_OUTER_TX_VLANTAG    ((uint32_t)0x00000000U)
/**
  * @}
  */ 
  
/**
  * @}
  */
  
/* Exported functions --------------------------------------------------------*/
/** @addtogroup ETHEx_Exported_Functions
  * @{
  */

/** @addtogroup ETHEx_Exported_Functions_Group1
  * @{
  */
/* MAC ARP Offloading APIs  ***************************************************/
void              HAL_ETHEx_EnableARPOffload(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_DisableARPOffload(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_SetARPAddressMatch(ETH_HandleTypeDef *heth, uint32_t IpAddress); 

/* MAC L3 L4 Filtering APIs ***************************************************/
void              HAL_ETHEx_EnableL3L4Filtering(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_DisableL3L4Filtering(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_GetL3FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L3FilterConfigTypeDef *pL3FilterConfig);
HAL_StatusTypeDef HAL_ETHEx_GetL4FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L4FilterConfigTypeDef *pL4FilterConfig);
HAL_StatusTypeDef HAL_ETHEx_SetL3FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L3FilterConfigTypeDef *pL3FilterConfig);
HAL_StatusTypeDef HAL_ETHEx_SetL4FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L4FilterConfigTypeDef *pL4FilterConfig);

/* MAC VLAN Processing APIs    ************************************************/
void              HAL_ETHEx_EnableVLANProcessing(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_DisableVLANProcessing(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_GetRxVLANConfig(ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig);
HAL_StatusTypeDef HAL_ETHEx_SetRxVLANConfig(ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig);
void              HAL_ETHEx_SetVLANHashTable(ETH_HandleTypeDef *heth, uint32_t VLANHashTable);
HAL_StatusTypeDef HAL_ETHEx_GetTxVLANConfig(ETH_HandleTypeDef *heth, uint32_t VLANTag ,ETH_TxVLANConfigTypeDef *pVlanConfig);
HAL_StatusTypeDef HAL_ETHEx_SetTxVLANConfig(ETH_HandleTypeDef *heth, uint32_t VLANTag ,ETH_TxVLANConfigTypeDef *pVlanConfig);
void              HAL_ETHEx_SetTxVLANIdentifier(ETH_HandleTypeDef *heth, uint32_t VLANTag ,uint32_t VLANIdentifier);

/* Energy Efficient Ethernet APIs *********************************************/
void              HAL_ETHEx_EnterLPIMode(ETH_HandleTypeDef *heth, FunctionalState TxAutomate, FunctionalState TxClockStop);
void              HAL_ETHEx_ExitLPIMode(ETH_HandleTypeDef *heth);
uint32_t          HAL_ETHEx_GetMACLPIEvent(ETH_HandleTypeDef *heth);
 
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

#endif /* STM32H7xx_HAL_ETH_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
