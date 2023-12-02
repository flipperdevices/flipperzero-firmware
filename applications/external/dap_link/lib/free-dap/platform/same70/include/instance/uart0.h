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

#ifndef _SAME70_UART0_INSTANCE_
#define _SAME70_UART0_INSTANCE_

/* ========== Register definition for UART0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_UART0_CR                       (0x400E0800U) /**< \brief (UART0) Control Register */
  #define REG_UART0_MR                       (0x400E0804U) /**< \brief (UART0) Mode Register */
  #define REG_UART0_IER                      (0x400E0808U) /**< \brief (UART0) Interrupt Enable Register */
  #define REG_UART0_IDR                      (0x400E080CU) /**< \brief (UART0) Interrupt Disable Register */
  #define REG_UART0_IMR                      (0x400E0810U) /**< \brief (UART0) Interrupt Mask Register */
  #define REG_UART0_SR                       (0x400E0814U) /**< \brief (UART0) Status Register */
  #define REG_UART0_RHR                      (0x400E0818U) /**< \brief (UART0) Receive Holding Register */
  #define REG_UART0_THR                      (0x400E081CU) /**< \brief (UART0) Transmit Holding Register */
  #define REG_UART0_BRGR                     (0x400E0820U) /**< \brief (UART0) Baud Rate Generator Register */
  #define REG_UART0_CMPR                     (0x400E0824U) /**< \brief (UART0) Comparison Register */
  #define REG_UART0_WPMR                     (0x400E08E4U) /**< \brief (UART0) Write Protection Mode Register */
  #define REG_UART0_VERSION                  (0x400E08FCU) /**< \brief (UART0) Version Register */
#else
  #define REG_UART0_CR      (*(__O  uint32_t*)0x400E0800U) /**< \brief (UART0) Control Register */
  #define REG_UART0_MR      (*(__IO uint32_t*)0x400E0804U) /**< \brief (UART0) Mode Register */
  #define REG_UART0_IER     (*(__O  uint32_t*)0x400E0808U) /**< \brief (UART0) Interrupt Enable Register */
  #define REG_UART0_IDR     (*(__O  uint32_t*)0x400E080CU) /**< \brief (UART0) Interrupt Disable Register */
  #define REG_UART0_IMR     (*(__I  uint32_t*)0x400E0810U) /**< \brief (UART0) Interrupt Mask Register */
  #define REG_UART0_SR      (*(__I  uint32_t*)0x400E0814U) /**< \brief (UART0) Status Register */
  #define REG_UART0_RHR     (*(__I  uint32_t*)0x400E0818U) /**< \brief (UART0) Receive Holding Register */
  #define REG_UART0_THR     (*(__O  uint32_t*)0x400E081CU) /**< \brief (UART0) Transmit Holding Register */
  #define REG_UART0_BRGR    (*(__IO uint32_t*)0x400E0820U) /**< \brief (UART0) Baud Rate Generator Register */
  #define REG_UART0_CMPR    (*(__IO uint32_t*)0x400E0824U) /**< \brief (UART0) Comparison Register */
  #define REG_UART0_WPMR    (*(__IO uint32_t*)0x400E08E4U) /**< \brief (UART0) Write Protection Mode Register */
  #define REG_UART0_VERSION (*(__I  uint32_t*)0x400E08FCU) /**< \brief (UART0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_UART0_INSTANCE_ */
