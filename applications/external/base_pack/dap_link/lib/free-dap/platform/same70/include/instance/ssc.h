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

#ifndef _SAME70_SSC_INSTANCE_
#define _SAME70_SSC_INSTANCE_

/* ========== Register definition for SSC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SSC_CR                       (0x40004000U) /**< \brief (SSC) Control Register */
  #define REG_SSC_CMR                      (0x40004004U) /**< \brief (SSC) Clock Mode Register */
  #define REG_SSC_RCMR                     (0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
  #define REG_SSC_RFMR                     (0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
  #define REG_SSC_TCMR                     (0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
  #define REG_SSC_TFMR                     (0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
  #define REG_SSC_RHR                      (0x40004020U) /**< \brief (SSC) Receive Holding Register */
  #define REG_SSC_THR                      (0x40004024U) /**< \brief (SSC) Transmit Holding Register */
  #define REG_SSC_RSHR                     (0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
  #define REG_SSC_TSHR                     (0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
  #define REG_SSC_RC0R                     (0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
  #define REG_SSC_RC1R                     (0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
  #define REG_SSC_SR                       (0x40004040U) /**< \brief (SSC) Status Register */
  #define REG_SSC_IER                      (0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
  #define REG_SSC_IDR                      (0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
  #define REG_SSC_IMR                      (0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
  #define REG_SSC_WPMR                     (0x400040E4U) /**< \brief (SSC) Write Protection Mode Register */
  #define REG_SSC_WPSR                     (0x400040E8U) /**< \brief (SSC) Write Protection Status Register */
  #define REG_SSC_VERSION                  (0x400040FCU) /**< \brief (SSC) Version Register */
#else
  #define REG_SSC_CR      (*(__O  uint32_t*)0x40004000U) /**< \brief (SSC) Control Register */
  #define REG_SSC_CMR     (*(__IO uint32_t*)0x40004004U) /**< \brief (SSC) Clock Mode Register */
  #define REG_SSC_RCMR    (*(__IO uint32_t*)0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
  #define REG_SSC_RFMR    (*(__IO uint32_t*)0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
  #define REG_SSC_TCMR    (*(__IO uint32_t*)0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
  #define REG_SSC_TFMR    (*(__IO uint32_t*)0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
  #define REG_SSC_RHR     (*(__I  uint32_t*)0x40004020U) /**< \brief (SSC) Receive Holding Register */
  #define REG_SSC_THR     (*(__O  uint32_t*)0x40004024U) /**< \brief (SSC) Transmit Holding Register */
  #define REG_SSC_RSHR    (*(__I  uint32_t*)0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
  #define REG_SSC_TSHR    (*(__IO uint32_t*)0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
  #define REG_SSC_RC0R    (*(__IO uint32_t*)0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
  #define REG_SSC_RC1R    (*(__IO uint32_t*)0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
  #define REG_SSC_SR      (*(__I  uint32_t*)0x40004040U) /**< \brief (SSC) Status Register */
  #define REG_SSC_IER     (*(__O  uint32_t*)0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
  #define REG_SSC_IDR     (*(__O  uint32_t*)0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
  #define REG_SSC_IMR     (*(__I  uint32_t*)0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
  #define REG_SSC_WPMR    (*(__IO uint32_t*)0x400040E4U) /**< \brief (SSC) Write Protection Mode Register */
  #define REG_SSC_WPSR    (*(__I  uint32_t*)0x400040E8U) /**< \brief (SSC) Write Protection Status Register */
  #define REG_SSC_VERSION (*(__I  uint32_t*)0x400040FCU) /**< \brief (SSC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_SSC_INSTANCE_ */
