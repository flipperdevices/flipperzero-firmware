/**
 * \file
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _SAME70_SUPC_INSTANCE_
#define _SAME70_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SUPC_CR                       (0x400E1810U) /**< \brief (SUPC) Supply Controller Control Register */
  #define REG_SUPC_SMMR                     (0x400E1814U) /**< \brief (SUPC) Supply Controller Supply Monitor Mode Register */
  #define REG_SUPC_MR                       (0x400E1818U) /**< \brief (SUPC) Supply Controller Mode Register */
  #define REG_SUPC_WUMR                     (0x400E181CU) /**< \brief (SUPC) Supply Controller Wake-up Mode Register */
  #define REG_SUPC_WUIR                     (0x400E1820U) /**< \brief (SUPC) Supply Controller Wake-up Inputs Register */
  #define REG_SUPC_SR                       (0x400E1824U) /**< \brief (SUPC) Supply Controller Status Register */
  #define REG_SYSC_VERSION                  (0x400E190CU) /**< \brief (SUPC) Version Register */
#else
  #define REG_SUPC_CR      (*(__O  uint32_t*)0x400E1810U) /**< \brief (SUPC) Supply Controller Control Register */
  #define REG_SUPC_SMMR    (*(__IO uint32_t*)0x400E1814U) /**< \brief (SUPC) Supply Controller Supply Monitor Mode Register */
  #define REG_SUPC_MR      (*(__IO uint32_t*)0x400E1818U) /**< \brief (SUPC) Supply Controller Mode Register */
  #define REG_SUPC_WUMR    (*(__IO uint32_t*)0x400E181CU) /**< \brief (SUPC) Supply Controller Wake-up Mode Register */
  #define REG_SUPC_WUIR    (*(__IO uint32_t*)0x400E1820U) /**< \brief (SUPC) Supply Controller Wake-up Inputs Register */
  #define REG_SUPC_SR      (*(__I  uint32_t*)0x400E1824U) /**< \brief (SUPC) Supply Controller Status Register */
  #define REG_SYSC_VERSION (*(__I  uint32_t*)0x400E190CU) /**< \brief (SUPC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_SUPC_INSTANCE_ */
