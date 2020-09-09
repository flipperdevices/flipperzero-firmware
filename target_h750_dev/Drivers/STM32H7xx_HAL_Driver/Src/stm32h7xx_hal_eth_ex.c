/**
  ******************************************************************************
  * @file    stm32h7xx_hal_eth_ex.c
  * @author  MCD Application Team
  * @brief   ETH HAL Extended module driver.
  *
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

#ifdef HAL_ETH_MODULE_ENABLED

#if defined(ETH)

/** @defgroup ETHEx ETHEx
  * @brief ETH HAL Extended module driver
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup ETHEx_Private_Constants ETHEx Private Constants
  * @{
  */
#define ETH_MACL4CR_MASK     (ETH_MACL3L4CR_L4PEN | ETH_MACL3L4CR_L4SPM | \
                              ETH_MACL3L4CR_L4SPIM | ETH_MACL3L4CR_L4DPM | \
                              ETH_MACL3L4CR_L4DPIM)

#define ETH_MACL3CR_MASK     (ETH_MACL3L4CR_L3PEN | ETH_MACL3L4CR_L3SAM | \
                              ETH_MACL3L4CR_L3SAIM | ETH_MACL3L4CR_L3DAM | \
                              ETH_MACL3L4CR_L3DAIM | ETH_MACL3L4CR_L3HSBM | \
                              ETH_MACL3L4CR_L3HDBM)

#define ETH_MACRXVLAN_MASK (ETH_MACVTR_EIVLRXS | ETH_MACVTR_EIVLS | \
                            ETH_MACVTR_ERIVLT | ETH_MACVTR_EDVLP | \
                            ETH_MACVTR_VTHM | ETH_MACVTR_EVLRXS | \
                            ETH_MACVTR_EVLS | ETH_MACVTR_DOVLTC | \
                            ETH_MACVTR_ERSVLM | ETH_MACVTR_ESVL | \
                            ETH_MACVTR_VTIM | ETH_MACVTR_ETV)

#define ETH_MACTXVLAN_MASK (ETH_MACVIR_VLTI | ETH_MACVIR_CSVL | \
                            ETH_MACVIR_VLP | ETH_MACVIR_VLC)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/
/** @defgroup ETHEx_Exported_Functions ETH Extended Exported Functions
  * @{
  */

/** @defgroup ETHEx_Exported_Functions_Group1 Extended features functions
  * @brief    Extended features functions
 *
@verbatim
 ===============================================================================
                      ##### Extended features functions #####
 ===============================================================================
    [..] This section provides functions allowing to:
      (+) Configure ARP offload module
      (+) Configure L3 and L4 filters
      (+) Configure Extended VLAN features
      (+) Configure Energy Efficient Ethernet module

@endverbatim
  * @{
  */

/**
  * @brief  Enables ARP Offload.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
void HAL_ETHEx_EnableARPOffload(ETH_HandleTypeDef *heth)
{
  SET_BIT(heth->Instance->MACCR, ETH_MACCR_ARP);
}

/**
  * @brief  Disables ARP Offload.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
void HAL_ETHEx_DisableARPOffload(ETH_HandleTypeDef *heth)
{
  CLEAR_BIT(heth->Instance->MACCR, ETH_MACCR_ARP);
}

/**
  * @brief  Set the ARP Match IP address
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  IpAddress: IP Address to be matched for incoming ARP requests
  * @retval None
  */
void HAL_ETHEx_SetARPAddressMatch(ETH_HandleTypeDef *heth, uint32_t IpAddress)
{
  WRITE_REG(heth->Instance->MACARPAR, IpAddress);
}

/**
  * @brief  Configures the L4 Filter, this function allow to:
  *         set the layer 4 protocol to be matched (TCP or UDP)
  *         enable/disable L4 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L4 filter to configured, this parameter must be one of the following
  *           ETH_L4_FILTER_0
  *           ETH_L4_FILTER_1
  * @param  pL4FilterConfig: pointer to a ETH_L4FilterConfigTypeDef structure
  *         that contains L4 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetL4FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter , ETH_L4FilterConfigTypeDef *pL4FilterConfig)
{
  __IO uint32_t *configreg = ((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter));

  if(pL4FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  /* Write configuration to (MACL3L4C0R + filter )register */
  MODIFY_REG(*configreg, ETH_MACL4CR_MASK ,(pL4FilterConfig->Protocol |
                                            pL4FilterConfig->SrcPortFilterMatch |
                                              pL4FilterConfig->DestPortFilterMatch));

  configreg = ((__IO uint32_t *)(&(heth->Instance->MACL4A0R) + Filter));

  /* Write configuration to (MACL4A0R + filter )register */
  MODIFY_REG(*configreg, (ETH_MACL4AR_L4DP | ETH_MACL4AR_L4SP) , (pL4FilterConfig->SourcePort |
                                                                                                                  (pL4FilterConfig->DestinationPort << 16)));

  /* Enable L4 filter */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);

  return HAL_OK;
}

