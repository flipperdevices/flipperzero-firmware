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

#ifndef _SAME70_USART0_INSTANCE_
#define _SAME70_USART0_INSTANCE_

/* ========== Register definition for USART0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_USART0_CR                        (0x40024000U) /**< \brief (USART0) Control Register */
  #define REG_USART0_MR                        (0x40024004U) /**< \brief (USART0) Mode Register */
  #define REG_USART0_IER                       (0x40024008U) /**< \brief (USART0) Interrupt Enable Register */
  #define REG_USART0_IDR                       (0x4002400CU) /**< \brief (USART0) Interrupt Disable Register */
  #define REG_USART0_IMR                       (0x40024010U) /**< \brief (USART0) Interrupt Mask Register */
  #define REG_USART0_CSR                       (0x40024014U) /**< \brief (USART0) Channel Status Register */
  #define REG_USART0_RHR                       (0x40024018U) /**< \brief (USART0) Receive Holding Register */
  #define REG_USART0_THR                       (0x4002401CU) /**< \brief (USART0) Transmit Holding Register */
  #define REG_USART0_BRGR                      (0x40024020U) /**< \brief (USART0) Baud Rate Generator Register */
  #define REG_USART0_RTOR                      (0x40024024U) /**< \brief (USART0) Receiver Time-out Register */
  #define REG_USART0_TTGR                      (0x40024028U) /**< \brief (USART0) Transmitter Timeguard Register */
  #define REG_USART0_FIDI                      (0x40024040U) /**< \brief (USART0) FI DI Ratio Register */
  #define REG_USART0_NER                       (0x40024044U) /**< \brief (USART0) Number of Errors Register */
  #define REG_USART0_IF                        (0x4002404CU) /**< \brief (USART0) IrDA Filter Register */
  #define REG_USART0_MAN                       (0x40024050U) /**< \brief (USART0) Manchester Configuration Register */
  #define REG_USART0_LINMR                     (0x40024054U) /**< \brief (USART0) LIN Mode Register */
  #define REG_USART0_LINIR                     (0x40024058U) /**< \brief (USART0) LIN Identifier Register */
  #define REG_USART0_LINBRR                    (0x4002405CU) /**< \brief (USART0) LIN Baud Rate Register */
  #define REG_USART0_LONMR                     (0x40024060U) /**< \brief (USART0) LON Mode Register */
  #define REG_USART0_LONPR                     (0x40024064U) /**< \brief (USART0) LON Preamble Register */
  #define REG_USART0_LONDL                     (0x40024068U) /**< \brief (USART0) LON Data Length Register */
  #define REG_USART0_LONL2HDR                  (0x4002406CU) /**< \brief (USART0) LON L2HDR Register */
  #define REG_USART0_LONBL                     (0x40024070U) /**< \brief (USART0) LON Backlog Register */
  #define REG_USART0_LONB1TX                   (0x40024074U) /**< \brief (USART0) LON Beta1 Tx Register */
  #define REG_USART0_LONB1RX                   (0x40024078U) /**< \brief (USART0) LON Beta1 Rx Register */
  #define REG_USART0_LONPRIO                   (0x4002407CU) /**< \brief (USART0) LON Priority Register */
  #define REG_USART0_IDTTX                     (0x40024080U) /**< \brief (USART0) LON IDT Tx Register */
  #define REG_USART0_IDTRX                     (0x40024084U) /**< \brief (USART0) LON IDT Rx Register */
  #define REG_USART0_ICDIFF                    (0x40024088U) /**< \brief (USART0) IC DIFF Register */
  #define REG_USART0_WPMR                      (0x400240E4U) /**< \brief (USART0) Write Protection Mode Register */
  #define REG_USART0_WPSR                      (0x400240E8U) /**< \brief (USART0) Write Protection Status Register */
  #define REG_USART0_VERSION                   (0x400240FCU) /**< \brief (USART0) Version Register */
