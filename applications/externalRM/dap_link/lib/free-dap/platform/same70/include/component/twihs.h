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

#ifndef _SAME70_TWIHS_COMPONENT_
#define _SAME70_TWIHS_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Two-wire Interface High Speed */
/* ============================================================================= */
/** \addtogroup SAME70_TWIHS Two-wire Interface High Speed */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Twihs hardware registers */
typedef struct {
  __O  uint32_t TWIHS_CR;      /**< \brief (Twihs Offset: 0x00) Control Register */
  __IO uint32_t TWIHS_MMR;     /**< \brief (Twihs Offset: 0x04) Master Mode Register */
  __IO uint32_t TWIHS_SMR;     /**< \brief (Twihs Offset: 0x08) Slave Mode Register */
  __IO uint32_t TWIHS_IADR;    /**< \brief (Twihs Offset: 0x0C) Internal Address Register */
  __IO uint32_t TWIHS_CWGR;    /**< \brief (Twihs Offset: 0x10) Clock Waveform Generator Register */
  __I  uint32_t Reserved1[3];
  __I  uint32_t TWIHS_SR;      /**< \brief (Twihs Offset: 0x20) Status Register */
  __O  uint32_t TWIHS_IER;     /**< \brief (Twihs Offset: 0x24) Interrupt Enable Register */
  __O  uint32_t TWIHS_IDR;     /**< \brief (Twihs Offset: 0x28) Interrupt Disable Register */
  __I  uint32_t TWIHS_IMR;     /**< \brief (Twihs Offset: 0x2C) Interrupt Mask Register */
  __I  uint32_t TWIHS_RHR;     /**< \brief (Twihs Offset: 0x30) Receive Holding Register */
  __O  uint32_t TWIHS_THR;     /**< \brief (Twihs Offset: 0x34) Transmit Holding Register */
  __IO uint32_t TWIHS_SMBTR;   /**< \brief (Twihs Offset: 0x38) SMBus Timing Register */
  __I  uint32_t Reserved2[2];
  __IO uint32_t TWIHS_FILTR;   /**< \brief (Twihs Offset: 0x44) Filter Register */
  __I  uint32_t Reserved3[1];
  __IO uint32_t TWIHS_SWMR;    /**< \brief (Twihs Offset: 0x4C) SleepWalking Matching Register */
  __I  uint32_t Reserved4[32];
  __I  uint32_t TWIHS_DR;      /**< \brief (Twihs Offset: 0xD0) Debug Register */
  __I  uint32_t Reserved5[4];
  __IO uint32_t TWIHS_WPMR;    /**< \brief (Twihs Offset: 0xE4) Write Protection Mode Register */
  __I  uint32_t TWIHS_WPSR;    /**< \brief (Twihs Offset: 0xE8) Write Protection Status Register */
  __I  uint32_t Reserved6[4];
  __I  uint32_t TWIHS_VER;     /**< \brief (Twihs Offset: 0xFC) Version Register */
} Twihs;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- TWIHS_CR : (TWIHS Offset: 0x00) Control Register -------- */
#define TWIHS_CR_START (0x1u << 0) /**< \brief (TWIHS_CR) Send a START Condition */
#define TWIHS_CR_STOP (0x1u << 1) /**< \brief (TWIHS_CR) Send a STOP Condition */
#define TWIHS_CR_MSEN (0x1u << 2) /**< \brief (TWIHS_CR) TWIHS Master Mode Enabled */
#define TWIHS_CR_MSDIS (0x1u << 3) /**< \brief (TWIHS_CR) TWIHS Master Mode Disabled */
#define TWIHS_CR_SVEN (0x1u << 4) /**< \brief (TWIHS_CR) TWIHS Slave Mode Enabled */
#define TWIHS_CR_SVDIS (0x1u << 5) /**< \brief (TWIHS_CR) TWIHS Slave Mode Disabled */
#define TWIHS_CR_QUICK (0x1u << 6) /**< \brief (TWIHS_CR) SMBus Quick Command */
#define TWIHS_CR_SWRST (0x1u << 7) /**< \brief (TWIHS_CR) Software Reset */
#define TWIHS_CR_HSEN (0x1u << 8) /**< \brief (TWIHS_CR) TWIHS High-Speed Mode Enabled */
#define TWIHS_CR_HSDIS (0x1u << 9) /**< \brief (TWIHS_CR) TWIHS High-Speed Mode Disabled */
#define TWIHS_CR_SMBEN (0x1u << 10) /**< \brief (TWIHS_CR) SMBus Mode Enabled */
#define TWIHS_CR_SMBDIS (0x1u << 11) /**< \brief (TWIHS_CR) SMBus Mode Disabled */
#define TWIHS_CR_PECEN (0x1u << 12) /**< \brief (TWIHS_CR) Packet Error Checking Enable */
#define TWIHS_CR_PECDIS (0x1u << 13) /**< \brief (TWIHS_CR) Packet Error Checking Disable */
#define TWIHS_CR_PECRQ (0x1u << 14) /**< \brief (TWIHS_CR) PEC Request */
#define TWIHS_CR_CLEAR (0x1u << 15) /**< \brief (TWIHS_CR) Bus CLEAR Command */
#define TWIHS_CR_ACMEN (0x1u << 16) /**< \brief (TWIHS_CR) Alternative Command Mode Enable */
#define TWIHS_CR_ACMDIS (0x1u << 17) /**< \brief (TWIHS_CR) Alternative Command Mode Disable */
#define TWIHS_CR_THRCLR (0x1u << 24) /**< \brief (TWIHS_CR) Transmit Holding Register Clear */
#define TWIHS_CR_LOCKCLR (0x1u << 26) /**< \brief (TWIHS_CR) Lock Clear */
#define TWIHS_CR_FIFOEN (0x1u << 28) /**< \brief (TWIHS_CR) FIFO Enable */
#define TWIHS_CR_FIFODIS (0x1u << 29) /**< \brief (TWIHS_CR) FIFO Disable */
/* -------- TWIHS_MMR : (TWIHS Offset: 0x04) Master Mode Register -------- */
#define TWIHS_MMR_IADRSZ_Pos 8
#define TWIHS_MMR_IADRSZ_Msk (0x3u << TWIHS_MMR_IADRSZ_Pos) /**< \brief (TWIHS_MMR) Internal Device Address Size */
#define TWIHS_MMR_IADRSZ(value) ((TWIHS_MMR_IADRSZ_Msk & ((value) << TWIHS_MMR_IADRSZ_Pos)))
#define   TWIHS_MMR_IADRSZ_NONE (0x0u << 8) /**< \brief (TWIHS_MMR) No internal device address */
#define   TWIHS_MMR_IADRSZ_1_BYTE (0x1u << 8) /**< \brief (TWIHS_MMR) One-byte internal device address */
#define   TWIHS_MMR_IADRSZ_2_BYTE (0x2u << 8) /**< \brief (TWIHS_MMR) Two-byte internal device address */
#define   TWIHS_MMR_IADRSZ_3_BYTE (0x3u << 8) /**< \brief (TWIHS_MMR) Three-byte internal device address */
#define TWIHS_MMR_MREAD (0x1u << 12) /**< \brief (TWIHS_MMR) Master Read Direction */
#define TWIHS_MMR_DADR_Pos 16
#define TWIHS_MMR_DADR_Msk (0x7fu << TWIHS_MMR_DADR_Pos) /**< \brief (TWIHS_MMR) Device Address */
#define TWIHS_MMR_DADR(value) ((TWIHS_MMR_DADR_Msk & ((value) << TWIHS_MMR_DADR_Pos)))
/* -------- TWIHS_SMR : (TWIHS Offset: 0x08) Slave Mode Register -------- */
#define TWIHS_SMR_NACKEN (0x1u << 0) /**< \brief (TWIHS_SMR) Slave Receiver Data Phase NACK enable */
#define TWIHS_SMR_SMDA (0x1u << 2) /**< \brief (TWIHS_SMR) SMBus Default Address */
#define TWIHS_SMR_SMHH (0x1u << 3) /**< \brief (TWIHS_SMR) SMBus Host Header */
#define TWIHS_SMR_SCLWSDIS (0x1u << 6) /**< \brief (TWIHS_SMR) Clock Wait State Disable */
#define TWIHS_SMR_MASK_Pos 8
#define TWIHS_SMR_MASK_Msk (0x7fu << TWIHS_SMR_MASK_Pos) /**< \brief (TWIHS_SMR) Slave Address Mask */
#define TWIHS_SMR_MASK(value) ((TWIHS_SMR_MASK_Msk & ((value) << TWIHS_SMR_MASK_Pos)))
#define TWIHS_SMR_SADR_Pos 16
#define TWIHS_SMR_SADR_Msk (0x7fu << TWIHS_SMR_SADR_Pos) /**< \brief (TWIHS_SMR) Slave Address */
#define TWIHS_SMR_SADR(value) ((TWIHS_SMR_SADR_Msk & ((value) << TWIHS_SMR_SADR_Pos)))
#define TWIHS_SMR_SADR1EN (0x1u << 28) /**< \brief (TWIHS_SMR) Slave Address 1 Enable */
#define TWIHS_SMR_SADR2EN (0x1u << 29) /**< \brief (TWIHS_SMR) Slave Address 2 Enable */
#define TWIHS_SMR_SADR3EN (0x1u << 30) /**< \brief (TWIHS_SMR) Slave Address 3 Enable */
#define TWIHS_SMR_DATAMEN (0x1u << 31) /**< \brief (TWIHS_SMR) Data Matching Enable */
/* -------- TWIHS_IADR : (TWIHS Offset: 0x0C) Internal Address Register -------- */
#define TWIHS_IADR_IADR_Pos 0
#define TWIHS_IADR_IADR_Msk (0xffffffu << TWIHS_IADR_IADR_Pos) /**< \brief (TWIHS_IADR) Internal Address */
#define TWIHS_IADR_IADR(value) ((TWIHS_IADR_IADR_Msk & ((value) << TWIHS_IADR_IADR_Pos)))
/* -------- TWIHS_CWGR : (TWIHS Offset: 0x10) Clock Waveform Generator Register -------- */
#define TWIHS_CWGR_CLDIV_Pos 0
#define TWIHS_CWGR_CLDIV_Msk (0xffu << TWIHS_CWGR_CLDIV_Pos) /**< \brief (TWIHS_CWGR) Clock Low Divider */
#define TWIHS_CWGR_CLDIV(value) ((TWIHS_CWGR_CLDIV_Msk & ((value) << TWIHS_CWGR_CLDIV_Pos)))
#define TWIHS_CWGR_CHDIV_Pos 8
#define TWIHS_CWGR_CHDIV_Msk (0xffu << TWIHS_CWGR_CHDIV_Pos) /**< \brief (TWIHS_CWGR) Clock High Divider */
#define TWIHS_CWGR_CHDIV(value) ((TWIHS_CWGR_CHDIV_Msk & ((value) << TWIHS_CWGR_CHDIV_Pos)))
#define TWIHS_CWGR_CKDIV_Pos 16
#define TWIHS_CWGR_CKDIV_Msk (0x7u << TWIHS_CWGR_CKDIV_Pos) /**< \brief (TWIHS_CWGR) Clock Divider */
#define TWIHS_CWGR_CKDIV(value) ((TWIHS_CWGR_CKDIV_Msk & ((value) << TWIHS_CWGR_CKDIV_Pos)))
#define TWIHS_CWGR_HOLD_Pos 24
#define TWIHS_CWGR_HOLD_Msk (0x3fu << TWIHS_CWGR_HOLD_Pos) /**< \brief (TWIHS_CWGR) TWD Hold Time Versus TWCK Falling */
#define TWIHS_CWGR_HOLD(value) ((TWIHS_CWGR_HOLD_Msk & ((value) << TWIHS_CWGR_HOLD_Pos)))
/* -------- TWIHS_SR : (TWIHS Offset: 0x20) Status Register -------- */
#define TWIHS_SR_TXCOMP (0x1u << 0) /**< \brief (TWIHS_SR) Transmission Completed (cleared by writing TWIHS_THR) */
#define TWIHS_SR_RXRDY (0x1u << 1) /**< \brief (TWIHS_SR) Receive Holding Register Ready (cleared by reading TWIHS_RHR) */
#define TWIHS_SR_TXRDY (0x1u << 2) /**< \brief (TWIHS_SR) Transmit Holding Register Ready (cleared by writing TWIHS_THR) */
#define TWIHS_SR_SVREAD (0x1u << 3) /**< \brief (TWIHS_SR) Slave Read */
#define TWIHS_SR_SVACC (0x1u << 4) /**< \brief (TWIHS_SR) Slave Access */
#define TWIHS_SR_GACC (0x1u << 5) /**< \brief (TWIHS_SR) General Call Access (cleared on read) */
#define TWIHS_SR_OVRE (0x1u << 6) /**< \brief (TWIHS_SR) Overrun Error (cleared on read) */
#define TWIHS_SR_UNRE (0x1u << 7) /**< \brief (TWIHS_SR) Underrun Error (cleared on read) */
#define TWIHS_SR_NACK (0x1u << 8) /**< \brief (TWIHS_SR) Not Acknowledged (cleared on read) */
#define TWIHS_SR_ARBLST (0x1u << 9) /**< \brief (TWIHS_SR) Arbitration Lost (cleared on read) */
#define TWIHS_SR_SCLWS (0x1u << 10) /**< \brief (TWIHS_SR) Clock Wait State */
#define TWIHS_SR_EOSACC (0x1u << 11) /**< \brief (TWIHS_SR) End Of Slave Access (cleared on read) */
#define TWIHS_SR_MCACK (0x1u << 16) /**< \brief (TWIHS_SR) Master Code Acknowledge (cleared on read) */
#define TWIHS_SR_TOUT (0x1u << 18) /**< \brief (TWIHS_SR) Timeout Error (cleared on read) */
#define TWIHS_SR_PECERR (0x1u << 19) /**< \brief (TWIHS_SR) PEC Error (cleared on read) */
#define TWIHS_SR_SMBDAM (0x1u << 20) /**< \brief (TWIHS_SR) SMBus Default Address Match (cleared on read) */
#define TWIHS_SR_SMBHHM (0x1u << 21) /**< \brief (TWIHS_SR) SMBus Host Header Address Match (cleared on read) */
#define TWIHS_SR_SCL (0x1u << 24) /**< \brief (TWIHS_SR) SCL line value */
#define TWIHS_SR_SDA (0x1u << 25) /**< \brief (TWIHS_SR) SDA line value */
/* -------- TWIHS_IER : (TWIHS Offset: 0x24) Interrupt Enable Register -------- */
#define TWIHS_IER_TXCOMP (0x1u << 0) /**< \brief (TWIHS_IER) Transmission Completed Interrupt Enable */
#define TWIHS_IER_RXRDY (0x1u << 1) /**< \brief (TWIHS_IER) Receive Holding Register Ready Interrupt Enable */
#define TWIHS_IER_TXRDY (0x1u << 2) /**< \brief (TWIHS_IER) Transmit Holding Register Ready Interrupt Enable */
#define TWIHS_IER_SVACC (0x1u << 4) /**< \brief (TWIHS_IER) Slave Access Interrupt Enable */
#define TWIHS_IER_GACC (0x1u << 5) /**< \brief (TWIHS_IER) General Call Access Interrupt Enable */
#define TWIHS_IER_OVRE (0x1u << 6) /**< \brief (TWIHS_IER) Overrun Error Interrupt Enable */
#define TWIHS_IER_UNRE (0x1u << 7) /**< \brief (TWIHS_IER) Underrun Error Interrupt Enable */
#define TWIHS_IER_NACK (0x1u << 8) /**< \brief (TWIHS_IER) Not Acknowledge Interrupt Enable */
#define TWIHS_IER_ARBLST (0x1u << 9) /**< \brief (TWIHS_IER) Arbitration Lost Interrupt Enable */
#define TWIHS_IER_SCL_WS (0x1u << 10) /**< \brief (TWIHS_IER) Clock Wait State Interrupt Enable */
#define TWIHS_IER_EOSACC (0x1u << 11) /**< \brief (TWIHS_IER) End Of Slave Access Interrupt Enable */
#define TWIHS_IER_MCACK (0x1u << 16) /**< \brief (TWIHS_IER) Master Code Acknowledge Interrupt Enable */
#define TWIHS_IER_TOUT (0x1u << 18) /**< \brief (TWIHS_IER) Timeout Error Interrupt Enable */
#define TWIHS_IER_PECERR (0x1u << 19) /**< \brief (TWIHS_IER) PEC Error Interrupt Enable */
#define TWIHS_IER_SMBDAM (0x1u << 20) /**< \brief (TWIHS_IER) SMBus Default Address Match Interrupt Enable */
#define TWIHS_IER_SMBHHM (0x1u << 21) /**< \brief (TWIHS_IER) SMBus Host Header Address Match Interrupt Enable */
/* -------- TWIHS_IDR : (TWIHS Offset: 0x28) Interrupt Disable Register -------- */
#define TWIHS_IDR_TXCOMP (0x1u << 0) /**< \brief (TWIHS_IDR) Transmission Completed Interrupt Disable */
#define TWIHS_IDR_RXRDY (0x1u << 1) /**< \brief (TWIHS_IDR) Receive Holding Register Ready Interrupt Disable */
#define TWIHS_IDR_TXRDY (0x1u << 2) /**< \brief (TWIHS_IDR) Transmit Holding Register Ready Interrupt Disable */
#define TWIHS_IDR_SVACC (0x1u << 4) /**< \brief (TWIHS_IDR) Slave Access Interrupt Disable */
#define TWIHS_IDR_GACC (0x1u << 5) /**< \brief (TWIHS_IDR) General Call Access Interrupt Disable */
#define TWIHS_IDR_OVRE (0x1u << 6) /**< \brief (TWIHS_IDR) Overrun Error Interrupt Disable */
#define TWIHS_IDR_UNRE (0x1u << 7) /**< \brief (TWIHS_IDR) Underrun Error Interrupt Disable */
#define TWIHS_IDR_NACK (0x1u << 8) /**< \brief (TWIHS_IDR) Not Acknowledge Interrupt Disable */
#define TWIHS_IDR_ARBLST (0x1u << 9) /**< \brief (TWIHS_IDR) Arbitration Lost Interrupt Disable */
#define TWIHS_IDR_SCL_WS (0x1u << 10) /**< \brief (TWIHS_IDR) Clock Wait State Interrupt Disable */
#define TWIHS_IDR_EOSACC (0x1u << 11) /**< \brief (TWIHS_IDR) End Of Slave Access Interrupt Disable */
#define TWIHS_IDR_MCACK (0x1u << 16) /**< \brief (TWIHS_IDR) Master Code Acknowledge Interrupt Disable */
#define TWIHS_IDR_TOUT (0x1u << 18) /**< \brief (TWIHS_IDR) Timeout Error Interrupt Disable */
#define TWIHS_IDR_PECERR (0x1u << 19) /**< \brief (TWIHS_IDR) PEC Error Interrupt Disable */
#define TWIHS_IDR_SMBDAM (0x1u << 20) /**< \brief (TWIHS_IDR) SMBus Default Address Match Interrupt Disable */
#define TWIHS_IDR_SMBHHM (0x1u << 21) /**< \brief (TWIHS_IDR) SMBus Host Header Address Match Interrupt Disable */
/* -------- TWIHS_IMR : (TWIHS Offset: 0x2C) Interrupt Mask Register -------- */
#define TWIHS_IMR_TXCOMP (0x1u << 0) /**< \brief (TWIHS_IMR) Transmission Completed Interrupt Mask */
#define TWIHS_IMR_RXRDY (0x1u << 1) /**< \brief (TWIHS_IMR) Receive Holding Register Ready Interrupt Mask */
#define TWIHS_IMR_TXRDY (0x1u << 2) /**< \brief (TWIHS_IMR) Transmit Holding Register Ready Interrupt Mask */
#define TWIHS_IMR_SVACC (0x1u << 4) /**< \brief (TWIHS_IMR) Slave Access Interrupt Mask */
#define TWIHS_IMR_GACC (0x1u << 5) /**< \brief (TWIHS_IMR) General Call Access Interrupt Mask */
#define TWIHS_IMR_OVRE (0x1u << 6) /**< \brief (TWIHS_IMR) Overrun Error Interrupt Mask */
#define TWIHS_IMR_UNRE (0x1u << 7) /**< \brief (TWIHS_IMR) Underrun Error Interrupt Mask */
#define TWIHS_IMR_NACK (0x1u << 8) /**< \brief (TWIHS_IMR) Not Acknowledge Interrupt Mask */
#define TWIHS_IMR_ARBLST (0x1u << 9) /**< \brief (TWIHS_IMR) Arbitration Lost Interrupt Mask */
#define TWIHS_IMR_SCL_WS (0x1u << 10) /**< \brief (TWIHS_IMR) Clock Wait State Interrupt Mask */
#define TWIHS_IMR_EOSACC (0x1u << 11) /**< \brief (TWIHS_IMR) End Of Slave Access Interrupt Mask */
#define TWIHS_IMR_MCACK (0x1u << 16) /**< \brief (TWIHS_IMR) Master Code Acknowledge Interrupt Mask */
#define TWIHS_IMR_TOUT (0x1u << 18) /**< \brief (TWIHS_IMR) Timeout Error Interrupt Mask */
#define TWIHS_IMR_PECERR (0x1u << 19) /**< \brief (TWIHS_IMR) PEC Error Interrupt Mask */
#define TWIHS_IMR_SMBDAM (0x1u << 20) /**< \brief (TWIHS_IMR) SMBus Default Address Match Interrupt Mask */
#define TWIHS_IMR_SMBHHM (0x1u << 21) /**< \brief (TWIHS_IMR) SMBus Host Header Address Match Interrupt Mask */
/* -------- TWIHS_RHR : (TWIHS Offset: 0x30) Receive Holding Register -------- */
#define TWIHS_RHR_RXDATA_Pos 0
#define TWIHS_RHR_RXDATA_Msk (0xffu << TWIHS_RHR_RXDATA_Pos) /**< \brief (TWIHS_RHR) Master or Slave Receive Holding Data */
/* -------- TWIHS_THR : (TWIHS Offset: 0x34) Transmit Holding Register -------- */
#define TWIHS_THR_TXDATA_Pos 0
#define TWIHS_THR_TXDATA_Msk (0xffu << TWIHS_THR_TXDATA_Pos) /**< \brief (TWIHS_THR) Master or Slave Transmit Holding Data */
#define TWIHS_THR_TXDATA(value) ((TWIHS_THR_TXDATA_Msk & ((value) << TWIHS_THR_TXDATA_Pos)))
/* -------- TWIHS_SMBTR : (TWIHS Offset: 0x38) SMBus Timing Register -------- */
#define TWIHS_SMBTR_PRESC_Pos 0
#define TWIHS_SMBTR_PRESC_Msk (0xfu << TWIHS_SMBTR_PRESC_Pos) /**< \brief (TWIHS_SMBTR) SMBus Clock Prescaler */
#define TWIHS_SMBTR_PRESC(value) ((TWIHS_SMBTR_PRESC_Msk & ((value) << TWIHS_SMBTR_PRESC_Pos)))
#define TWIHS_SMBTR_TLOWS_Pos 8
#define TWIHS_SMBTR_TLOWS_Msk (0xffu << TWIHS_SMBTR_TLOWS_Pos) /**< \brief (TWIHS_SMBTR) Slave Clock Stretch Maximum Cycles */
#define TWIHS_SMBTR_TLOWS(value) ((TWIHS_SMBTR_TLOWS_Msk & ((value) << TWIHS_SMBTR_TLOWS_Pos)))
#define TWIHS_SMBTR_TLOWM_Pos 16
#define TWIHS_SMBTR_TLOWM_Msk (0xffu << TWIHS_SMBTR_TLOWM_Pos) /**< \brief (TWIHS_SMBTR) Master Clock Stretch Maximum Cycles */
#define TWIHS_SMBTR_TLOWM(value) ((TWIHS_SMBTR_TLOWM_Msk & ((value) << TWIHS_SMBTR_TLOWM_Pos)))
#define TWIHS_SMBTR_THMAX_Pos 24
#define TWIHS_SMBTR_THMAX_Msk (0xffu << TWIHS_SMBTR_THMAX_Pos) /**< \brief (TWIHS_SMBTR) Clock High Maximum Cycles */
#define TWIHS_SMBTR_THMAX(value) ((TWIHS_SMBTR_THMAX_Msk & ((value) << TWIHS_SMBTR_THMAX_Pos)))
/* -------- TWIHS_FILTR : (TWIHS Offset: 0x44) Filter Register -------- */
#define TWIHS_FILTR_FILT (0x1u << 0) /**< \brief (TWIHS_FILTR) RX Digital Filter */
#define TWIHS_FILTR_PADFEN (0x1u << 1) /**< \brief (TWIHS_FILTR) PAD Filter Enable */
#define TWIHS_FILTR_PADFCFG (0x1u << 2) /**< \brief (TWIHS_FILTR) PAD Filter Config */
#define TWIHS_FILTR_THRES_Pos 8
#define TWIHS_FILTR_THRES_Msk (0x7u << TWIHS_FILTR_THRES_Pos) /**< \brief (TWIHS_FILTR) Digital Filter Threshold */
#define TWIHS_FILTR_THRES(value) ((TWIHS_FILTR_THRES_Msk & ((value) << TWIHS_FILTR_THRES_Pos)))
/* -------- TWIHS_SWMR : (TWIHS Offset: 0x4C) SleepWalking Matching Register -------- */
#define TWIHS_SWMR_SADR1_Pos 0
#define TWIHS_SWMR_SADR1_Msk (0x7fu << TWIHS_SWMR_SADR1_Pos) /**< \brief (TWIHS_SWMR) Slave Address 1 */
#define TWIHS_SWMR_SADR1(value) ((TWIHS_SWMR_SADR1_Msk & ((value) << TWIHS_SWMR_SADR1_Pos)))
#define TWIHS_SWMR_SADR2_Pos 8
#define TWIHS_SWMR_SADR2_Msk (0x7fu << TWIHS_SWMR_SADR2_Pos) /**< \brief (TWIHS_SWMR) Slave Address 2 */
#define TWIHS_SWMR_SADR2(value) ((TWIHS_SWMR_SADR2_Msk & ((value) << TWIHS_SWMR_SADR2_Pos)))
#define TWIHS_SWMR_SADR3_Pos 16
#define TWIHS_SWMR_SADR3_Msk (0x7fu << TWIHS_SWMR_SADR3_Pos) /**< \brief (TWIHS_SWMR) Slave Address 3 */
#define TWIHS_SWMR_SADR3(value) ((TWIHS_SWMR_SADR3_Msk & ((value) << TWIHS_SWMR_SADR3_Pos)))
#define TWIHS_SWMR_DATAM_Pos 24
#define TWIHS_SWMR_DATAM_Msk (0xffu << TWIHS_SWMR_DATAM_Pos) /**< \brief (TWIHS_SWMR) Data Match */
#define TWIHS_SWMR_DATAM(value) ((TWIHS_SWMR_DATAM_Msk & ((value) << TWIHS_SWMR_DATAM_Pos)))
/* -------- TWIHS_DR : (TWIHS Offset: 0xD0) Debug Register -------- */
#define TWIHS_DR_SWEN (0x1u << 0) /**< \brief (TWIHS_DR) SleepWalking Enable */
#define TWIHS_DR_CLKRQ (0x1u << 1) /**< \brief (TWIHS_DR) Clock Request */
#define TWIHS_DR_SWMATCH (0x1u << 2) /**< \brief (TWIHS_DR) SleepWalking Match */
#define TWIHS_DR_TRP (0x1u << 3) /**< \brief (TWIHS_DR) Transfer Pending */
/* -------- TWIHS_WPMR : (TWIHS Offset: 0xE4) Write Protection Mode Register -------- */
#define TWIHS_WPMR_WPEN (0x1u << 0) /**< \brief (TWIHS_WPMR) Write Protection Enable */
#define TWIHS_WPMR_WPKEY_Pos 8
#define TWIHS_WPMR_WPKEY_Msk (0xffffffu << TWIHS_WPMR_WPKEY_Pos) /**< \brief (TWIHS_WPMR) Write Protection Key */
#define TWIHS_WPMR_WPKEY(value) ((TWIHS_WPMR_WPKEY_Msk & ((value) << TWIHS_WPMR_WPKEY_Pos)))
#define   TWIHS_WPMR_WPKEY_PASSWD (0x545749u << 8) /**< \brief (TWIHS_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0 */
/* -------- TWIHS_WPSR : (TWIHS Offset: 0xE8) Write Protection Status Register -------- */
#define TWIHS_WPSR_WPVS (0x1u << 0) /**< \brief (TWIHS_WPSR) Write Protection Violation Status */
#define TWIHS_WPSR_WPVSRC_Pos 8
#define TWIHS_WPSR_WPVSRC_Msk (0xffffffu << TWIHS_WPSR_WPVSRC_Pos) /**< \brief (TWIHS_WPSR) Write Protection Violation Source */
/* -------- TWIHS_VER : (TWIHS Offset: 0xFC) Version Register -------- */
#define TWIHS_VER_VERSION_Pos 0
#define TWIHS_VER_VERSION_Msk (0xfffu << TWIHS_VER_VERSION_Pos) /**< \brief (TWIHS_VER) Version of the Hardware Module */
#define TWIHS_VER_MFN_Pos 16
#define TWIHS_VER_MFN_Msk (0x7u << TWIHS_VER_MFN_Pos) /**< \brief (TWIHS_VER) Metal Fix Number */

/*@}*/


#endif /* _SAME70_TWIHS_COMPONENT_ */