/**
  * @brief  Configures the L4 Filter, this function allow to:
  *         set the layer 4 protocol to be matched (TCP or UDP)
  *         enable/disable L4 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L4 filter to configured, this parameter must be one of the following
  *           ETH_L4_FILTER_0
  *           ETH_L4_FILTER_1
  * @param  pL4FilterConfig: pointer to a ETH_L4FilterConfigTypeDef structure
  *         that contains L4 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetL4FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L4FilterConfigTypeDef *pL4FilterConfig)
{
  if(pL4FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  /* Get configuration to (MACL3L4C0R + filter )register */
  pL4FilterConfig->Protocol = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), ETH_MACL3L4CR_L4PEN);
  pL4FilterConfig->DestPortFilterMatch = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), (ETH_MACL3L4CR_L4DPM | ETH_MACL3L4CR_L4DPIM));
  pL4FilterConfig->SrcPortFilterMatch = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), (ETH_MACL3L4CR_L4SPM | ETH_MACL3L4CR_L4SPIM));

  /* Get configuration to (MACL3L4C0R + filter )register */
  pL4FilterConfig->DestinationPort = (READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL4A0R) + Filter)), ETH_MACL4AR_L4DP) >> 16);
  pL4FilterConfig->SourcePort = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL4A0R) + Filter)), ETH_MACL4AR_L4SP);

  return HAL_OK;
}

/**
  * @brief  Configures the L3 Filter, this function allow to:
  *         set the layer 3 protocol to be matched (IPv4 or IPv6)
  *         enable/disable L3 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L3 filter to configured, this parameter must be one of the following
  *           ETH_L3_FILTER_0
  *           ETH_L3_FILTER_1
  * @param  pL3FilterConfig: pointer to a ETH_L3FilterConfigTypeDef structure
  *         that contains L3 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetL3FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L3FilterConfigTypeDef *pL3FilterConfig)
{
  __IO uint32_t *configreg = ((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter));

  if(pL3FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  /* Write configuration to (MACL3L4C0R + filter )register */
  MODIFY_REG(*configreg, ETH_MACL3CR_MASK, (pL3FilterConfig->Protocol |
                                            pL3FilterConfig->SrcAddrFilterMatch |
                                              pL3FilterConfig->DestAddrFilterMatch |
                                                (pL3FilterConfig->SrcAddrHigherBitsMatch << 6) |
                                                  (pL3FilterConfig->DestAddrHigherBitsMatch << 11)));

  /* Check if IPv6 protocol is selected */
  if(pL3FilterConfig->Protocol != ETH_L3_IPV4_MATCH)
  {
    /* Set the IPv6 address match */
    /* Set Bits[31:0] of 128-bit IP addr */
    *((__IO uint32_t *)(&(heth->Instance->MACL3A0R0R) + Filter)) = pL3FilterConfig->Ip6Addr[0];
    /* Set Bits[63:32] of 128-bit IP addr */
    *((__IO uint32_t *)(&(heth->Instance->MACL3A1R0R) + Filter)) = pL3FilterConfig->Ip6Addr[1];
    /* update Bits[95:64] of 128-bit IP addr */
    *((__IO uint32_t *)(&(heth->Instance->MACL3A2R0R) + Filter)) = pL3FilterConfig->Ip6Addr[2];
    /* update Bits[127:96] of 128-bit IP addr */
    *((__IO uint32_t *)(&(heth->Instance->MACL3A3R0R) + Filter)) = pL3FilterConfig->Ip6Addr[3];
  }
  else /* IPv4 protocol is selected */
  {
    /* Set the IPv4 source address match */
    *((__IO uint32_t *)(&(heth->Instance->MACL3A0R0R) + Filter)) = pL3FilterConfig->Ip4SrcAddr;
    /* Set the IPv4 destination address match */
    *((__IO uint32_t *)(&(heth->Instance->MACL3A1R0R) + Filter)) = pL3FilterConfig->Ip4DestAddr;
  }

  return HAL_OK;
}

