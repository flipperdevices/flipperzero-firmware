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
#ifndef _SAME70_UART3_INSTANCE_
#define _SAME70_UART3_INSTANCE_

/* ========== Register definition for UART3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_UART3_CR                       (0x400E1C00U) /**< \brief (UART3) Control Register */
  #define REG_UART3_MR                       (0x400E1C04U) /**< \brief (UART3) Mode Register */
  #define REG_UART3_IER                      (0x400E1C08U) /**< \brief (UART3) Interrupt Enable Register */
  #define REG_UART3_IDR                      (0x400E1C0CU) /**< \brief (UART3) Interrupt Disable Register */
  #define REG_UART3_IMR                      (0x400E1C10U) /**< \brief (UART3) Interrupt Mask Register */
  #define REG_UART3_SR                       (0x400E1C14U) /**< \brief (UART3) Status Register */
  #define REG_UART3_RHR                      (0x400E1C18U) /**< \brief (UART3) Receive Holding Register */
  #define REG_UART3_THR                      (0x400E1C1CU) /**< \brief (UART3) Transmit Holding Register */
  #define REG_UART3_BRGR                     (0x400E1C20U) /**< \brief (UART3) Baud Rate Generator Register */
  #define REG_UART3_CMPR                     (0x400E1C24U) /**< \brief (UART3) Comparison Register */
  #define REG_UART3_WPMR                     (0x400E1CE4U) /**< \brief (UART3) Write Protection Mode Register */
  #define REG_UART3_VERSION                  (0x400E1CFCU) /**< \brief (UART3) Version Register */
#else
  #define REG_UART3_CR      (*(__O  uint32_t*)0x400E1C00U) /**< \brief (UART3) Control Register */
  #define REG_UART3_MR      (*(__IO uint32_t*)0x400E1C04U) /**< \brief (UART3) Mode Register */
  #define REG_UART3_IER     (*(__O  uint32_t*)0x400E1C08U) /**< \brief (UART3) Interrupt Enable Register */
  #define REG_UART3_IDR     (*(__O  uint32_t*)0x400E1C0CU) /**< \brief (UART3) Interrupt Disable Register */
  #define REG_UART3_IMR     (*(__I  uint32_t*)0x400E1C10U) /**< \brief (UART3) Interrupt Mask Register */
  #define REG_UART3_SR      (*(__I  uint32_t*)0x400E1C14U) /**< \brief (UART3) Status Register */
  #define REG_UART3_RHR     (*(__I  uint32_t*)0x400E1C18U) /**< \brief (UART3) Receive Holding Register */
  #define REG_UART3_THR     (*(__O  uint32_t*)0x400E1C1CU) /**< \brief (UART3) Transmit Holding Register */
  #define REG_UART3_BRGR    (*(__IO uint32_t*)0x400E1C20U) /**< \brief (UART3) Baud Rate Generator Register */
  #define REG_UART3_CMPR    (*(__IO uint32_t*)0x400E1C24U) /**< \brief (UART3) Comparison Register */
  #define REG_UART3_WPMR    (*(__IO uint32_t*)0x400E1CE4U) /**< \brief (UART3) Write Protection Mode Register */
  #define REG_UART3_VERSION (*(__I  uint32_t*)0x400E1CFCU) /**< \brief (UART3) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_UART3_INSTANCE_ */
