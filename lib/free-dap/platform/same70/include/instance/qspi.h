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

#ifndef _SAME70_QSPI_INSTANCE_
#define _SAME70_QSPI_INSTANCE_

/* ========== Register definition for QSPI peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_QSPI_CR                       (0x4007C000U) /**< \brief (QSPI) Control Register */
  #define REG_QSPI_MR                       (0x4007C004U) /**< \brief (QSPI) Mode Register */
  #define REG_QSPI_RDR                      (0x4007C008U) /**< \brief (QSPI) Receive Data Register */
  #define REG_QSPI_TDR                      (0x4007C00CU) /**< \brief (QSPI) Transmit Data Register */
  #define REG_QSPI_SR                       (0x4007C010U) /**< \brief (QSPI) Status Register */
  #define REG_QSPI_IER                      (0x4007C014U) /**< \brief (QSPI) Interrupt Enable Register */
  #define REG_QSPI_IDR                      (0x4007C018U) /**< \brief (QSPI) Interrupt Disable Register */
  #define REG_QSPI_IMR                      (0x4007C01CU) /**< \brief (QSPI) Interrupt Mask Register */
  #define REG_QSPI_SCR                      (0x4007C020U) /**< \brief (QSPI) Serial Clock Register */
  #define REG_QSPI_IAR                      (0x4007C030U) /**< \brief (QSPI) Instruction Address Register */
  #define REG_QSPI_ICR                      (0x4007C034U) /**< \brief (QSPI) Instruction Code Register */
  #define REG_QSPI_IFR                      (0x4007C038U) /**< \brief (QSPI) Instruction Frame Register */
  #define REG_QSPI_SMR                      (0x4007C040U) /**< \brief (QSPI) Scrambling Mode Register */
  #define REG_QSPI_SKR                      (0x4007C044U) /**< \brief (QSPI) Scrambling Key Register */
  #define REG_QSPI_WPMR                     (0x4007C0E4U) /**< \brief (QSPI) Write Protection Mode Register */
  #define REG_QSPI_WPSR                     (0x4007C0E8U) /**< \brief (QSPI) Write Protection Status Register */
  #define REG_QSPI_VERSION                  (0x4007C0FCU) /**< \brief (QSPI) Version Register */
#else
  #define REG_QSPI_CR      (*(__O  uint32_t*)0x4007C000U) /**< \brief (QSPI) Control Register */
  #define REG_QSPI_MR      (*(__IO uint32_t*)0x4007C004U) /**< \brief (QSPI) Mode Register */
  #define REG_QSPI_RDR     (*(__I  uint32_t*)0x4007C008U) /**< \brief (QSPI) Receive Data Register */
  #define REG_QSPI_TDR     (*(__O  uint32_t*)0x4007C00CU) /**< \brief (QSPI) Transmit Data Register */
  #define REG_QSPI_SR      (*(__I  uint32_t*)0x4007C010U) /**< \brief (QSPI) Status Register */
  #define REG_QSPI_IER     (*(__O  uint32_t*)0x4007C014U) /**< \brief (QSPI) Interrupt Enable Register */
  #define REG_QSPI_IDR     (*(__O  uint32_t*)0x4007C018U) /**< \brief (QSPI) Interrupt Disable Register */
  #define REG_QSPI_IMR     (*(__I  uint32_t*)0x4007C01CU) /**< \brief (QSPI) Interrupt Mask Register */
  #define REG_QSPI_SCR     (*(__IO uint32_t*)0x4007C020U) /**< \brief (QSPI) Serial Clock Register */
  #define REG_QSPI_IAR     (*(__IO uint32_t*)0x4007C030U) /**< \brief (QSPI) Instruction Address Register */
  #define REG_QSPI_ICR     (*(__IO uint32_t*)0x4007C034U) /**< \brief (QSPI) Instruction Code Register */
  #define REG_QSPI_IFR     (*(__IO uint32_t*)0x4007C038U) /**< \brief (QSPI) Instruction Frame Register */
  #define REG_QSPI_SMR     (*(__IO uint32_t*)0x4007C040U) /**< \brief (QSPI) Scrambling Mode Register */
  #define REG_QSPI_SKR     (*(__O  uint32_t*)0x4007C044U) /**< \brief (QSPI) Scrambling Key Register */
  #define REG_QSPI_WPMR    (*(__IO uint32_t*)0x4007C0E4U) /**< \brief (QSPI) Write Protection Mode Register */
  #define REG_QSPI_WPSR    (*(__I  uint32_t*)0x4007C0E8U) /**< \brief (QSPI) Write Protection Status Register */
  #define REG_QSPI_VERSION (*(__I  uint32_t*)0x4007C0FCU) /**< \brief (QSPI) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_QSPI_INSTANCE_ */