/**
  * @brief  Configures the L3 Filter, this function allow to:
  *         set the layer 3 protocol to be matched (IPv4 or IPv6)
  *         enable/disable L3 source/destination port perfect/inverse match.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  Filter: L3 filter to configured, this parameter must be one of the following
  *           ETH_L3_FILTER_0
  *           ETH_L3_FILTER_1
  * @param  pL3FilterConfig: pointer to a ETH_L3FilterConfigTypeDef structure
  *         that will contain the L3 filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetL3FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter, ETH_L3FilterConfigTypeDef *pL3FilterConfig)
{
  if(pL3FilterConfig == NULL)
  {
    return HAL_ERROR;
  }

  pL3FilterConfig->Protocol = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), ETH_MACL3L4CR_L3PEN);
  pL3FilterConfig->SrcAddrFilterMatch = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), (ETH_MACL3L4CR_L3SAM | ETH_MACL3L4CR_L3SAIM));
  pL3FilterConfig->DestAddrFilterMatch = READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), (ETH_MACL3L4CR_L3DAM | ETH_MACL3L4CR_L3DAIM));
  pL3FilterConfig->SrcAddrHigherBitsMatch = (READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), ETH_MACL3L4CR_L3HSBM) >> 6);
  pL3FilterConfig->DestAddrHigherBitsMatch = (READ_BIT(*((__IO uint32_t *)(&(heth->Instance->MACL3L4C0R) + Filter)), ETH_MACL3L4CR_L3HDBM) >> 11);

  if(pL3FilterConfig->Protocol != ETH_L3_IPV4_MATCH)
  {
    pL3FilterConfig->Ip6Addr[0] = *((__IO uint32_t *)(&(heth->Instance->MACL3A0R0R) + Filter));
    pL3FilterConfig->Ip6Addr[1] = *((__IO uint32_t *)(&(heth->Instance->MACL3A1R0R) + Filter));
    pL3FilterConfig->Ip6Addr[2] = *((__IO uint32_t *)(&(heth->Instance->MACL3A2R0R) + Filter));
    pL3FilterConfig->Ip6Addr[3] = *((__IO uint32_t *)(&(heth->Instance->MACL3A3R0R) + Filter));
  }
  else
  {
    pL3FilterConfig->Ip4SrcAddr = *((__IO uint32_t *)(&(heth->Instance->MACL3A0R0R) + Filter));
    pL3FilterConfig->Ip4DestAddr = *((__IO uint32_t *)(&(heth->Instance->MACL3A1R0R) + Filter));
  }

  return HAL_OK;
}

/**
  * @brief  Enables L3 and L4 filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_EnableL3L4Filtering(ETH_HandleTypeDef *heth)
{
  /* Enable L3/L4 filter */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);
}

/**
  * @brief  Disables L3 and L4 filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_DisableL3L4Filtering(ETH_HandleTypeDef *heth)
{
  /* Disable L3/L4 filter */
  CLEAR_BIT(heth->Instance->MACPFR, ETH_MACPFR_IPFE);
}

