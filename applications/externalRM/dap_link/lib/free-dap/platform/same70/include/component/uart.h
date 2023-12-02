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

#ifndef _SAME70_UART_COMPONENT_
#define _SAME70_UART_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Universal Asynchronous Receiver Transmitter */
/* ============================================================================= */
/** \addtogroup SAME70_UART Universal Asynchronous Receiver Transmitter */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Uart hardware registers */
typedef struct {
  __O  uint32_t UART_CR;       /**< \brief (Uart Offset: 0x0000) Control Register */
  __IO uint32_t UART_MR;       /**< \brief (Uart Offset: 0x0004) Mode Register */
  __O  uint32_t UART_IER;      /**< \brief (Uart Offset: 0x0008) Interrupt Enable Register */
  __O  uint32_t UART_IDR;      /**< \brief (Uart Offset: 0x000C) Interrupt Disable Register */
  __I  uint32_t UART_IMR;      /**< \brief (Uart Offset: 0x0010) Interrupt Mask Register */
  __I  uint32_t UART_SR;       /**< \brief (Uart Offset: 0x0014) Status Register */
  __I  uint32_t UART_RHR;      /**< \brief (Uart Offset: 0x0018) Receive Holding Register */
  __O  uint32_t UART_THR;      /**< \brief (Uart Offset: 0x001C) Transmit Holding Register */
  __IO uint32_t UART_BRGR;     /**< \brief (Uart Offset: 0x0020) Baud Rate Generator Register */
  __IO uint32_t UART_CMPR;     /**< \brief (Uart Offset: 0x0024) Comparison Register */
  __I  uint32_t Reserved1[47];
  __IO uint32_t UART_WPMR;     /**< \brief (Uart Offset: 0x00E4) Write Protection Mode Register */
  __I  uint32_t Reserved2[5];
  __I  uint32_t UART_VERSION;  /**< \brief (Uart Offset: 0x00FC) Version Register */
} Uart;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- UART_CR : (UART Offset: 0x0000) Control Register -------- */
#define UART_CR_RSTRX (0x1u << 2) /**< \brief (UART_CR) Reset Receiver */
#define UART_CR_RSTTX (0x1u << 3) /**< \brief (UART_CR) Reset Transmitter */
#define UART_CR_RXEN (0x1u << 4) /**< \brief (UART_CR) Receiver Enable */
#define UART_CR_RXDIS (0x1u << 5) /**< \brief (UART_CR) Receiver Disable */
#define UART_CR_TXEN (0x1u << 6) /**< \brief (UART_CR) Transmitter Enable */
#define UART_CR_TXDIS (0x1u << 7) /**< \brief (UART_CR) Transmitter Disable */
#define UART_CR_RSTSTA (0x1u << 8) /**< \brief (UART_CR) Reset Status */
#define UART_CR_REQCLR (0x1u << 12) /**< \brief (UART_CR) Request Clear */
#define UART_CR_DBGE (0x1u << 15) /**< \brief (UART_CR) Debug Enable */
/* -------- UART_MR : (UART Offset: 0x0004) Mode Register -------- */
#define UART_MR_FILTER (0x1u << 4) /**< \brief (UART_MR) Receiver Digital Filter */
#define   UART_MR_FILTER_DISABLED (0x0u << 4) /**< \brief (UART_MR) UART does not filter the receive line. */
#define   UART_MR_FILTER_ENABLED (0x1u << 4) /**< \brief (UART_MR) UART filters the receive line using a three-sample filter (16x-bit clock) (2 over 3 majority). */
#define UART_MR_PAR_Pos 9
#define UART_MR_PAR_Msk (0x7u << UART_MR_PAR_Pos) /**< \brief (UART_MR) Parity Type */
#define UART_MR_PAR(value) ((UART_MR_PAR_Msk & ((value) << UART_MR_PAR_Pos)))
#define   UART_MR_PAR_EVEN (0x0u << 9) /**< \brief (UART_MR) Even Parity */
#define   UART_MR_PAR_ODD (0x1u << 9) /**< \brief (UART_MR) Odd Parity */
#define   UART_MR_PAR_SPACE (0x2u << 9) /**< \brief (UART_MR) Space: parity forced to 0 */
#define   UART_MR_PAR_MARK (0x3u << 9) /**< \brief (UART_MR) Mark: parity forced to 1 */
#define   UART_MR_PAR_NO (0x4u << 9) /**< \brief (UART_MR) No parity */
#define UART_MR_BRSRCCK (0x1u << 12) /**< \brief (UART_MR) Baud Rate Source Clock */
#define   UART_MR_BRSRCCK_PERIPH_CLK (0x0u << 12) /**< \brief (UART_MR) The baud rate is driven by the peripheral clock */
#define   UART_MR_BRSRCCK_PMC_PCK (0x1u << 12) /**< \brief (UART_MR) The baud rate is driven by a PMC programmable clock PCK (see section Power Management Controller (PMC)). */
#define UART_MR_CHMODE_Pos 14
#define UART_MR_CHMODE_Msk (0x3u << UART_MR_CHMODE_Pos) /**< \brief (UART_MR) Channel Mode */
#define UART_MR_CHMODE(value) ((UART_MR_CHMODE_Msk & ((value) << UART_MR_CHMODE_Pos)))
#define   UART_MR_CHMODE_NORMAL (0x0u << 14) /**< \brief (UART_MR) Normal mode */
#define   UART_MR_CHMODE_AUTOMATIC (0x1u << 14) /**< \brief (UART_MR) Automatic echo */
#define   UART_MR_CHMODE_LOCAL_LOOPBACK (0x2u << 14) /**< \brief (UART_MR) Local loopback */
#define   UART_MR_CHMODE_REMOTE_LOOPBACK (0x3u << 14) /**< \brief (UART_MR) Remote loopback */
/* -------- UART_IER : (UART Offset: 0x0008) Interrupt Enable Register -------- */
#define UART_IER_RXRDY (0x1u << 0) /**< \brief (UART_IER) Enable RXRDY Interrupt */
#define UART_IER_TXRDY (0x1u << 1) /**< \brief (UART_IER) Enable TXRDY Interrupt */
#define UART_IER_OVRE (0x1u << 5) /**< \brief (UART_IER) Enable Overrun Error Interrupt */
#define UART_IER_FRAME (0x1u << 6) /**< \brief (UART_IER) Enable Framing Error Interrupt */
#define UART_IER_PARE (0x1u << 7) /**< \brief (UART_IER) Enable Parity Error Interrupt */
#define UART_IER_TXEMPTY (0x1u << 9) /**< \brief (UART_IER) Enable TXEMPTY Interrupt */
#define UART_IER_CMP (0x1u << 15) /**< \brief (UART_IER) Enable Comparison Interrupt */
/* -------- UART_IDR : (UART Offset: 0x000C) Interrupt Disable Register -------- */
#define UART_IDR_RXRDY (0x1u << 0) /**< \brief (UART_IDR) Disable RXRDY Interrupt */
#define UART_IDR_TXRDY (0x1u << 1) /**< \brief (UART_IDR) Disable TXRDY Interrupt */
#define UART_IDR_OVRE (0x1u << 5) /**< \brief (UART_IDR) Disable Overrun Error Interrupt */
#define UART_IDR_FRAME (0x1u << 6) /**< \brief (UART_IDR) Disable Framing Error Interrupt */
#define UART_IDR_PARE (0x1u << 7) /**< \brief (UART_IDR) Disable Parity Error Interrupt */
#define UART_IDR_TXEMPTY (0x1u << 9) /**< \brief (UART_IDR) Disable TXEMPTY Interrupt */
#define UART_IDR_CMP (0x1u << 15) /**< \brief (UART_IDR) Disable Comparison Interrupt */
/* -------- UART_IMR : (UART Offset: 0x0010) Interrupt Mask Register -------- */
#define UART_IMR_RXRDY (0x1u << 0) /**< \brief (UART_IMR) Mask RXRDY Interrupt */
#define UART_IMR_TXRDY (0x1u << 1) /**< \brief (UART_IMR) Disable TXRDY Interrupt */
#define UART_IMR_OVRE (0x1u << 5) /**< \brief (UART_IMR) Mask Overrun Error Interrupt */
#define UART_IMR_FRAME (0x1u << 6) /**< \brief (UART_IMR) Mask Framing Error Interrupt */
#define UART_IMR_PARE (0x1u << 7) /**< \brief (UART_IMR) Mask Parity Error Interrupt */
#define UART_IMR_TXEMPTY (0x1u << 9) /**< \brief (UART_IMR) Mask TXEMPTY Interrupt */
#define UART_IMR_CMP (0x1u << 15) /**< \brief (UART_IMR) Mask Comparison Interrupt */
/* -------- UART_SR : (UART Offset: 0x0014) Status Register -------- */
#define UART_SR_RXRDY (0x1u << 0) /**< \brief (UART_SR) Receiver Ready */
#define UART_SR_TXRDY (0x1u << 1) /**< \brief (UART_SR) Transmitter Ready */
#define UART_SR_OVRE (0x1u << 5) /**< \brief (UART_SR) Overrun Error */
#define UART_SR_FRAME (0x1u << 6) /**< \brief (UART_SR) Framing Error */
#define UART_SR_PARE (0x1u << 7) /**< \brief (UART_SR) Parity Error */
#define UART_SR_TXEMPTY (0x1u << 9) /**< \brief (UART_SR) Transmitter Empty */
#define UART_SR_CMP (0x1u << 15) /**< \brief (UART_SR) Comparison Match */
#define UART_SR_SWES (0x1u << 21) /**< \brief (UART_SR) SleepWalking Enable Status */
#define UART_SR_CLKREQ (0x1u << 22) /**< \brief (UART_SR) Clock Request */
#define UART_SR_WKUPREQ (0x1u << 23) /**< \brief (UART_SR) Wake-Up Request */
/* -------- UART_RHR : (UART Offset: 0x0018) Receive Holding Register -------- */
#define UART_RHR_RXCHR_Pos 0
#define UART_RHR_RXCHR_Msk (0xffu << UART_RHR_RXCHR_Pos) /**< \brief (UART_RHR) Received Character */
/* -------- UART_THR : (UART Offset: 0x001C) Transmit Holding Register -------- */
#define UART_THR_TXCHR_Pos 0
#define UART_THR_TXCHR_Msk (0xffu << UART_THR_TXCHR_Pos) /**< \brief (UART_THR) Character to be Transmitted */
#define UART_THR_TXCHR(value) ((UART_THR_TXCHR_Msk & ((value) << UART_THR_TXCHR_Pos)))
/* -------- UART_BRGR : (UART Offset: 0x0020) Baud Rate Generator Register -------- */
#define UART_BRGR_CD_Pos 0
#define UART_BRGR_CD_Msk (0xffffu << UART_BRGR_CD_Pos) /**< \brief (UART_BRGR) Clock Divisor */
#define UART_BRGR_CD(value) ((UART_BRGR_CD_Msk & ((value) << UART_BRGR_CD_Pos)))
/* -------- UART_CMPR : (UART Offset: 0x0024) Comparison Register -------- */
#define UART_CMPR_VAL1_Pos 0
#define UART_CMPR_VAL1_Msk (0xffu << UART_CMPR_VAL1_Pos) /**< \brief (UART_CMPR) First Comparison Value for Received Character */
#define UART_CMPR_VAL1(value) ((UART_CMPR_VAL1_Msk & ((value) << UART_CMPR_VAL1_Pos)))
#define UART_CMPR_CMPMODE (0x1u << 12) /**< \brief (UART_CMPR) Comparison Mode */
#define   UART_CMPR_CMPMODE_FLAG_ONLY (0x0u << 12) /**< \brief (UART_CMPR) Any character is received and comparison function drives CMP flag. */
#define   UART_CMPR_CMPMODE_START_CONDITION (0x1u << 12) /**< \brief (UART_CMPR) Comparison condition must be met to start reception. */
#define UART_CMPR_CMPPAR (0x1u << 14) /**< \brief (UART_CMPR) Compare Parity */
#define UART_CMPR_VAL2_Pos 16
#define UART_CMPR_VAL2_Msk (0xffu << UART_CMPR_VAL2_Pos) /**< \brief (UART_CMPR) Second Comparison Value for Received Character */
#define UART_CMPR_VAL2(value) ((UART_CMPR_VAL2_Msk & ((value) << UART_CMPR_VAL2_Pos)))
/* -------- UART_WPMR : (UART Offset: 0x00E4) Write Protection Mode Register -------- */
#define UART_WPMR_WPEN (0x1u << 0) /**< \brief (UART_WPMR) Write Protection Enable */
#define UART_WPMR_WPKEY_Pos 8
#define UART_WPMR_WPKEY_Msk (0xffffffu << UART_WPMR_WPKEY_Pos) /**< \brief (UART_WPMR) Write Protection Key */
#define UART_WPMR_WPKEY(value) ((UART_WPMR_WPKEY_Msk & ((value) << UART_WPMR_WPKEY_Pos)))
#define   UART_WPMR_WPKEY_PASSWD (0x554152u << 8) /**< \brief (UART_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0. */
/* -------- UART_VERSION : (UART Offset: 0x00FC) Version Register -------- */
#define UART_VERSION_VERSION_Pos 0
#define UART_VERSION_VERSION_Msk (0xfffu << UART_VERSION_VERSION_Pos) /**< \brief (UART_VERSION) Hardware Module Version */
#define UART_VERSION_MFN_Pos 16
#define UART_VERSION_MFN_Msk (0x7u << UART_VERSION_MFN_Pos) /**< \brief (UART_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_UART_COMPONENT_ */
