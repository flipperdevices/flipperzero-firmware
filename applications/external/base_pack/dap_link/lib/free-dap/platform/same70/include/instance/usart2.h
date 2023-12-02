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

#ifndef _SAME70_USART2_INSTANCE_
#define _SAME70_USART2_INSTANCE_

/* ========== Register definition for USART2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_USART2_CR                        (0x4002C000U) /**< \brief (USART2) Control Register */
  #define REG_USART2_MR                        (0x4002C004U) /**< \brief (USART2) Mode Register */
  #define REG_USART2_IER                       (0x4002C008U) /**< \brief (USART2) Interrupt Enable Register */
  #define REG_USART2_IDR                       (0x4002C00CU) /**< \brief (USART2) Interrupt Disable Register */
  #define REG_USART2_IMR                       (0x4002C010U) /**< \brief (USART2) Interrupt Mask Register */
  #define REG_USART2_CSR                       (0x4002C014U) /**< \brief (USART2) Channel Status Register */
  #define REG_USART2_RHR                       (0x4002C018U) /**< \brief (USART2) Receive Holding Register */
  #define REG_USART2_THR                       (0x4002C01CU) /**< \brief (USART2) Transmit Holding Register */
  #define REG_USART2_BRGR                      (0x4002C020U) /**< \brief (USART2) Baud Rate Generator Register */
  #define REG_USART2_RTOR                      (0x4002C024U) /**< \brief (USART2) Receiver Time-out Register */
  #define REG_USART2_TTGR                      (0x4002C028U) /**< \brief (USART2) Transmitter Timeguard Register */
  #define REG_USART2_FIDI                      (0x4002C040U) /**< \brief (USART2) FI DI Ratio Register */
  #define REG_USART2_NER                       (0x4002C044U) /**< \brief (USART2) Number of Errors Register */
  #define REG_USART2_IF                        (0x4002C04CU) /**< \brief (USART2) IrDA Filter Register */
  #define REG_USART2_MAN                       (0x4002C050U) /**< \brief (USART2) Manchester Configuration Register */
  #define REG_USART2_LINMR                     (0x4002C054U) /**< \brief (USART2) LIN Mode Register */
  #define REG_USART2_LINIR                     (0x4002C058U) /**< \brief (USART2) LIN Identifier Register */
  #define REG_USART2_LINBRR                    (0x4002C05CU) /**< \brief (USART2) LIN Baud Rate Register */
  #define REG_USART2_LONMR                     (0x4002C060U) /**< \brief (USART2) LON Mode Register */
  #define REG_USART2_LONPR                     (0x4002C064U) /**< \brief (USART2) LON Preamble Register */
  #define REG_USART2_LONDL                     (0x4002C068U) /**< \brief (USART2) LON Data Length Register */
  #define REG_USART2_LONL2HDR                  (0x4002C06CU) /**< \brief (USART2) LON L2HDR Register */
  #define REG_USART2_LONBL                     (0x4002C070U) /**< \brief (USART2) LON Backlog Register */
  #define REG_USART2_LONB1TX                   (0x4002C074U) /**< \brief (USART2) LON Beta1 Tx Register */
  #define REG_USART2_LONB1RX                   (0x4002C078U) /**< \brief (USART2) LON Beta1 Rx Register */
  #define REG_USART2_LONPRIO                   (0x4002C07CU) /**< \brief (USART2) LON Priority Register */
  #define REG_USART2_IDTTX                     (0x4002C080U) /**< \brief (USART2) LON IDT Tx Register */
  #define REG_USART2_IDTRX                     (0x4002C084U) /**< \brief (USART2) LON IDT Rx Register */
  #define REG_USART2_ICDIFF                    (0x4002C088U) /**< \brief (USART2) IC DIFF Register */
  #define REG_USART2_WPMR                      (0x4002C0E4U) /**< \brief (USART2) Write Protection Mode Register */
  #define REG_USART2_WPSR                      (0x4002C0E8U) /**< \brief (USART2) Write Protection Status Register */
  #define REG_USART2_VERSION                   (0x4002C0FCU) /**< \brief (USART2) Version Register */