/**
  * @brief  Get the VLAN Configuration for Receive Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  pVlanConfig: pointer to a ETH_RxVLANConfigTypeDef structure
  *         that will contain the VLAN filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_GetRxVLANConfig(ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig)
{
  if(pVlanConfig == NULL)
  {
    return HAL_ERROR;
  }

  pVlanConfig->InnerVLANTagInStatus = ((READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_EIVLRXS) >> 31) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->StripInnerVLANTag  = READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_EIVLS);
  pVlanConfig->InnerVLANTag = ((READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_ERIVLT) >> 27) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->DoubleVLANProcessing = ((READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_EDVLP) >> 26) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->VLANTagHashTableMatch = ((READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_VTHM) >> 25) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->VLANTagInStatus = ((READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_EVLRXS) >> 24) == 0U) ? DISABLE : ENABLE;
  pVlanConfig->StripVLANTag = READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_EVLS);
  pVlanConfig->VLANTypeCheck = READ_BIT(heth->Instance->MACVTR, (ETH_MACVTR_DOVLTC | ETH_MACVTR_ERSVLM | ETH_MACVTR_ESVL));
  pVlanConfig->VLANTagInverceMatch = ((READ_BIT(heth->Instance->MACVTR, ETH_MACVTR_VTIM) >> 17) == 0U) ? DISABLE : ENABLE;

  return HAL_OK;
}

/**
  * @brief  Set the VLAN Configuration for Receive Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  pVlanConfig: pointer to a ETH_RxVLANConfigTypeDef structure
  *         that contains VLAN filter configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETHEx_SetRxVLANConfig(ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig)
{
  if(pVlanConfig == NULL)
  {
    return HAL_ERROR;
  }

  /* Write config to MACVTR */
  MODIFY_REG(heth->Instance->MACVTR, ETH_MACRXVLAN_MASK, (((uint32_t)pVlanConfig->InnerVLANTagInStatus << 31) |
                                                          pVlanConfig->StripInnerVLANTag |
                                                            ((uint32_t)pVlanConfig->InnerVLANTag << 27) |
                                                              ((uint32_t)pVlanConfig->DoubleVLANProcessing << 26) |
                                                                ((uint32_t)pVlanConfig->VLANTagHashTableMatch << 25) |
                                                                  ((uint32_t)pVlanConfig->VLANTagInStatus << 24) |
                                                                    pVlanConfig->StripVLANTag |
                                                                      pVlanConfig->VLANTypeCheck |
                                                                        ((uint32_t)pVlanConfig->VLANTagInverceMatch << 17)));

  return HAL_OK;
}

/**
  * @brief  Set the VLAN Hash Table
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANHashTable: VLAN hash table 16 bit value
  * @retval None
  */
void HAL_ETHEx_SetVLANHashTable(ETH_HandleTypeDef *heth, uint32_t VLANHashTable)
{
  MODIFY_REG(heth->Instance->MACVHTR, ETH_MACVHTR_VLHT, VLANHashTable);
}

/**
  * @brief  Get the VLAN Configuration for Transmit Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANTag: Selects the vlan tag, this parameter must be one of the following
  *                 ETH_OUTER_TX_VLANTAG
  *                 ETH_INNER_TX_VLANTAG
  * @param  pVlanConfig: pointer to a ETH_TxVLANConfigTypeDef structure
  *         that will contain the Tx VLAN filter configuration.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_ETHEx_GetTxVLANConfig(ETH_HandleTypeDef *heth, uint32_t VLANTag ,ETH_TxVLANConfigTypeDef *pVlanConfig)
{
  if (pVlanConfig == NULL)
  {
    return HAL_ERROR;
  }

  if(VLANTag == ETH_INNER_TX_VLANTAG)
  {
    pVlanConfig->SourceTxDesc = ((READ_BIT(heth->Instance->MACIVIR, ETH_MACVIR_VLTI) >> 20) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->SVLANType = ((READ_BIT(heth->Instance->MACIVIR, ETH_MACVIR_CSVL) >> 19) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->VLANTagControl = READ_BIT(heth->Instance->MACIVIR, (ETH_MACVIR_VLP | ETH_MACVIR_VLC));
  }
  else
  {
    pVlanConfig->SourceTxDesc = ((READ_BIT(heth->Instance->MACVIR, ETH_MACVIR_VLTI) >> 20) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->SVLANType = ((READ_BIT(heth->Instance->MACVIR, ETH_MACVIR_CSVL) >> 19) == 0U) ? DISABLE : ENABLE;
    pVlanConfig->VLANTagControl = READ_BIT(heth->Instance->MACVIR, (ETH_MACVIR_VLP | ETH_MACVIR_VLC));
  }

  return HAL_OK;;
}

/**
  * @brief  Set the VLAN Configuration for Transmit Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANTag: Selects the vlan tag, this parameter must be one of the following
  *                 ETH_OUTER_TX_VLANTAG
  *                 ETH_INNER_TX_VLANTAG
  * @param  pVlanConfig: pointer to a ETH_TxVLANConfigTypeDef structure
  *         that contains Tx VLAN filter configuration.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_ETHEx_SetTxVLANConfig(ETH_HandleTypeDef *heth, uint32_t VLANTag ,ETH_TxVLANConfigTypeDef *pVlanConfig)
{
  if(VLANTag == ETH_INNER_TX_VLANTAG)
  {
    MODIFY_REG(heth->Instance->MACIVIR, ETH_MACTXVLAN_MASK, (((uint32_t)pVlanConfig->SourceTxDesc << 20) |
                                                            ((uint32_t)pVlanConfig->SVLANType << 19) |
                                                              pVlanConfig->VLANTagControl));
    /* Enable Double VLAN processing */
    SET_BIT(heth->Instance->MACVTR, ETH_MACVTR_EDVLP);
  }
  else
  {
    MODIFY_REG(heth->Instance->MACVIR, ETH_MACTXVLAN_MASK, (((uint32_t)pVlanConfig->SourceTxDesc << 20) |
                                                            ((uint32_t)pVlanConfig->SVLANType << 19) |
                                                              pVlanConfig->VLANTagControl));
  }

  return HAL_OK;
}

