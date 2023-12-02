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

#ifndef _SAME70_SPI0_INSTANCE_
#define _SAME70_SPI0_INSTANCE_

/* ========== Register definition for SPI0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SPI0_CR                       (0x40008000U) /**< \brief (SPI0) Control Register */
  #define REG_SPI0_MR                       (0x40008004U) /**< \brief (SPI0) Mode Register */
  #define REG_SPI0_RDR                      (0x40008008U) /**< \brief (SPI0) Receive Data Register */
  #define REG_SPI0_TDR                      (0x4000800CU) /**< \brief (SPI0) Transmit Data Register */
  #define REG_SPI0_SR                       (0x40008010U) /**< \brief (SPI0) Status Register */
  #define REG_SPI0_IER                      (0x40008014U) /**< \brief (SPI0) Interrupt Enable Register */
  #define REG_SPI0_IDR                      (0x40008018U) /**< \brief (SPI0) Interrupt Disable Register */
  #define REG_SPI0_IMR                      (0x4000801CU) /**< \brief (SPI0) Interrupt Mask Register */
  #define REG_SPI0_CSR                      (0x40008030U) /**< \brief (SPI0) Chip Select Register (CS_number = 0) */
  #define REG_SPI0_WPMR                     (0x400080E4U) /**< \brief (SPI0) Write Protection Mode Register */
  #define REG_SPI0_WPSR                     (0x400080E8U) /**< \brief (SPI0) Write Protection Status Register */
  #define REG_SPI0_VERSION                  (0x400080FCU) /**< \brief (SPI0) Version Register */
#else
  #define REG_SPI0_CR      (*(__O  uint32_t*)0x40008000U) /**< \brief (SPI0) Control Register */
  #define REG_SPI0_MR      (*(__IO uint32_t*)0x40008004U) /**< \brief (SPI0) Mode Register */
  #define REG_SPI0_RDR     (*(__I  uint32_t*)0x40008008U) /**< \brief (SPI0) Receive Data Register */
  #define REG_SPI0_TDR     (*(__O  uint32_t*)0x4000800CU) /**< \brief (SPI0) Transmit Data Register */
  #define REG_SPI0_SR      (*(__I  uint32_t*)0x40008010U) /**< \brief (SPI0) Status Register */
  #define REG_SPI0_IER     (*(__O  uint32_t*)0x40008014U) /**< \brief (SPI0) Interrupt Enable Register */
  #define REG_SPI0_IDR     (*(__O  uint32_t*)0x40008018U) /**< \brief (SPI0) Interrupt Disable Register */
  #define REG_SPI0_IMR     (*(__I  uint32_t*)0x4000801CU) /**< \brief (SPI0) Interrupt Mask Register */
  #define REG_SPI0_CSR     (*(__IO uint32_t*)0x40008030U) /**< \brief (SPI0) Chip Select Register (CS_number = 0) */
  #define REG_SPI0_WPMR    (*(__IO uint32_t*)0x400080E4U) /**< \brief (SPI0) Write Protection Mode Register */
  #define REG_SPI0_WPSR    (*(__I  uint32_t*)0x400080E8U) /**< \brief (SPI0) Write Protection Status Register */
  #define REG_SPI0_VERSION (*(__I  uint32_t*)0x400080FCU) /**< \brief (SPI0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_SPI0_INSTANCE_ */
