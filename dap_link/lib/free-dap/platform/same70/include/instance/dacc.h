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

#ifndef _SAME70_DACC_INSTANCE_
#define _SAME70_DACC_INSTANCE_

/* ========== Register definition for DACC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_DACC_CR                       (0x40040000U) /**< \brief (DACC) Control Register */
  #define REG_DACC_MR                       (0x40040004U) /**< \brief (DACC) Mode Register */
  #define REG_DACC_TRIGR                    (0x40040008U) /**< \brief (DACC) Trigger Register */
  #define REG_DACC_CHER                     (0x40040010U) /**< \brief (DACC) Channel Enable Register */
  #define REG_DACC_CHDR                     (0x40040014U) /**< \brief (DACC) Channel Disable Register */
  #define REG_DACC_CHSR                     (0x40040018U) /**< \brief (DACC) Channel Status Register */
  #define REG_DACC_CDR                      (0x4004001CU) /**< \brief (DACC) Conversion Data Register */
  #define REG_DACC_IER                      (0x40040024U) /**< \brief (DACC) Interrupt Enable Register */
  #define REG_DACC_IDR                      (0x40040028U) /**< \brief (DACC) Interrupt Disable Register */
  #define REG_DACC_IMR                      (0x4004002CU) /**< \brief (DACC) Interrupt Mask Register */
  #define REG_DACC_ISR                      (0x40040030U) /**< \brief (DACC) Interrupt Status Register */
  #define REG_DACC_ACR                      (0x40040094U) /**< \brief (DACC) Analog Current Register */
  #define REG_DACC_WPMR                     (0x400400E4U) /**< \brief (DACC) Write Protection Mode Register */
  #define REG_DACC_WPSR                     (0x400400E8U) /**< \brief (DACC) Write Protection Status Register */
  #define REG_DACC_VERSION                  (0x400400FCU) /**< \brief (DACC) Version Register */
#else
  #define REG_DACC_CR      (*(__O  uint32_t*)0x40040000U) /**< \brief (DACC) Control Register */
  #define REG_DACC_MR      (*(__IO uint32_t*)0x40040004U) /**< \brief (DACC) Mode Register */
  #define REG_DACC_TRIGR   (*(__IO uint32_t*)0x40040008U) /**< \brief (DACC) Trigger Register */
  #define REG_DACC_CHER    (*(__O  uint32_t*)0x40040010U) /**< \brief (DACC) Channel Enable Register */
  #define REG_DACC_CHDR    (*(__O  uint32_t*)0x40040014U) /**< \brief (DACC) Channel Disable Register */
  #define REG_DACC_CHSR    (*(__I  uint32_t*)0x40040018U) /**< \brief (DACC) Channel Status Register */
  #define REG_DACC_CDR     (*(__O  uint32_t*)0x4004001CU) /**< \brief (DACC) Conversion Data Register */
  #define REG_DACC_IER     (*(__O  uint32_t*)0x40040024U) /**< \brief (DACC) Interrupt Enable Register */
  #define REG_DACC_IDR     (*(__O  uint32_t*)0x40040028U) /**< \brief (DACC) Interrupt Disable Register */
  #define REG_DACC_IMR     (*(__I  uint32_t*)0x4004002CU) /**< \brief (DACC) Interrupt Mask Register */
  #define REG_DACC_ISR     (*(__I  uint32_t*)0x40040030U) /**< \brief (DACC) Interrupt Status Register */
  #define REG_DACC_ACR     (*(__IO uint32_t*)0x40040094U) /**< \brief (DACC) Analog Current Register */
  #define REG_DACC_WPMR    (*(__IO uint32_t*)0x400400E4U) /**< \brief (DACC) Write Protection Mode Register */
  #define REG_DACC_WPSR    (*(__I  uint32_t*)0x400400E8U) /**< \brief (DACC) Write Protection Status Register */
  #define REG_DACC_VERSION (*(__I  uint32_t*)0x400400FCU) /**< \brief (DACC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_DACC_INSTANCE_ */