/**
  * @brief  Set the VLAN Tag Identifier for Transmit Packets.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  VLANTag: Selects the vlan tag, this parameter must be one of the following
  *                 ETH_OUTER_TX_VLANTAG
  *                 ETH_INNER_TX_VLANTAG
  * @param  VLANIdentifier: VLAN Identifier 16 bit value
  * @retval None
  */
void HAL_ETHEx_SetTxVLANIdentifier(ETH_HandleTypeDef *heth, uint32_t VLANTag ,uint32_t VLANIdentifier)
{
  if(VLANTag == ETH_INNER_TX_VLANTAG)
  {
    MODIFY_REG(heth->Instance->MACIVIR, ETH_MACVIR_VLT, VLANIdentifier);
  }
  else
  {
    MODIFY_REG(heth->Instance->MACVIR, ETH_MACVIR_VLT, VLANIdentifier);
  }
}

/**
  * @brief  Enables the VLAN Tag Filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_EnableVLANProcessing(ETH_HandleTypeDef *heth)
{
  /* Enable VLAN processing */
  SET_BIT(heth->Instance->MACPFR, ETH_MACPFR_VTFE);
}

/**
  * @brief  Disables the VLAN Tag Filtering process.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None.
  */
void HAL_ETHEx_DisableVLANProcessing(ETH_HandleTypeDef *heth)
{
  /* Disable VLAN processing */
  CLEAR_BIT(heth->Instance->MACPFR, ETH_MACPFR_VTFE);
}

/**
  * @brief  Enters the Low Power Idle (LPI) mode
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  TxAutomate: Enable/Disbale automate enter/exit LPI mode.
  * @param  TxClockStop: Enable/Disbale Tx clock stop in LPI mode.
  * @retval None
  */
void HAL_ETHEx_EnterLPIMode(ETH_HandleTypeDef *heth, FunctionalState TxAutomate, FunctionalState TxClockStop)
{
  /* Enable LPI Interrupts */
  __HAL_ETH_MAC_ENABLE_IT(heth, ETH_MACIER_LPIIE);

  /* Write to LPI Control register: Enter low power mode */
  MODIFY_REG(heth->Instance->MACLCSR, (ETH_MACLCSR_LPIEN | ETH_MACLCSR_LPITXA | ETH_MACLCSR_LPITCSE), (((uint32_t)TxAutomate << 19) |
                                                                                                       ((uint32_t)TxClockStop << 21) |
                                                                                                         ETH_MACLCSR_LPIEN));
}

/**
  * @brief  Exits the Low Power Idle (LPI) mode.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
void HAL_ETHEx_ExitLPIMode(ETH_HandleTypeDef *heth)
{
  /* Clear the LPI Config and exit low power mode */
  CLEAR_BIT(heth->Instance->MACLCSR, (ETH_MACLCSR_LPIEN | ETH_MACLCSR_LPITXA | ETH_MACLCSR_LPITCSE));

  /* Enable LPI Interrupts */
  __HAL_ETH_MAC_DISABLE_IT(heth, ETH_MACIER_LPIIE);
}


/**
  * @brief  Returns the ETH MAC LPI event
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval ETH MAC WakeUp event
  */
uint32_t HAL_ETHEx_GetMACLPIEvent(ETH_HandleTypeDef *heth)
{
  return heth->MACLPIEvent;
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

#endif /* ETH */

#endif /* HAL_ETH_MODULE_ENABLED */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
