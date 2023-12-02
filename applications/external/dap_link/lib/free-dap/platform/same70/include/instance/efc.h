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

#ifndef _SAME70_EFC_INSTANCE_
#define _SAME70_EFC_INSTANCE_

/* ========== Register definition for EFC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_EFC_FMR                      (0x400E0C00U) /**< \brief (EFC) EEFC Flash Mode Register */
  #define REG_EFC_FCR                      (0x400E0C04U) /**< \brief (EFC) EEFC Flash Command Register */
  #define REG_EFC_FSR                      (0x400E0C08U) /**< \brief (EFC) EEFC Flash Status Register */
  #define REG_EFC_FRR                      (0x400E0C0CU) /**< \brief (EFC) EEFC Flash Result Register */
  #define REG_EFC_VERSION                  (0x400E0C14U) /**< \brief (EFC) EEFC Version Register */
  #define REG_EFC_WPMR                     (0x400E0CE4U) /**< \brief (EFC) Write Protection Mode Register */
#else
  #define REG_EFC_FMR     (*(__IO uint32_t*)0x400E0C00U) /**< \brief (EFC) EEFC Flash Mode Register */
  #define REG_EFC_FCR     (*(__O  uint32_t*)0x400E0C04U) /**< \brief (EFC) EEFC Flash Command Register */
  #define REG_EFC_FSR     (*(__I  uint32_t*)0x400E0C08U) /**< \brief (EFC) EEFC Flash Status Register */
  #define REG_EFC_FRR     (*(__I  uint32_t*)0x400E0C0CU) /**< \brief (EFC) EEFC Flash Result Register */
  #define REG_EFC_VERSION (*(__I  uint32_t*)0x400E0C14U) /**< \brief (EFC) EEFC Version Register */
  #define REG_EFC_WPMR    (*(__IO uint32_t*)0x400E0CE4U) /**< \brief (EFC) Write Protection Mode Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_EFC_INSTANCE_ */
