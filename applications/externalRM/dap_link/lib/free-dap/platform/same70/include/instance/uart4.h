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

#ifndef _SAME70_UART4_INSTANCE_
#define _SAME70_UART4_INSTANCE_

/* ========== Register definition for UART4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_UART4_CR                       (0x400E1E00U) /**< \brief (UART4) Control Register */
  #define REG_UART4_MR                       (0x400E1E04U) /**< \brief (UART4) Mode Register */
  #define REG_UART4_IER                      (0x400E1E08U) /**< \brief (UART4) Interrupt Enable Register */
  #define REG_UART4_IDR                      (0x400E1E0CU) /**< \brief (UART4) Interrupt Disable Register */
  #define REG_UART4_IMR                      (0x400E1E10U) /**< \brief (UART4) Interrupt Mask Register */
  #define REG_UART4_SR                       (0x400E1E14U) /**< \brief (UART4) Status Register */
  #define REG_UART4_RHR                      (0x400E1E18U) /**< \brief (UART4) Receive Holding Register */
  #define REG_UART4_THR                      (0x400E1E1CU) /**< \brief (UART4) Transmit Holding Register */
  #define REG_UART4_BRGR                     (0x400E1E20U) /**< \brief (UART4) Baud Rate Generator Register */
  #define REG_UART4_CMPR                     (0x400E1E24U) /**< \brief (UART4) Comparison Register */
  #define REG_UART4_WPMR                     (0x400E1EE4U) /**< \brief (UART4) Write Protection Mode Register */
  #define REG_UART4_VERSION                  (0x400E1EFCU) /**< \brief (UART4) Version Register */
#else
  #define REG_UART4_CR      (*(__O  uint32_t*)0x400E1E00U) /**< \brief (UART4) Control Register */
  #define REG_UART4_MR      (*(__IO uint32_t*)0x400E1E04U) /**< \brief (UART4) Mode Register */
  #define REG_UART4_IER     (*(__O  uint32_t*)0x400E1E08U) /**< \brief (UART4) Interrupt Enable Register */
  #define REG_UART4_IDR     (*(__O  uint32_t*)0x400E1E0CU) /**< \brief (UART4) Interrupt Disable Register */
  #define REG_UART4_IMR     (*(__I  uint32_t*)0x400E1E10U) /**< \brief (UART4) Interrupt Mask Register */
  #define REG_UART4_SR      (*(__I  uint32_t*)0x400E1E14U) /**< \brief (UART4) Status Register */
  #define REG_UART4_RHR     (*(__I  uint32_t*)0x400E1E18U) /**< \brief (UART4) Receive Holding Register */
  #define REG_UART4_THR     (*(__O  uint32_t*)0x400E1E1CU) /**< \brief (UART4) Transmit Holding Register */
  #define REG_UART4_BRGR    (*(__IO uint32_t*)0x400E1E20U) /**< \brief (UART4) Baud Rate Generator Register */
  #define REG_UART4_CMPR    (*(__IO uint32_t*)0x400E1E24U) /**< \brief (UART4) Comparison Register */
  #define REG_UART4_WPMR    (*(__IO uint32_t*)0x400E1EE4U) /**< \brief (UART4) Write Protection Mode Register */
  #define REG_UART4_VERSION (*(__I  uint32_t*)0x400E1EFCU) /**< \brief (UART4) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_UART4_INSTANCE_ */