#else
  #define REG_USART0_CR       (*(__O  uint32_t*)0x40024000U) /**< \brief (USART0) Control Register */
  #define REG_USART0_MR       (*(__IO uint32_t*)0x40024004U) /**< \brief (USART0) Mode Register */
  #define REG_USART0_IER      (*(__O  uint32_t*)0x40024008U) /**< \brief (USART0) Interrupt Enable Register */
  #define REG_USART0_IDR      (*(__O  uint32_t*)0x4002400CU) /**< \brief (USART0) Interrupt Disable Register */
  #define REG_USART0_IMR      (*(__I  uint32_t*)0x40024010U) /**< \brief (USART0) Interrupt Mask Register */
  #define REG_USART0_CSR      (*(__I  uint32_t*)0x40024014U) /**< \brief (USART0) Channel Status Register */
  #define REG_USART0_RHR      (*(__I  uint32_t*)0x40024018U) /**< \brief (USART0) Receive Holding Register */
  #define REG_USART0_THR      (*(__O  uint32_t*)0x4002401CU) /**< \brief (USART0) Transmit Holding Register */
  #define REG_USART0_BRGR     (*(__IO uint32_t*)0x40024020U) /**< \brief (USART0) Baud Rate Generator Register */
  #define REG_USART0_RTOR     (*(__IO uint32_t*)0x40024024U) /**< \brief (USART0) Receiver Time-out Register */
  #define REG_USART0_TTGR     (*(__IO uint32_t*)0x40024028U) /**< \brief (USART0) Transmitter Timeguard Register */
  #define REG_USART0_FIDI     (*(__IO uint32_t*)0x40024040U) /**< \brief (USART0) FI DI Ratio Register */
  #define REG_USART0_NER      (*(__I  uint32_t*)0x40024044U) /**< \brief (USART0) Number of Errors Register */
  #define REG_USART0_IF       (*(__IO uint32_t*)0x4002404CU) /**< \brief (USART0) IrDA Filter Register */
  #define REG_USART0_MAN      (*(__IO uint32_t*)0x40024050U) /**< \brief (USART0) Manchester Configuration Register */
  #define REG_USART0_LINMR    (*(__IO uint32_t*)0x40024054U) /**< \brief (USART0) LIN Mode Register */
  #define REG_USART0_LINIR    (*(__IO uint32_t*)0x40024058U) /**< \brief (USART0) LIN Identifier Register */
  #define REG_USART0_LINBRR   (*(__I  uint32_t*)0x4002405CU) /**< \brief (USART0) LIN Baud Rate Register */
  #define REG_USART0_LONMR    (*(__IO uint32_t*)0x40024060U) /**< \brief (USART0) LON Mode Register */
  #define REG_USART0_LONPR    (*(__IO uint32_t*)0x40024064U) /**< \brief (USART0) LON Preamble Register */
  #define REG_USART0_LONDL    (*(__IO uint32_t*)0x40024068U) /**< \brief (USART0) LON Data Length Register */
  #define REG_USART0_LONL2HDR (*(__IO uint32_t*)0x4002406CU) /**< \brief (USART0) LON L2HDR Register */
  #define REG_USART0_LONBL    (*(__I  uint32_t*)0x40024070U) /**< \brief (USART0) LON Backlog Register */
  #define REG_USART0_LONB1TX  (*(__IO uint32_t*)0x40024074U) /**< \brief (USART0) LON Beta1 Tx Register */
  #define REG_USART0_LONB1RX  (*(__IO uint32_t*)0x40024078U) /**< \brief (USART0) LON Beta1 Rx Register */
  #define REG_USART0_LONPRIO  (*(__IO uint32_t*)0x4002407CU) /**< \brief (USART0) LON Priority Register */
  #define REG_USART0_IDTTX    (*(__IO uint32_t*)0x40024080U) /**< \brief (USART0) LON IDT Tx Register */
  #define REG_USART0_IDTRX    (*(__IO uint32_t*)0x40024084U) /**< \brief (USART0) LON IDT Rx Register */
  #define REG_USART0_ICDIFF   (*(__IO uint32_t*)0x40024088U) /**< \brief (USART0) IC DIFF Register */
  #define REG_USART0_WPMR     (*(__IO uint32_t*)0x400240E4U) /**< \brief (USART0) Write Protection Mode Register */
  #define REG_USART0_WPSR     (*(__I  uint32_t*)0x400240E8U) /**< \brief (USART0) Write Protection Status Register */
  #define REG_USART0_VERSION  (*(__I  uint32_t*)0x400240FCU) /**< \brief (USART0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_USART0_INSTANCE_ */
