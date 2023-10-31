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

#ifndef _SAME70_SPI1_INSTANCE_
#define _SAME70_SPI1_INSTANCE_

/* ========== Register definition for SPI1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SPI1_CR                       (0x40058000U) /**< \brief (SPI1) Control Register */
  #define REG_SPI1_MR                       (0x40058004U) /**< \brief (SPI1) Mode Register */
  #define REG_SPI1_RDR                      (0x40058008U) /**< \brief (SPI1) Receive Data Register */
  #define REG_SPI1_TDR                      (0x4005800CU) /**< \brief (SPI1) Transmit Data Register */
  #define REG_SPI1_SR                       (0x40058010U) /**< \brief (SPI1) Status Register */
  #define REG_SPI1_IER                      (0x40058014U) /**< \brief (SPI1) Interrupt Enable Register */
  #define REG_SPI1_IDR                      (0x40058018U) /**< \brief (SPI1) Interrupt Disable Register */
  #define REG_SPI1_IMR                      (0x4005801CU) /**< \brief (SPI1) Interrupt Mask Register */
  #define REG_SPI1_CSR                      (0x40058030U) /**< \brief (SPI1) Chip Select Register (CS_number = 0) */
  #define REG_SPI1_WPMR                     (0x400580E4U) /**< \brief (SPI1) Write Protection Mode Register */
  #define REG_SPI1_WPSR                     (0x400580E8U) /**< \brief (SPI1) Write Protection Status Register */
  #define REG_SPI1_VERSION                  (0x400580FCU) /**< \brief (SPI1) Version Register */
#else
  #define REG_SPI1_CR      (*(__O  uint32_t*)0x40058000U) /**< \brief (SPI1) Control Register */
  #define REG_SPI1_MR      (*(__IO uint32_t*)0x40058004U) /**< \brief (SPI1) Mode Register */
  #define REG_SPI1_RDR     (*(__I  uint32_t*)0x40058008U) /**< \brief (SPI1) Receive Data Register */
  #define REG_SPI1_TDR     (*(__O  uint32_t*)0x4005800CU) /**< \brief (SPI1) Transmit Data Register */
  #define REG_SPI1_SR      (*(__I  uint32_t*)0x40058010U) /**< \brief (SPI1) Status Register */
  #define REG_SPI1_IER     (*(__O  uint32_t*)0x40058014U) /**< \brief (SPI1) Interrupt Enable Register */
  #define REG_SPI1_IDR     (*(__O  uint32_t*)0x40058018U) /**< \brief (SPI1) Interrupt Disable Register */
  #define REG_SPI1_IMR     (*(__I  uint32_t*)0x4005801CU) /**< \brief (SPI1) Interrupt Mask Register */
  #define REG_SPI1_CSR     (*(__IO uint32_t*)0x40058030U) /**< \brief (SPI1) Chip Select Register (CS_number = 0) */
  #define REG_SPI1_WPMR    (*(__IO uint32_t*)0x400580E4U) /**< \brief (SPI1) Write Protection Mode Register */
  #define REG_SPI1_WPSR    (*(__I  uint32_t*)0x400580E8U) /**< \brief (SPI1) Write Protection Status Register */
  #define REG_SPI1_VERSION (*(__I  uint32_t*)0x400580FCU) /**< \brief (SPI1) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_SPI1_INSTANCE_ */