#else
  #define REG_USART2_CR       (*(__O  uint32_t*)0x4002C000U) /**< \brief (USART2) Control Register */
  #define REG_USART2_MR       (*(__IO uint32_t*)0x4002C004U) /**< \brief (USART2) Mode Register */
  #define REG_USART2_IER      (*(__O  uint32_t*)0x4002C008U) /**< \brief (USART2) Interrupt Enable Register */
  #define REG_USART2_IDR      (*(__O  uint32_t*)0x4002C00CU) /**< \brief (USART2) Interrupt Disable Register */
  #define REG_USART2_IMR      (*(__I  uint32_t*)0x4002C010U) /**< \brief (USART2) Interrupt Mask Register */
  #define REG_USART2_CSR      (*(__I  uint32_t*)0x4002C014U) /**< \brief (USART2) Channel Status Register */
  #define REG_USART2_RHR      (*(__I  uint32_t*)0x4002C018U) /**< \brief (USART2) Receive Holding Register */
  #define REG_USART2_THR      (*(__O  uint32_t*)0x4002C01CU) /**< \brief (USART2) Transmit Holding Register */
  #define REG_USART2_BRGR     (*(__IO uint32_t*)0x4002C020U) /**< \brief (USART2) Baud Rate Generator Register */
  #define REG_USART2_RTOR     (*(__IO uint32_t*)0x4002C024U) /**< \brief (USART2) Receiver Time-out Register */
  #define REG_USART2_TTGR     (*(__IO uint32_t*)0x4002C028U) /**< \brief (USART2) Transmitter Timeguard Register */
  #define REG_USART2_FIDI     (*(__IO uint32_t*)0x4002C040U) /**< \brief (USART2) FI DI Ratio Register */
  #define REG_USART2_NER      (*(__I  uint32_t*)0x4002C044U) /**< \brief (USART2) Number of Errors Register */
  #define REG_USART2_IF       (*(__IO uint32_t*)0x4002C04CU) /**< \brief (USART2) IrDA Filter Register */
  #define REG_USART2_MAN      (*(__IO uint32_t*)0x4002C050U) /**< \brief (USART2) Manchester Configuration Register */
  #define REG_USART2_LINMR    (*(__IO uint32_t*)0x4002C054U) /**< \brief (USART2) LIN Mode Register */
  #define REG_USART2_LINIR    (*(__IO uint32_t*)0x4002C058U) /**< \brief (USART2) LIN Identifier Register */
  #define REG_USART2_LINBRR   (*(__I  uint32_t*)0x4002C05CU) /**< \brief (USART2) LIN Baud Rate Register */
  #define REG_USART2_LONMR    (*(__IO uint32_t*)0x4002C060U) /**< \brief (USART2) LON Mode Register */
  #define REG_USART2_LONPR    (*(__IO uint32_t*)0x4002C064U) /**< \brief (USART2) LON Preamble Register */
  #define REG_USART2_LONDL    (*(__IO uint32_t*)0x4002C068U) /**< \brief (USART2) LON Data Length Register */
  #define REG_USART2_LONL2HDR (*(__IO uint32_t*)0x4002C06CU) /**< \brief (USART2) LON L2HDR Register */
  #define REG_USART2_LONBL    (*(__I  uint32_t*)0x4002C070U) /**< \brief (USART2) LON Backlog Register */
  #define REG_USART2_LONB1TX  (*(__IO uint32_t*)0x4002C074U) /**< \brief (USART2) LON Beta1 Tx Register */
  #define REG_USART2_LONB1RX  (*(__IO uint32_t*)0x4002C078U) /**< \brief (USART2) LON Beta1 Rx Register */
  #define REG_USART2_LONPRIO  (*(__IO uint32_t*)0x4002C07CU) /**< \brief (USART2) LON Priority Register */
  #define REG_USART2_IDTTX    (*(__IO uint32_t*)0x4002C080U) /**< \brief (USART2) LON IDT Tx Register */
  #define REG_USART2_IDTRX    (*(__IO uint32_t*)0x4002C084U) /**< \brief (USART2) LON IDT Rx Register */
  #define REG_USART2_ICDIFF   (*(__IO uint32_t*)0x4002C088U) /**< \brief (USART2) IC DIFF Register */
  #define REG_USART2_WPMR     (*(__IO uint32_t*)0x4002C0E4U) /**< \brief (USART2) Write Protection Mode Register */
  #define REG_USART2_WPSR     (*(__I  uint32_t*)0x4002C0E8U) /**< \brief (USART2) Write Protection Status Register */
  #define REG_USART2_VERSION  (*(__I  uint32_t*)0x4002C0FCU) /**< \brief (USART2) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_USART2_INSTANCE_ */
