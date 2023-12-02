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

#ifndef _SAME70_USART_COMPONENT_
#define _SAME70_USART_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Universal Synchronous Asynchronous Receiver Transmitter */
/* ============================================================================= */
/** \addtogroup SAME70_USART Universal Synchronous Asynchronous Receiver Transmitter */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Usart hardware registers */
typedef struct {
  __O  uint32_t US_CR;         /**< \brief (Usart Offset: 0x0000) Control Register */
  __IO uint32_t US_MR;         /**< \brief (Usart Offset: 0x0004) Mode Register */
  __O  uint32_t US_IER;        /**< \brief (Usart Offset: 0x0008) Interrupt Enable Register */
  __O  uint32_t US_IDR;        /**< \brief (Usart Offset: 0x000C) Interrupt Disable Register */
  __I  uint32_t US_IMR;        /**< \brief (Usart Offset: 0x0010) Interrupt Mask Register */
  __I  uint32_t US_CSR;        /**< \brief (Usart Offset: 0x0014) Channel Status Register */
  __I  uint32_t US_RHR;        /**< \brief (Usart Offset: 0x0018) Receive Holding Register */
  __O  uint32_t US_THR;        /**< \brief (Usart Offset: 0x001C) Transmit Holding Register */
  __IO uint32_t US_BRGR;       /**< \brief (Usart Offset: 0x0020) Baud Rate Generator Register */
  __IO uint32_t US_RTOR;       /**< \brief (Usart Offset: 0x0024) Receiver Time-out Register */
  __IO uint32_t US_TTGR;       /**< \brief (Usart Offset: 0x0028) Transmitter Timeguard Register */
  __I  uint32_t Reserved1[5];
  __IO uint32_t US_FIDI;       /**< \brief (Usart Offset: 0x0040) FI DI Ratio Register */
  __I  uint32_t US_NER;        /**< \brief (Usart Offset: 0x0044) Number of Errors Register */
  __I  uint32_t Reserved2[1];
  __IO uint32_t US_IF;         /**< \brief (Usart Offset: 0x004C) IrDA Filter Register */
  __IO uint32_t US_MAN;        /**< \brief (Usart Offset: 0x0050) Manchester Configuration Register */
  __IO uint32_t US_LINMR;      /**< \brief (Usart Offset: 0x0054) LIN Mode Register */
  __IO uint32_t US_LINIR;      /**< \brief (Usart Offset: 0x0058) LIN Identifier Register */
  __I  uint32_t US_LINBRR;     /**< \brief (Usart Offset: 0x005C) LIN Baud Rate Register */
  __IO uint32_t US_LONMR;      /**< \brief (Usart Offset: 0x0060) LON Mode Register */
  __IO uint32_t US_LONPR;      /**< \brief (Usart Offset: 0x0064) LON Preamble Register */
  __IO uint32_t US_LONDL;      /**< \brief (Usart Offset: 0x0068) LON Data Length Register */
  __IO uint32_t US_LONL2HDR;   /**< \brief (Usart Offset: 0x006C) LON L2HDR Register */
  __I  uint32_t US_LONBL;      /**< \brief (Usart Offset: 0x0070) LON Backlog Register */
  __IO uint32_t US_LONB1TX;    /**< \brief (Usart Offset: 0x0074) LON Beta1 Tx Register */
  __IO uint32_t US_LONB1RX;    /**< \brief (Usart Offset: 0x0078) LON Beta1 Rx Register */
  __IO uint32_t US_LONPRIO;    /**< \brief (Usart Offset: 0x007C) LON Priority Register */
  __IO uint32_t US_IDTTX;      /**< \brief (Usart Offset: 0x0080) LON IDT Tx Register */
  __IO uint32_t US_IDTRX;      /**< \brief (Usart Offset: 0x0084) LON IDT Rx Register */
  __IO uint32_t US_ICDIFF;     /**< \brief (Usart Offset: 0x0088) IC DIFF Register */
  __I  uint32_t Reserved3[22];
  __IO uint32_t US_WPMR;       /**< \brief (Usart Offset: 0x00E4) Write Protection Mode Register */
  __I  uint32_t US_WPSR;       /**< \brief (Usart Offset: 0x00E8) Write Protection Status Register */
  __I  uint32_t Reserved4[4];
  __I  uint32_t US_VERSION;    /**< \brief (Usart Offset: 0x00FC) Version Register */
} Usart;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- US_CR : (USART Offset: 0x0000) Control Register -------- */
#define US_CR_RSTRX (0x1u << 2) /**< \brief (US_CR) Reset Receiver */
#define US_CR_RSTTX (0x1u << 3) /**< \brief (US_CR) Reset Transmitter */
#define US_CR_RXEN (0x1u << 4) /**< \brief (US_CR) Receiver Enable */
#define US_CR_RXDIS (0x1u << 5) /**< \brief (US_CR) Receiver Disable */
#define US_CR_TXEN (0x1u << 6) /**< \brief (US_CR) Transmitter Enable */
#define US_CR_TXDIS (0x1u << 7) /**< \brief (US_CR) Transmitter Disable */
#define US_CR_RSTSTA (0x1u << 8) /**< \brief (US_CR) Reset Status Bits */
#define US_CR_STTBRK (0x1u << 9) /**< \brief (US_CR) Start Break */
#define US_CR_STPBRK (0x1u << 10) /**< \brief (US_CR) Stop Break */
#define US_CR_STTTO (0x1u << 11) /**< \brief (US_CR) Clear TIMEOUT Flag and Start Time-out After Next Character Received */
#define US_CR_SENDA (0x1u << 12) /**< \brief (US_CR) Send Address */
#define US_CR_RSTIT (0x1u << 13) /**< \brief (US_CR) Reset Iterations */
#define US_CR_RSTNACK (0x1u << 14) /**< \brief (US_CR) Reset Non Acknowledge */
#define US_CR_RETTO (0x1u << 15) /**< \brief (US_CR) Start Time-out Immediately */
#define US_CR_DTREN (0x1u << 16) /**< \brief (US_CR) Data Terminal Ready Enable */
#define US_CR_DTRDIS (0x1u << 17) /**< \brief (US_CR) Data Terminal Ready Disable */
#define US_CR_RTSEN (0x1u << 18) /**< \brief (US_CR) Request to Send Pin Control */
#define US_CR_RTSDIS (0x1u << 19) /**< \brief (US_CR) Request to Send Pin Control */
#define US_CR_LINABT (0x1u << 20) /**< \brief (US_CR) Abort LIN Transmission */
#define US_CR_LINWKUP (0x1u << 21) /**< \brief (US_CR) Send LIN Wakeup Signal */
#define US_CR_FCS (0x1u << 18) /**< \brief (US_CR) Force SPI Chip Select */
#define US_CR_RCS (0x1u << 19) /**< \brief (US_CR) Release SPI Chip Select */
/* -------- US_MR : (USART Offset: 0x0004) Mode Register -------- */
#define US_MR_USART_MODE_Pos 0
#define US_MR_USART_MODE_Msk (0xfu << US_MR_USART_MODE_Pos) /**< \brief (US_MR) USART Mode of Operation */
#define US_MR_USART_MODE(value) ((US_MR_USART_MODE_Msk & ((value) << US_MR_USART_MODE_Pos)))
#define   US_MR_USART_MODE_NORMAL (0x0u << 0) /**< \brief (US_MR) Normal mode */
#define   US_MR_USART_MODE_RS485 (0x1u << 0) /**< \brief (US_MR) RS485 */
#define   US_MR_USART_MODE_HW_HANDSHAKING (0x2u << 0) /**< \brief (US_MR) Hardware Handshaking */
#define   US_MR_USART_MODE_MODEM (0x3u << 0) /**< \brief (US_MR) Modem */
#define   US_MR_USART_MODE_IS07816_T_0 (0x4u << 0) /**< \brief (US_MR) IS07816 Protocol: T = 0 */
#define   US_MR_USART_MODE_IS07816_T_1 (0x6u << 0) /**< \brief (US_MR) IS07816 Protocol: T = 1 */
#define   US_MR_USART_MODE_IRDA (0x8u << 0) /**< \brief (US_MR) IrDA */
#define   US_MR_USART_MODE_LON (0x9u << 0) /**< \brief (US_MR) LON */
#define   US_MR_USART_MODE_LIN_MASTER (0xAu << 0) /**< \brief (US_MR) LIN Master mode */
#define   US_MR_USART_MODE_LIN_SLAVE (0xBu << 0) /**< \brief (US_MR) LIN Slave mode */
#define   US_MR_USART_MODE_SPI_MASTER (0xEu << 0) /**< \brief (US_MR) SPI master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2) */
#define   US_MR_USART_MODE_SPI_SLAVE (0xFu << 0) /**< \brief (US_MR) SPI Slave mode */
#define US_MR_USCLKS_Pos 4
#define US_MR_USCLKS_Msk (0x3u << US_MR_USCLKS_Pos) /**< \brief (US_MR) Clock Selection */
#define US_MR_USCLKS(value) ((US_MR_USCLKS_Msk & ((value) << US_MR_USCLKS_Pos)))
#define   US_MR_USCLKS_MCK (0x0u << 4) /**< \brief (US_MR) Peripheral clock is selected */
#define   US_MR_USCLKS_DIV (0x1u << 4) /**< \brief (US_MR) Peripheral clock divided (DIV=DIV=8) is selected */
#define   US_MR_USCLKS_PCK (0x2u << 4) /**< \brief (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1. */
#define   US_MR_USCLKS_SCK (0x3u << 4) /**< \brief (US_MR) Serial clock (SCK) is selected */
#define US_MR_CHRL_Pos 6
#define US_MR_CHRL_Msk (0x3u << US_MR_CHRL_Pos) /**< \brief (US_MR) Character Length */
#define US_MR_CHRL(value) ((US_MR_CHRL_Msk & ((value) << US_MR_CHRL_Pos)))
#define   US_MR_CHRL_5_BIT (0x0u << 6) /**< \brief (US_MR) Character length is 5 bits */
#define   US_MR_CHRL_6_BIT (0x1u << 6) /**< \brief (US_MR) Character length is 6 bits */
#define   US_MR_CHRL_7_BIT (0x2u << 6) /**< \brief (US_MR) Character length is 7 bits */
#define   US_MR_CHRL_8_BIT (0x3u << 6) /**< \brief (US_MR) Character length is 8 bits */
#define US_MR_SYNC (0x1u << 8) /**< \brief (US_MR) Synchronous Mode Select */
#define US_MR_PAR_Pos 9
#define US_MR_PAR_Msk (0x7u << US_MR_PAR_Pos) /**< \brief (US_MR) Parity Type */
#define US_MR_PAR(value) ((US_MR_PAR_Msk & ((value) << US_MR_PAR_Pos)))
#define   US_MR_PAR_EVEN (0x0u << 9) /**< \brief (US_MR) Even parity */
#define   US_MR_PAR_ODD (0x1u << 9) /**< \brief (US_MR) Odd parity */
#define   US_MR_PAR_SPACE (0x2u << 9) /**< \brief (US_MR) Parity forced to 0 (Space) */
#define   US_MR_PAR_MARK (0x3u << 9) /**< \brief (US_MR) Parity forced to 1 (Mark) */
#define   US_MR_PAR_NO (0x4u << 9) /**< \brief (US_MR) No parity */
#define   US_MR_PAR_MULTIDROP (0x6u << 9) /**< \brief (US_MR) Multidrop mode */
#define US_MR_NBSTOP_Pos 12
#define US_MR_NBSTOP_Msk (0x3u << US_MR_NBSTOP_Pos) /**< \brief (US_MR) Number of Stop Bits */
#define US_MR_NBSTOP(value) ((US_MR_NBSTOP_Msk & ((value) << US_MR_NBSTOP_Pos)))
#define   US_MR_NBSTOP_1_BIT (0x0u << 12) /**< \brief (US_MR) 1 stop bit */
#define   US_MR_NBSTOP_1_5_BIT (0x1u << 12) /**< \brief (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1) */
#define   US_MR_NBSTOP_2_BIT (0x2u << 12) /**< \brief (US_MR) 2 stop bits */
#define US_MR_CHMODE_Pos 14
#define US_MR_CHMODE_Msk (0x3u << US_MR_CHMODE_Pos) /**< \brief (US_MR) Channel Mode */
#define US_MR_CHMODE(value) ((US_MR_CHMODE_Msk & ((value) << US_MR_CHMODE_Pos)))
#define   US_MR_CHMODE_NORMAL (0x0u << 14) /**< \brief (US_MR) Normal mode */
#define   US_MR_CHMODE_AUTOMATIC (0x1u << 14) /**< \brief (US_MR) Automatic Echo. Receiver input is connected to the TXD pin. */
#define   US_MR_CHMODE_LOCAL_LOOPBACK (0x2u << 14) /**< \brief (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input. */
#define   US_MR_CHMODE_REMOTE_LOOPBACK (0x3u << 14) /**< \brief (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin. */
#define US_MR_MSBF (0x1u << 16) /**< \brief (US_MR) Bit Order */
#define US_MR_MODE9 (0x1u << 17) /**< \brief (US_MR) 9-bit Character Length */
#define US_MR_CLKO (0x1u << 18) /**< \brief (US_MR) Clock Output Select */
#define US_MR_OVER (0x1u << 19) /**< \brief (US_MR) Oversampling Mode */
#define US_MR_INACK (0x1u << 20) /**< \brief (US_MR) Inhibit Non Acknowledge */
#define US_MR_DSNACK (0x1u << 21) /**< \brief (US_MR) Disable Successive NACK */
#define US_MR_VAR_SYNC (0x1u << 22) /**< \brief (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter */
#define US_MR_INVDATA (0x1u << 23) /**< \brief (US_MR) Inverted Data */
#define US_MR_MAX_ITERATION_Pos 24
#define US_MR_MAX_ITERATION_Msk (0x7u << US_MR_MAX_ITERATION_Pos) /**< \brief (US_MR) Maximum Number of Automatic Iteration */
#define US_MR_MAX_ITERATION(value) ((US_MR_MAX_ITERATION_Msk & ((value) << US_MR_MAX_ITERATION_Pos)))
#define US_MR_FILTER (0x1u << 28) /**< \brief (US_MR) Receive Line Filter */
#define US_MR_MAN (0x1u << 29) /**< \brief (US_MR) Manchester Encoder/Decoder Enable */
#define US_MR_MODSYNC (0x1u << 30) /**< \brief (US_MR) Manchester Synchronization Mode */
#define US_MR_ONEBIT (0x1u << 31) /**< \brief (US_MR) Start Frame Delimiter Selector */
#define US_MR_CPHA (0x1u << 8) /**< \brief (US_MR) SPI Clock Phase */
#define US_MR_CPOL (0x1u << 16) /**< \brief (US_MR) SPI Clock Polarity */
#define US_MR_WRDBT (0x1u << 20) /**< \brief (US_MR) Wait Read Data Before Transfer */
/* -------- US_IER : (USART Offset: 0x0008) Interrupt Enable Register -------- */
#define US_IER_RXRDY (0x1u << 0) /**< \brief (US_IER) RXRDY Interrupt Enable */
#define US_IER_TXRDY (0x1u << 1) /**< \brief (US_IER) TXRDY Interrupt Enable */
#define US_IER_RXBRK (0x1u << 2) /**< \brief (US_IER) Receiver Break Interrupt Enable */
#define US_IER_OVRE (0x1u << 5) /**< \brief (US_IER) Overrun Error Interrupt Enable */
#define US_IER_FRAME (0x1u << 6) /**< \brief (US_IER) Framing Error Interrupt Enable */
#define US_IER_PARE (0x1u << 7) /**< \brief (US_IER) Parity Error Interrupt Enable */
#define US_IER_TIMEOUT (0x1u << 8) /**< \brief (US_IER) Time-out Interrupt Enable */
#define US_IER_TXEMPTY (0x1u << 9) /**< \brief (US_IER) TXEMPTY Interrupt Enable */
#define US_IER_ITER (0x1u << 10) /**< \brief (US_IER) Max number of Repetitions Reached Interrupt Enable */
#define US_IER_NACK (0x1u << 13) /**< \brief (US_IER) Non Acknowledge Interrupt Enable */
#define US_IER_RIIC (0x1u << 16) /**< \brief (US_IER) Ring Indicator Input Change Enable */
#define US_IER_DSRIC (0x1u << 17) /**< \brief (US_IER) Data Set Ready Input Change Enable */
#define US_IER_DCDIC (0x1u << 18) /**< \brief (US_IER) Data Carrier Detect Input Change Interrupt Enable */
#define US_IER_CTSIC (0x1u << 19) /**< \brief (US_IER) Clear to Send Input Change Interrupt Enable */
#define US_IER_MANE (0x1u << 24) /**< \brief (US_IER) Manchester Error Interrupt Enable */
#define US_IER_UNRE (0x1u << 10) /**< \brief (US_IER) SPI Underrun Error Interrupt Enable */
#define US_IER_NSSE (0x1u << 19) /**< \brief (US_IER) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Interrupt Enable */
#define US_IER_LINBK (0x1u << 13) /**< \brief (US_IER) LIN Break Sent or LIN Break Received Interrupt Enable */
#define US_IER_LINID (0x1u << 14) /**< \brief (US_IER) LIN Identifier Sent or LIN Identifier Received Interrupt Enable */
#define US_IER_LINTC (0x1u << 15) /**< \brief (US_IER) LIN Transfer Completed Interrupt Enable */
#define US_IER_LINBE (0x1u << 25) /**< \brief (US_IER) LIN Bus Error Interrupt Enable */
#define US_IER_LINISFE (0x1u << 26) /**< \brief (US_IER) LIN Inconsistent Synch Field Error Interrupt Enable */
#define US_IER_LINIPE (0x1u << 27) /**< \brief (US_IER) LIN Identifier Parity Interrupt Enable */
#define US_IER_LINCE (0x1u << 28) /**< \brief (US_IER) LIN Checksum Error Interrupt Enable */
#define US_IER_LINSNRE (0x1u << 29) /**< \brief (US_IER) LIN Slave Not Responding Error Interrupt Enable */
#define US_IER_LINSTE (0x1u << 30) /**< \brief (US_IER) LIN Synch Tolerance Error Interrupt Enable */
#define US_IER_LINHTE (0x1u << 31) /**< \brief (US_IER) LIN Header Timeout Error Interrupt Enable */
#define US_IER_LSFE (0x1u << 6) /**< \brief (US_IER) LON Short Frame Error Interrupt Enable */
#define US_IER_LCRCE (0x1u << 7) /**< \brief (US_IER) LON CRC Error Interrupt Enable */
#define US_IER_LTXD (0x1u << 24) /**< \brief (US_IER) LON Transmission Done Interrupt Enable */
#define US_IER_LCOL (0x1u << 25) /**< \brief (US_IER) LON Collision Interrupt Enable */
#define US_IER_LFET (0x1u << 26) /**< \brief (US_IER) LON Frame Early Termination Interrupt Enable */
#define US_IER_LRXD (0x1u << 27) /**< \brief (US_IER) LON Reception Done Interrupt Enable */
#define US_IER_LBLOVFE (0x1u << 28) /**< \brief (US_IER) LON Backlog Overflow Error Interrupt Enable */
/* -------- US_IDR : (USART Offset: 0x000C) Interrupt Disable Register -------- */
#define US_IDR_RXRDY (0x1u << 0) /**< \brief (US_IDR) RXRDY Interrupt Disable */
#define US_IDR_TXRDY (0x1u << 1) /**< \brief (US_IDR) TXRDY Interrupt Disable */
#define US_IDR_RXBRK (0x1u << 2) /**< \brief (US_IDR) Receiver Break Interrupt Disable */
#define US_IDR_OVRE (0x1u << 5) /**< \brief (US_IDR) Overrun Error Interrupt Enable */
#define US_IDR_FRAME (0x1u << 6) /**< \brief (US_IDR) Framing Error Interrupt Disable */
#define US_IDR_PARE (0x1u << 7) /**< \brief (US_IDR) Parity Error Interrupt Disable */
#define US_IDR_TIMEOUT (0x1u << 8) /**< \brief (US_IDR) Time-out Interrupt Disable */
#define US_IDR_TXEMPTY (0x1u << 9) /**< \brief (US_IDR) TXEMPTY Interrupt Disable */
#define US_IDR_ITER (0x1u << 10) /**< \brief (US_IDR) Max Number of Repetitions Reached Interrupt Disable */
#define US_IDR_NACK (0x1u << 13) /**< \brief (US_IDR) Non Acknowledge Interrupt Disable */
#define US_IDR_RIIC (0x1u << 16) /**< \brief (US_IDR) Ring Indicator Input Change Disable */
#define US_IDR_DSRIC (0x1u << 17) /**< \brief (US_IDR) Data Set Ready Input Change Disable */
#define US_IDR_DCDIC (0x1u << 18) /**< \brief (US_IDR) Data Carrier Detect Input Change Interrupt Disable */
#define US_IDR_CTSIC (0x1u << 19) /**< \brief (US_IDR) Clear to Send Input Change Interrupt Disable */
#define US_IDR_MANE (0x1u << 24) /**< \brief (US_IDR) Manchester Error Interrupt Disable */
#define US_IDR_UNRE (0x1u << 10) /**< \brief (US_IDR) SPI Underrun Error Interrupt Disable */
#define US_IDR_NSSE (0x1u << 19) /**< \brief (US_IDR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Interrupt Disable */
#define US_IDR_LINBK (0x1u << 13) /**< \brief (US_IDR) LIN Break Sent or LIN Break Received Interrupt Disable */
#define US_IDR_LINID (0x1u << 14) /**< \brief (US_IDR) LIN Identifier Sent or LIN Identifier Received Interrupt Disable */
#define US_IDR_LINTC (0x1u << 15) /**< \brief (US_IDR) LIN Transfer Completed Interrupt Disable */
#define US_IDR_LINBE (0x1u << 25) /**< \brief (US_IDR) LIN Bus Error Interrupt Disable */
#define US_IDR_LINISFE (0x1u << 26) /**< \brief (US_IDR) LIN Inconsistent Synch Field Error Interrupt Disable */
#define US_IDR_LINIPE (0x1u << 27) /**< \brief (US_IDR) LIN Identifier Parity Interrupt Disable */
#define US_IDR_LINCE (0x1u << 28) /**< \brief (US_IDR) LIN Checksum Error Interrupt Disable */
#define US_IDR_LINSNRE (0x1u << 29) /**< \brief (US_IDR) LIN Slave Not Responding Error Interrupt Disable */
#define US_IDR_LINSTE (0x1u << 30) /**< \brief (US_IDR) LIN Synch Tolerance Error Interrupt Disable */
#define US_IDR_LINHTE (0x1u << 31) /**< \brief (US_IDR) LIN Header Timeout Error Interrupt Disable */
#define US_IDR_LSFE (0x1u << 6) /**< \brief (US_IDR) LON Short Frame Error Interrupt Disable */
#define US_IDR_LCRCE (0x1u << 7) /**< \brief (US_IDR) LON CRC Error Interrupt Disable */
#define US_IDR_LTXD (0x1u << 24) /**< \brief (US_IDR) LON Transmission Done Interrupt Disable */
#define US_IDR_LCOL (0x1u << 25) /**< \brief (US_IDR) LON Collision Interrupt Disable */
#define US_IDR_LFET (0x1u << 26) /**< \brief (US_IDR) LON Frame Early Termination Interrupt Disable */
#define US_IDR_LRXD (0x1u << 27) /**< \brief (US_IDR) LON Reception Done Interrupt Disable */
#define US_IDR_LBLOVFE (0x1u << 28) /**< \brief (US_IDR) LON Backlog Overflow Error Interrupt Disable */
/* -------- US_IMR : (USART Offset: 0x0010) Interrupt Mask Register -------- */
#define US_IMR_RXRDY (0x1u << 0) /**< \brief (US_IMR) RXRDY Interrupt Mask */
#define US_IMR_TXRDY (0x1u << 1) /**< \brief (US_IMR) TXRDY Interrupt Mask */
#define US_IMR_RXBRK (0x1u << 2) /**< \brief (US_IMR) Receiver Break Interrupt Mask */
#define US_IMR_OVRE (0x1u << 5) /**< \brief (US_IMR) Overrun Error Interrupt Mask */
#define US_IMR_FRAME (0x1u << 6) /**< \brief (US_IMR) Framing Error Interrupt Mask */
#define US_IMR_PARE (0x1u << 7) /**< \brief (US_IMR) Parity Error Interrupt Mask */
#define US_IMR_TIMEOUT (0x1u << 8) /**< \brief (US_IMR) Time-out Interrupt Mask */
#define US_IMR_TXEMPTY (0x1u << 9) /**< \brief (US_IMR) TXEMPTY Interrupt Mask */
#define US_IMR_ITER (0x1u << 10) /**< \brief (US_IMR) Max Number of Repetitions Reached Interrupt Mask */
#define US_IMR_NACK (0x1u << 13) /**< \brief (US_IMR) Non Acknowledge Interrupt Mask */
#define US_IMR_RIIC (0x1u << 16) /**< \brief (US_IMR) Ring Indicator Input Change Mask */
#define US_IMR_DSRIC (0x1u << 17) /**< \brief (US_IMR) Data Set Ready Input Change Mask */
#define US_IMR_DCDIC (0x1u << 18) /**< \brief (US_IMR) Data Carrier Detect Input Change Interrupt Mask */
#define US_IMR_CTSIC (0x1u << 19) /**< \brief (US_IMR) Clear to Send Input Change Interrupt Mask */
#define US_IMR_MANE (0x1u << 24) /**< \brief (US_IMR) Manchester Error Interrupt Mask */
#define US_IMR_UNRE (0x1u << 10) /**< \brief (US_IMR) SPI Underrun Error Interrupt Mask */
#define US_IMR_NSSE (0x1u << 19) /**< \brief (US_IMR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Interrupt Mask */
#define US_IMR_LINBK (0x1u << 13) /**< \brief (US_IMR) LIN Break Sent or LIN Break Received Interrupt Mask */
#define US_IMR_LINID (0x1u << 14) /**< \brief (US_IMR) LIN Identifier Sent or LIN Identifier Received Interrupt Mask */
#define US_IMR_LINTC (0x1u << 15) /**< \brief (US_IMR) LIN Transfer Completed Interrupt Mask */
#define US_IMR_LINBE (0x1u << 25) /**< \brief (US_IMR) LIN Bus Error Interrupt Mask */
#define US_IMR_LINISFE (0x1u << 26) /**< \brief (US_IMR) LIN Inconsistent Synch Field Error Interrupt Mask */
#define US_IMR_LINIPE (0x1u << 27) /**< \brief (US_IMR) LIN Identifier Parity Interrupt Mask */
#define US_IMR_LINCE (0x1u << 28) /**< \brief (US_IMR) LIN Checksum Error Interrupt Mask */
#define US_IMR_LINSNRE (0x1u << 29) /**< \brief (US_IMR) LIN Slave Not Responding Error Interrupt Mask */
#define US_IMR_LINSTE (0x1u << 30) /**< \brief (US_IMR) LIN Synch Tolerance Error Interrupt Mask */
#define US_IMR_LINHTE (0x1u << 31) /**< \brief (US_IMR) LIN Header Timeout Error Interrupt Mask */
#define US_IMR_LSFE (0x1u << 6) /**< \brief (US_IMR) LON Short Frame Error Interrupt Mask */
#define US_IMR_LCRCE (0x1u << 7) /**< \brief (US_IMR) LON CRC Error Interrupt Mask */
#define US_IMR_LTXD (0x1u << 24) /**< \brief (US_IMR) LON Transmission Done Interrupt Mask */
#define US_IMR_LCOL (0x1u << 25) /**< \brief (US_IMR) LON Collision Interrupt Mask */
#define US_IMR_LFET (0x1u << 26) /**< \brief (US_IMR) LON Frame Early Termination Interrupt Mask */
#define US_IMR_LRXD (0x1u << 27) /**< \brief (US_IMR) LON Reception Done Interrupt Mask */
#define US_IMR_LBLOVFE (0x1u << 28) /**< \brief (US_IMR) LON Backlog Overflow Error Interrupt Mask */
/* -------- US_CSR : (USART Offset: 0x0014) Channel Status Register -------- */
#define US_CSR_RXRDY (0x1u << 0) /**< \brief (US_CSR) Receiver Ready (cleared by reading US_RHR) */
#define US_CSR_TXRDY (0x1u << 1) /**< \brief (US_CSR) Transmitter Ready (cleared by writing US_THR) */
#define US_CSR_RXBRK (0x1u << 2) /**< \brief (US_CSR) Break Received/End of Break (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_OVRE (0x1u << 5) /**< \brief (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_FRAME (0x1u << 6) /**< \brief (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_PARE (0x1u << 7) /**< \brief (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_TIMEOUT (0x1u << 8) /**< \brief (US_CSR) Receiver Time-out (cleared by writing a one to bit US_CR.STTTO) */
#define US_CSR_TXEMPTY (0x1u << 9) /**< \brief (US_CSR) Transmitter Empty (cleared by writing US_THR) */
#define US_CSR_ITER (0x1u << 10) /**< \brief (US_CSR) Max Number of Repetitions Reached (cleared by writing a one to bit US_CR.RSTIT) */
#define US_CSR_NACK (0x1u << 13) /**< \brief (US_CSR) Non Acknowledge Interrupt (cleared by writing a one to bit US_CR.RSTNACK) */
#define US_CSR_RIIC (0x1u << 16) /**< \brief (US_CSR) Ring Indicator Input Change Flag (cleared on read) */
#define US_CSR_DSRIC (0x1u << 17) /**< \brief (US_CSR) Data Set Ready Input Change Flag (cleared on read) */
#define US_CSR_DCDIC (0x1u << 18) /**< \brief (US_CSR) Data Carrier Detect Input Change Flag (cleared on read) */
#define US_CSR_CTSIC (0x1u << 19) /**< \brief (US_CSR) Clear to Send Input Change Flag (cleared on read) */
#define US_CSR_RI (0x1u << 20) /**< \brief (US_CSR) Image of RI Input */
#define US_CSR_DSR (0x1u << 21) /**< \brief (US_CSR) Image of DSR Input */
#define US_CSR_DCD (0x1u << 22) /**< \brief (US_CSR) Image of DCD Input */
#define US_CSR_CTS (0x1u << 23) /**< \brief (US_CSR) Image of CTS Input */
#define US_CSR_MANERR (0x1u << 24) /**< \brief (US_CSR) Manchester Error (cleared by writing a one to the bit US_CR.RSTSTA) */
#define US_CSR_UNRE (0x1u << 10) /**< \brief (US_CSR) Underrun Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_NSSE (0x1u << 19) /**< \brief (US_CSR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event (cleared on read) */
#define US_CSR_NSS (0x1u << 23) /**< \brief (US_CSR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event (cleared on read) */
#define US_CSR_LINBK (0x1u << 13) /**< \brief (US_CSR) LIN Break Sent or LIN Break Received (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINID (0x1u << 14) /**< \brief (US_CSR) LIN Identifier Sent or LIN Identifier Received (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINTC (0x1u << 15) /**< \brief (US_CSR) LIN Transfer Completed (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINBLS (0x1u << 23) /**< \brief (US_CSR) LIN Bus Line Status */
#define US_CSR_LINBE (0x1u << 25) /**< \brief (US_CSR) LIN Bit Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINISFE (0x1u << 26) /**< \brief (US_CSR) LIN Inconsistent Synch Field Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINIPE (0x1u << 27) /**< \brief (US_CSR) LIN Identifier Parity Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINCE (0x1u << 28) /**< \brief (US_CSR) LIN Checksum Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINSNRE (0x1u << 29) /**< \brief (US_CSR) LIN Slave Not Responding Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINSTE (0x1u << 30) /**< \brief (US_CSR) LIN Synch Tolerance Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LINHTE (0x1u << 31) /**< \brief (US_CSR) LIN Header Timeout Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LSFE (0x1u << 6) /**< \brief (US_CSR) LON Short Frame Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LCRCE (0x1u << 7) /**< \brief (US_CSR) LON CRC Error (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LTXD (0x1u << 24) /**< \brief (US_CSR) LON Transmission End Flag (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LCOL (0x1u << 25) /**< \brief (US_CSR) LON Collision Detected Flag (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LFET (0x1u << 26) /**< \brief (US_CSR) LON Frame Early Termination (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LRXD (0x1u << 27) /**< \brief (US_CSR) LON Reception End Flag (cleared by writing a one to bit US_CR.RSTSTA) */
#define US_CSR_LBLOVFE (0x1u << 28) /**< \brief (US_CSR) LON Backlog Overflow Error (cleared by writing a one to bit US_CR.RSTSTA) */
/* -------- US_RHR : (USART Offset: 0x0018) Receive Holding Register -------- */
#define US_RHR_RXCHR_Pos 0
#define US_RHR_RXCHR_Msk (0x1ffu << US_RHR_RXCHR_Pos) /**< \brief (US_RHR) Received Character */
#define US_RHR_RXSYNH (0x1u << 15) /**< \brief (US_RHR) Received Sync */
/* -------- US_THR : (USART Offset: 0x001C) Transmit Holding Register -------- */
#define US_THR_TXCHR_Pos 0
#define US_THR_TXCHR_Msk (0x1ffu << US_THR_TXCHR_Pos) /**< \brief (US_THR) Character to be Transmitted */
#define US_THR_TXCHR(value) ((US_THR_TXCHR_Msk & ((value) << US_THR_TXCHR_Pos)))
#define US_THR_TXSYNH (0x1u << 15) /**< \brief (US_THR) Sync Field to be Transmitted */
/* -------- US_BRGR : (USART Offset: 0x0020) Baud Rate Generator Register -------- */
#define US_BRGR_CD_Pos 0
#define US_BRGR_CD_Msk (0xffffu << US_BRGR_CD_Pos) /**< \brief (US_BRGR) Clock Divider */
#define US_BRGR_CD(value) ((US_BRGR_CD_Msk & ((value) << US_BRGR_CD_Pos)))
#define US_BRGR_FP_Pos 16
#define US_BRGR_FP_Msk (0x7u << US_BRGR_FP_Pos) /**< \brief (US_BRGR) Fractional Part */
#define US_BRGR_FP(value) ((US_BRGR_FP_Msk & ((value) << US_BRGR_FP_Pos)))
/* -------- US_RTOR : (USART Offset: 0x0024) Receiver Time-out Register -------- */
#define US_RTOR_TO_Pos 0
#define US_RTOR_TO_Msk (0x1ffffu << US_RTOR_TO_Pos) /**< \brief (US_RTOR) Time-out Value */
#define US_RTOR_TO(value) ((US_RTOR_TO_Msk & ((value) << US_RTOR_TO_Pos)))
/* -------- US_TTGR : (USART Offset: 0x0028) Transmitter Timeguard Register -------- */
#define US_TTGR_TG_Pos 0
#define US_TTGR_TG_Msk (0xffu << US_TTGR_TG_Pos) /**< \brief (US_TTGR) Timeguard Value */
#define US_TTGR_TG(value) ((US_TTGR_TG_Msk & ((value) << US_TTGR_TG_Pos)))
#define US_TTGR_PCYCLE_Pos 0
#define US_TTGR_PCYCLE_Msk (0xffffffu << US_TTGR_PCYCLE_Pos) /**< \brief (US_TTGR) LON PCYCLE Length */
#define US_TTGR_PCYCLE(value) ((US_TTGR_PCYCLE_Msk & ((value) << US_TTGR_PCYCLE_Pos)))
/* -------- US_FIDI : (USART Offset: 0x0040) FI DI Ratio Register -------- */
#define US_FIDI_FI_DI_RATIO_Pos 0
#define US_FIDI_FI_DI_RATIO_Msk (0xffffu << US_FIDI_FI_DI_RATIO_Pos) /**< \brief (US_FIDI) FI Over DI Ratio Value */
#define US_FIDI_FI_DI_RATIO(value) ((US_FIDI_FI_DI_RATIO_Msk & ((value) << US_FIDI_FI_DI_RATIO_Pos)))
#define US_FIDI_BETA2_Pos 0
#define US_FIDI_BETA2_Msk (0xffffffu << US_FIDI_BETA2_Pos) /**< \brief (US_FIDI) LON BETA2 Length */
#define US_FIDI_BETA2(value) ((US_FIDI_BETA2_Msk & ((value) << US_FIDI_BETA2_Pos)))
/* -------- US_NER : (USART Offset: 0x0044) Number of Errors Register -------- */
#define US_NER_NB_ERRORS_Pos 0
#define US_NER_NB_ERRORS_Msk (0xffu << US_NER_NB_ERRORS_Pos) /**< \brief (US_NER) Number of Errors */
/* -------- US_IF : (USART Offset: 0x004C) IrDA Filter Register -------- */
#define US_IF_IRDA_FILTER_Pos 0
#define US_IF_IRDA_FILTER_Msk (0xffu << US_IF_IRDA_FILTER_Pos) /**< \brief (US_IF) IrDA Filter */
#define US_IF_IRDA_FILTER(value) ((US_IF_IRDA_FILTER_Msk & ((value) << US_IF_IRDA_FILTER_Pos)))
/* -------- US_MAN : (USART Offset: 0x0050) Manchester Configuration Register -------- */
#define US_MAN_TX_PL_Pos 0
#define US_MAN_TX_PL_Msk (0xfu << US_MAN_TX_PL_Pos) /**< \brief (US_MAN) Transmitter Preamble Length */
#define US_MAN_TX_PL(value) ((US_MAN_TX_PL_Msk & ((value) << US_MAN_TX_PL_Pos)))
#define US_MAN_TX_PP_Pos 8
#define US_MAN_TX_PP_Msk (0x3u << US_MAN_TX_PP_Pos) /**< \brief (US_MAN) Transmitter Preamble Pattern */
#define US_MAN_TX_PP(value) ((US_MAN_TX_PP_Msk & ((value) << US_MAN_TX_PP_Pos)))
#define   US_MAN_TX_PP_ALL_ONE (0x0u << 8) /**< \brief (US_MAN) The preamble is composed of '1's */
#define   US_MAN_TX_PP_ALL_ZERO (0x1u << 8) /**< \brief (US_MAN) The preamble is composed of '0's */
#define   US_MAN_TX_PP_ZERO_ONE (0x2u << 8) /**< \brief (US_MAN) The preamble is composed of '01's */
#define   US_MAN_TX_PP_ONE_ZERO (0x3u << 8) /**< \brief (US_MAN) The preamble is composed of '10's */
#define US_MAN_TX_MPOL (0x1u << 12) /**< \brief (US_MAN) Transmitter Manchester Polarity */
#define US_MAN_RX_PL_Pos 16
#define US_MAN_RX_PL_Msk (0xfu << US_MAN_RX_PL_Pos) /**< \brief (US_MAN) Receiver Preamble Length */
#define US_MAN_RX_PL(value) ((US_MAN_RX_PL_Msk & ((value) << US_MAN_RX_PL_Pos)))
#define US_MAN_RX_PP_Pos 24
#define US_MAN_RX_PP_Msk (0x3u << US_MAN_RX_PP_Pos) /**< \brief (US_MAN) Receiver Preamble Pattern detected */
#define US_MAN_RX_PP(value) ((US_MAN_RX_PP_Msk & ((value) << US_MAN_RX_PP_Pos)))
#define   US_MAN_RX_PP_ALL_ONE (0x0u << 24) /**< \brief (US_MAN) The preamble is composed of '1's */
#define   US_MAN_RX_PP_ALL_ZERO (0x1u << 24) /**< \brief (US_MAN) The preamble is composed of '0's */
#define   US_MAN_RX_PP_ZERO_ONE (0x2u << 24) /**< \brief (US_MAN) The preamble is composed of '01's */
#define   US_MAN_RX_PP_ONE_ZERO (0x3u << 24) /**< \brief (US_MAN) The preamble is composed of '10's */
#define US_MAN_RX_MPOL (0x1u << 28) /**< \brief (US_MAN) Receiver Manchester Polarity */
#define US_MAN_ONE (0x1u << 29) /**< \brief (US_MAN) Must Be Set to 1 */
#define US_MAN_DRIFT (0x1u << 30) /**< \brief (US_MAN) Drift Compensation */
#define US_MAN_RXIDLEV (0x1u << 31) /**< \brief (US_MAN)  */
/* -------- US_LINMR : (USART Offset: 0x0054) LIN Mode Register -------- */
#define US_LINMR_NACT_Pos 0
#define US_LINMR_NACT_Msk (0x3u << US_LINMR_NACT_Pos) /**< \brief (US_LINMR) LIN Node Action */
#define US_LINMR_NACT(value) ((US_LINMR_NACT_Msk & ((value) << US_LINMR_NACT_Pos)))
#define   US_LINMR_NACT_PUBLISH (0x0u << 0) /**< \brief (US_LINMR) The USART transmits the response. */
#define   US_LINMR_NACT_SUBSCRIBE (0x1u << 0) /**< \brief (US_LINMR) The USART receives the response. */
#define   US_LINMR_NACT_IGNORE (0x2u << 0) /**< \brief (US_LINMR) The USART does not transmit and does not receive the response. */
#define US_LINMR_PARDIS (0x1u << 2) /**< \brief (US_LINMR) Parity Disable */
#define US_LINMR_CHKDIS (0x1u << 3) /**< \brief (US_LINMR) Checksum Disable */
#define US_LINMR_CHKTYP (0x1u << 4) /**< \brief (US_LINMR) Checksum Type */
#define US_LINMR_DLM (0x1u << 5) /**< \brief (US_LINMR) Data Length Mode */
#define US_LINMR_FSDIS (0x1u << 6) /**< \brief (US_LINMR) Frame Slot Mode Disable */
#define US_LINMR_WKUPTYP (0x1u << 7) /**< \brief (US_LINMR) Wakeup Signal Type */
#define US_LINMR_DLC_Pos 8
#define US_LINMR_DLC_Msk (0xffu << US_LINMR_DLC_Pos) /**< \brief (US_LINMR) Data Length Control */
#define US_LINMR_DLC(value) ((US_LINMR_DLC_Msk & ((value) << US_LINMR_DLC_Pos)))
#define US_LINMR_PDCM (0x1u << 16) /**< \brief (US_LINMR) DMAC Mode */
#define US_LINMR_SYNCDIS (0x1u << 17) /**< \brief (US_LINMR) Synchronization Disable */
/* -------- US_LINIR : (USART Offset: 0x0058) LIN Identifier Register -------- */
#define US_LINIR_IDCHR_Pos 0
#define US_LINIR_IDCHR_Msk (0xffu << US_LINIR_IDCHR_Pos) /**< \brief (US_LINIR) Identifier Character */
#define US_LINIR_IDCHR(value) ((US_LINIR_IDCHR_Msk & ((value) << US_LINIR_IDCHR_Pos)))
/* -------- US_LINBRR : (USART Offset: 0x005C) LIN Baud Rate Register -------- */
#define US_LINBRR_LINCD_Pos 0
#define US_LINBRR_LINCD_Msk (0xffffu << US_LINBRR_LINCD_Pos) /**< \brief (US_LINBRR) Clock Divider after Synchronization */
#define US_LINBRR_LINFP_Pos 16
#define US_LINBRR_LINFP_Msk (0x7u << US_LINBRR_LINFP_Pos) /**< \brief (US_LINBRR) Fractional Part after Synchronization */
/* -------- US_LONMR : (USART Offset: 0x0060) LON Mode Register -------- */
#define US_LONMR_COMMT (0x1u << 0) /**< \brief (US_LONMR) LON comm_type Parameter Value */
#define US_LONMR_COLDET (0x1u << 1) /**< \brief (US_LONMR) LON Collision Detection Feature */
#define US_LONMR_TCOL (0x1u << 2) /**< \brief (US_LONMR) Terminate Frame upon Collision Notification */
#define US_LONMR_CDTAIL (0x1u << 3) /**< \brief (US_LONMR) LON Collision Detection on Frame Tail */
#define US_LONMR_DMAM (0x1u << 4) /**< \brief (US_LONMR) LON DMA Mode */
#define US_LONMR_LCDS (0x1u << 5) /**< \brief (US_LONMR) LON Collision Detection Source */
#define US_LONMR_EOFS_Pos 16
#define US_LONMR_EOFS_Msk (0xffu << US_LONMR_EOFS_Pos) /**< \brief (US_LONMR) End of Frame Condition Size */
#define US_LONMR_EOFS(value) ((US_LONMR_EOFS_Msk & ((value) << US_LONMR_EOFS_Pos)))
/* -------- US_LONPR : (USART Offset: 0x0064) LON Preamble Register -------- */
#define US_LONPR_LONPL_Pos 0
#define US_LONPR_LONPL_Msk (0x3fffu << US_LONPR_LONPL_Pos) /**< \brief (US_LONPR) LON Preamble Length */
#define US_LONPR_LONPL(value) ((US_LONPR_LONPL_Msk & ((value) << US_LONPR_LONPL_Pos)))
/* -------- US_LONDL : (USART Offset: 0x0068) LON Data Length Register -------- */
#define US_LONDL_LONDL_Pos 0
#define US_LONDL_LONDL_Msk (0xffu << US_LONDL_LONDL_Pos) /**< \brief (US_LONDL) LON Data Length */
#define US_LONDL_LONDL(value) ((US_LONDL_LONDL_Msk & ((value) << US_LONDL_LONDL_Pos)))
/* -------- US_LONL2HDR : (USART Offset: 0x006C) LON L2HDR Register -------- */
#define US_LONL2HDR_BLI_Pos 0
#define US_LONL2HDR_BLI_Msk (0x3fu << US_LONL2HDR_BLI_Pos) /**< \brief (US_LONL2HDR) LON Backlog Increment */
#define US_LONL2HDR_BLI(value) ((US_LONL2HDR_BLI_Msk & ((value) << US_LONL2HDR_BLI_Pos)))
#define US_LONL2HDR_ALTP (0x1u << 6) /**< \brief (US_LONL2HDR) LON Alternate Path Bit */
#define US_LONL2HDR_PB (0x1u << 7) /**< \brief (US_LONL2HDR) LON Priority Bit */
/* -------- US_LONBL : (USART Offset: 0x0070) LON Backlog Register -------- */
#define US_LONBL_LONBL_Pos 0
#define US_LONBL_LONBL_Msk (0x3fu << US_LONBL_LONBL_Pos) /**< \brief (US_LONBL) LON Node Backlog Value */
/* -------- US_LONB1TX : (USART Offset: 0x0074) LON Beta1 Tx Register -------- */
#define US_LONB1TX_BETA1TX_Pos 0
#define US_LONB1TX_BETA1TX_Msk (0xffffffu << US_LONB1TX_BETA1TX_Pos) /**< \brief (US_LONB1TX) LON Beta1 Length after Transmission */
#define US_LONB1TX_BETA1TX(value) ((US_LONB1TX_BETA1TX_Msk & ((value) << US_LONB1TX_BETA1TX_Pos)))
/* -------- US_LONB1RX : (USART Offset: 0x0078) LON Beta1 Rx Register -------- */
#define US_LONB1RX_BETA1RX_Pos 0
#define US_LONB1RX_BETA1RX_Msk (0xffffffu << US_LONB1RX_BETA1RX_Pos) /**< \brief (US_LONB1RX) LON Beta1 Length after Reception */
#define US_LONB1RX_BETA1RX(value) ((US_LONB1RX_BETA1RX_Msk & ((value) << US_LONB1RX_BETA1RX_Pos)))
/* -------- US_LONPRIO : (USART Offset: 0x007C) LON Priority Register -------- */
#define US_LONPRIO_PSNB_Pos 0
#define US_LONPRIO_PSNB_Msk (0x7fu << US_LONPRIO_PSNB_Pos) /**< \brief (US_LONPRIO) LON Priority Slot Number */
#define US_LONPRIO_PSNB(value) ((US_LONPRIO_PSNB_Msk & ((value) << US_LONPRIO_PSNB_Pos)))
#define US_LONPRIO_NPS_Pos 8
#define US_LONPRIO_NPS_Msk (0x7fu << US_LONPRIO_NPS_Pos) /**< \brief (US_LONPRIO) LON Node Priority Slot */
#define US_LONPRIO_NPS(value) ((US_LONPRIO_NPS_Msk & ((value) << US_LONPRIO_NPS_Pos)))
/* -------- US_IDTTX : (USART Offset: 0x0080) LON IDT Tx Register -------- */
#define US_IDTTX_IDTTX_Pos 0
#define US_IDTTX_IDTTX_Msk (0xffffffu << US_IDTTX_IDTTX_Pos) /**< \brief (US_IDTTX) LON Indeterminate Time after Transmission (comm_type = 1 mode only) */
#define US_IDTTX_IDTTX(value) ((US_IDTTX_IDTTX_Msk & ((value) << US_IDTTX_IDTTX_Pos)))
/* -------- US_IDTRX : (USART Offset: 0x0084) LON IDT Rx Register -------- */
#define US_IDTRX_IDTRX_Pos 0
#define US_IDTRX_IDTRX_Msk (0xffffffu << US_IDTRX_IDTRX_Pos) /**< \brief (US_IDTRX) LON Indeterminate Time after Reception (comm_type = 1 mode only) */
#define US_IDTRX_IDTRX(value) ((US_IDTRX_IDTRX_Msk & ((value) << US_IDTRX_IDTRX_Pos)))
/* -------- US_ICDIFF : (USART Offset: 0x0088) IC DIFF Register -------- */
#define US_ICDIFF_ICDIFF_Pos 0
#define US_ICDIFF_ICDIFF_Msk (0xfu << US_ICDIFF_ICDIFF_Pos) /**< \brief (US_ICDIFF) IC Differentiator Number */
#define US_ICDIFF_ICDIFF(value) ((US_ICDIFF_ICDIFF_Msk & ((value) << US_ICDIFF_ICDIFF_Pos)))
/* -------- US_WPMR : (USART Offset: 0x00E4) Write Protection Mode Register -------- */
#define US_WPMR_WPEN (0x1u << 0) /**< \brief (US_WPMR) Write Protection Enable */
#define US_WPMR_WPKEY_Pos 8
#define US_WPMR_WPKEY_Msk (0xffffffu << US_WPMR_WPKEY_Pos) /**< \brief (US_WPMR) Write Protection Key */
#define US_WPMR_WPKEY(value) ((US_WPMR_WPKEY_Msk & ((value) << US_WPMR_WPKEY_Pos)))
#define   US_WPMR_WPKEY_PASSWD (0x555341u << 8) /**< \brief (US_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. */
/* -------- US_WPSR : (USART Offset: 0x00E8) Write Protection Status Register -------- */
#define US_WPSR_WPVS (0x1u << 0) /**< \brief (US_WPSR) Write Protection Violation Status */
#define US_WPSR_WPVSRC_Pos 8
#define US_WPSR_WPVSRC_Msk (0xffffu << US_WPSR_WPVSRC_Pos) /**< \brief (US_WPSR) Write Protection Violation Source */
/* -------- US_VERSION : (USART Offset: 0x00FC) Version Register -------- */
#define US_VERSION_VERSION_Pos 0
#define US_VERSION_VERSION_Msk (0xfffu << US_VERSION_VERSION_Pos) /**< \brief (US_VERSION) Hardware Module Version */
#define US_VERSION_MFN_Pos 16
#define US_VERSION_MFN_Msk (0x7u << US_VERSION_MFN_Pos) /**< \brief (US_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_USART_COMPONENT_ */
