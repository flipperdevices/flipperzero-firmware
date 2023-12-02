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

#ifndef _SAME70_QSPI_COMPONENT_
#define _SAME70_QSPI_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Quad Serial Peripheral Interface */
/* ============================================================================= */
/** \addtogroup SAME70_QSPI Quad Serial Peripheral Interface */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Qspi hardware registers */
typedef struct {
  __O  uint32_t QSPI_CR;       /**< \brief (Qspi Offset: 0x00) Control Register */
  __IO uint32_t QSPI_MR;       /**< \brief (Qspi Offset: 0x04) Mode Register */
  __I  uint32_t QSPI_RDR;      /**< \brief (Qspi Offset: 0x08) Receive Data Register */
  __O  uint32_t QSPI_TDR;      /**< \brief (Qspi Offset: 0x0C) Transmit Data Register */
  __I  uint32_t QSPI_SR;       /**< \brief (Qspi Offset: 0x10) Status Register */
  __O  uint32_t QSPI_IER;      /**< \brief (Qspi Offset: 0x14) Interrupt Enable Register */
  __O  uint32_t QSPI_IDR;      /**< \brief (Qspi Offset: 0x18) Interrupt Disable Register */
  __I  uint32_t QSPI_IMR;      /**< \brief (Qspi Offset: 0x1C) Interrupt Mask Register */
  __IO uint32_t QSPI_SCR;      /**< \brief (Qspi Offset: 0x20) Serial Clock Register */
  __I  uint32_t Reserved1[3];
  __IO uint32_t QSPI_IAR;      /**< \brief (Qspi Offset: 0x30) Instruction Address Register */
  __IO uint32_t QSPI_ICR;      /**< \brief (Qspi Offset: 0x34) Instruction Code Register */
  __IO uint32_t QSPI_IFR;      /**< \brief (Qspi Offset: 0x38) Instruction Frame Register */
  __I  uint32_t Reserved2[1];
  __IO uint32_t QSPI_SMR;      /**< \brief (Qspi Offset: 0x40) Scrambling Mode Register */
  __O  uint32_t QSPI_SKR;      /**< \brief (Qspi Offset: 0x44) Scrambling Key Register */
  __I  uint32_t Reserved3[39];
  __IO uint32_t QSPI_WPMR;     /**< \brief (Qspi Offset: 0xE4) Write Protection Mode Register */
  __I  uint32_t QSPI_WPSR;     /**< \brief (Qspi Offset: 0xE8) Write Protection Status Register */
  __I  uint32_t Reserved4[4];
  __I  uint32_t QSPI_VERSION;  /**< \brief (Qspi Offset: 0x00FC) Version Register */
} Qspi;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- QSPI_CR : (QSPI Offset: 0x00) Control Register -------- */
#define QSPI_CR_QSPIEN (0x1u << 0) /**< \brief (QSPI_CR) QSPI Enable */
#define QSPI_CR_QSPIDIS (0x1u << 1) /**< \brief (QSPI_CR) QSPI Disable */
#define QSPI_CR_SWRST (0x1u << 7) /**< \brief (QSPI_CR) QSPI Software Reset */
#define QSPI_CR_LASTXFER (0x1u << 24) /**< \brief (QSPI_CR) Last Transfer */
/* -------- QSPI_MR : (QSPI Offset: 0x04) Mode Register -------- */
#define QSPI_MR_SMM (0x1u << 0) /**< \brief (QSPI_MR) Serial Memory Mode */
#define   QSPI_MR_SMM_SPI (0x0u << 0) /**< \brief (QSPI_MR) The QSPI is in SPI mode. */
#define   QSPI_MR_SMM_MEMORY (0x1u << 0) /**< \brief (QSPI_MR) The QSPI is in Serial Memory mode. */
#define QSPI_MR_LLB (0x1u << 1) /**< \brief (QSPI_MR) Local Loopback Enable */
#define   QSPI_MR_LLB_DISABLED (0x0u << 1) /**< \brief (QSPI_MR) Local loopback path disabled. */
#define   QSPI_MR_LLB_ENABLED (0x1u << 1) /**< \brief (QSPI_MR) Local loopback path enabled. */
#define QSPI_MR_WDRBT (0x1u << 2) /**< \brief (QSPI_MR) Wait Data Read Before Transfer */
#define   QSPI_MR_WDRBT_DISABLED (0x0u << 2) /**< \brief (QSPI_MR) No effect. In SPI mode, a transfer can be initiated whatever the state of the QSPI_RDR is. */
#define   QSPI_MR_WDRBT_ENABLED (0x1u << 2) /**< \brief (QSPI_MR) In SPI mode, a transfer can start only if the QSPI_RDR is empty, i.e., does not contain any unread data. This mode prevents overrun error in reception. */
#define QSPI_MR_CSMODE_Pos 4
#define QSPI_MR_CSMODE_Msk (0x3u << QSPI_MR_CSMODE_Pos) /**< \brief (QSPI_MR) Chip Select Mode */
#define QSPI_MR_CSMODE(value) ((QSPI_MR_CSMODE_Msk & ((value) << QSPI_MR_CSMODE_Pos)))
#define   QSPI_MR_CSMODE_NOT_RELOADED (0x0u << 4) /**< \brief (QSPI_MR) The chip select is deasserted if TD has not been reloaded before the end of the current transfer. */
#define   QSPI_MR_CSMODE_LASTXFER (0x1u << 4) /**< \brief (QSPI_MR) The chip select is deasserted when the bit LASTXFER is written at 1 and the character written in TD has been transferred. */
#define   QSPI_MR_CSMODE_SYSTEMATICALLY (0x2u << 4) /**< \brief (QSPI_MR) The chip select is deasserted systematically after each transfer. */
#define QSPI_MR_NBBITS_Pos 8
#define QSPI_MR_NBBITS_Msk (0xfu << QSPI_MR_NBBITS_Pos) /**< \brief (QSPI_MR) Number Of Bits Per Transfer */
#define QSPI_MR_NBBITS(value) ((QSPI_MR_NBBITS_Msk & ((value) << QSPI_MR_NBBITS_Pos)))
#define   QSPI_MR_NBBITS_8_BIT (0x0u << 8) /**< \brief (QSPI_MR) 8 bits for transfer */
#define   QSPI_MR_NBBITS_9_BIT (0x1u << 8) /**< \brief (QSPI_MR) 9 bits for transfer */
#define   QSPI_MR_NBBITS_10_BIT (0x2u << 8) /**< \brief (QSPI_MR) 10 bits for transfer */
#define   QSPI_MR_NBBITS_11_BIT (0x3u << 8) /**< \brief (QSPI_MR) 11 bits for transfer */
#define   QSPI_MR_NBBITS_12_BIT (0x4u << 8) /**< \brief (QSPI_MR) 12 bits for transfer */
#define   QSPI_MR_NBBITS_13_BIT (0x5u << 8) /**< \brief (QSPI_MR) 13 bits for transfer */
#define   QSPI_MR_NBBITS_14_BIT (0x6u << 8) /**< \brief (QSPI_MR) 14 bits for transfer */
#define   QSPI_MR_NBBITS_15_BIT (0x7u << 8) /**< \brief (QSPI_MR) 15 bits for transfer */
#define   QSPI_MR_NBBITS_16_BIT (0x8u << 8) /**< \brief (QSPI_MR) 16 bits for transfer */
#define QSPI_MR_DLYBCT_Pos 16
#define QSPI_MR_DLYBCT_Msk (0xffu << QSPI_MR_DLYBCT_Pos) /**< \brief (QSPI_MR) Delay Between Consecutive Transfers */
#define QSPI_MR_DLYBCT(value) ((QSPI_MR_DLYBCT_Msk & ((value) << QSPI_MR_DLYBCT_Pos)))
#define QSPI_MR_DLYCS_Pos 24
#define QSPI_MR_DLYCS_Msk (0xffu << QSPI_MR_DLYCS_Pos) /**< \brief (QSPI_MR) Minimum Inactive QCS Delay */
#define QSPI_MR_DLYCS(value) ((QSPI_MR_DLYCS_Msk & ((value) << QSPI_MR_DLYCS_Pos)))
/* -------- QSPI_RDR : (QSPI Offset: 0x08) Receive Data Register -------- */
#define QSPI_RDR_RD_Pos 0
#define QSPI_RDR_RD_Msk (0xffffu << QSPI_RDR_RD_Pos) /**< \brief (QSPI_RDR) Receive Data */
/* -------- QSPI_TDR : (QSPI Offset: 0x0C) Transmit Data Register -------- */
#define QSPI_TDR_TD_Pos 0
#define QSPI_TDR_TD_Msk (0xffffu << QSPI_TDR_TD_Pos) /**< \brief (QSPI_TDR) Transmit Data */
#define QSPI_TDR_TD(value) ((QSPI_TDR_TD_Msk & ((value) << QSPI_TDR_TD_Pos)))
/* -------- QSPI_SR : (QSPI Offset: 0x10) Status Register -------- */
#define QSPI_SR_RDRF (0x1u << 0) /**< \brief (QSPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) */
#define QSPI_SR_TDRE (0x1u << 1) /**< \brief (QSPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) */
#define QSPI_SR_TXEMPTY (0x1u << 2) /**< \brief (QSPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) */
#define QSPI_SR_OVRES (0x1u << 3) /**< \brief (QSPI_SR) Overrun Error Status (cleared on read) */
#define QSPI_SR_CSR (0x1u << 8) /**< \brief (QSPI_SR) Chip Select Rise */
#define QSPI_SR_CSS (0x1u << 9) /**< \brief (QSPI_SR) Chip Select Status */
#define QSPI_SR_INSTRE (0x1u << 10) /**< \brief (QSPI_SR) Instruction End Status */
#define QSPI_SR_QSPIENS (0x1u << 24) /**< \brief (QSPI_SR) QSPI Enable Status */
/* -------- QSPI_IER : (QSPI Offset: 0x14) Interrupt Enable Register -------- */
#define QSPI_IER_RDRF (0x1u << 0) /**< \brief (QSPI_IER) Receive Data Register Full Interrupt Enable */
#define QSPI_IER_TDRE (0x1u << 1) /**< \brief (QSPI_IER) Transmit Data Register Empty Interrupt Enable */
#define QSPI_IER_TXEMPTY (0x1u << 2) /**< \brief (QSPI_IER) Transmission Registers Empty Enable */
#define QSPI_IER_OVRES (0x1u << 3) /**< \brief (QSPI_IER) Overrun Error Interrupt Enable */
#define QSPI_IER_CSR (0x1u << 8) /**< \brief (QSPI_IER) Chip Select Rise Interrupt Enable */
#define QSPI_IER_CSS (0x1u << 9) /**< \brief (QSPI_IER) Chip Select Status Interrupt Enable */
#define QSPI_IER_INSTRE (0x1u << 10) /**< \brief (QSPI_IER) Instruction End Interrupt Enable */
/* -------- QSPI_IDR : (QSPI Offset: 0x18) Interrupt Disable Register -------- */
#define QSPI_IDR_RDRF (0x1u << 0) /**< \brief (QSPI_IDR) Receive Data Register Full Interrupt Disable */
#define QSPI_IDR_TDRE (0x1u << 1) /**< \brief (QSPI_IDR) Transmit Data Register Empty Interrupt Disable */
#define QSPI_IDR_TXEMPTY (0x1u << 2) /**< \brief (QSPI_IDR) Transmission Registers Empty Disable */
#define QSPI_IDR_OVRES (0x1u << 3) /**< \brief (QSPI_IDR) Overrun Error Interrupt Disable */
#define QSPI_IDR_CSR (0x1u << 8) /**< \brief (QSPI_IDR) Chip Select Rise Interrupt Disable */
#define QSPI_IDR_CSS (0x1u << 9) /**< \brief (QSPI_IDR) Chip Select Status Interrupt Disable */
#define QSPI_IDR_INSTRE (0x1u << 10) /**< \brief (QSPI_IDR) Instruction End Interrupt Disable */
/* -------- QSPI_IMR : (QSPI Offset: 0x1C) Interrupt Mask Register -------- */
#define QSPI_IMR_RDRF (0x1u << 0) /**< \brief (QSPI_IMR) Receive Data Register Full Interrupt Mask */
#define QSPI_IMR_TDRE (0x1u << 1) /**< \brief (QSPI_IMR) Transmit Data Register Empty Interrupt Mask */
#define QSPI_IMR_TXEMPTY (0x1u << 2) /**< \brief (QSPI_IMR) Transmission Registers Empty Mask */
#define QSPI_IMR_OVRES (0x1u << 3) /**< \brief (QSPI_IMR) Overrun Error Interrupt Mask */
#define QSPI_IMR_CSR (0x1u << 8) /**< \brief (QSPI_IMR) Chip Select Rise Interrupt Mask */
#define QSPI_IMR_CSS (0x1u << 9) /**< \brief (QSPI_IMR) Chip Select Status Interrupt Mask */
#define QSPI_IMR_INSTRE (0x1u << 10) /**< \brief (QSPI_IMR) Instruction End Interrupt Mask */
/* -------- QSPI_SCR : (QSPI Offset: 0x20) Serial Clock Register -------- */
#define QSPI_SCR_CPOL (0x1u << 0) /**< \brief (QSPI_SCR) Clock Polarity */
#define QSPI_SCR_CPHA (0x1u << 1) /**< \brief (QSPI_SCR) Clock Phase */
#define QSPI_SCR_SCBR_Pos 8
#define QSPI_SCR_SCBR_Msk (0xffu << QSPI_SCR_SCBR_Pos) /**< \brief (QSPI_SCR) Serial Clock Baud Rate */
#define QSPI_SCR_SCBR(value) ((QSPI_SCR_SCBR_Msk & ((value) << QSPI_SCR_SCBR_Pos)))
#define QSPI_SCR_DLYBS_Pos 16
#define QSPI_SCR_DLYBS_Msk (0xffu << QSPI_SCR_DLYBS_Pos) /**< \brief (QSPI_SCR) Delay Before QSCK */
#define QSPI_SCR_DLYBS(value) ((QSPI_SCR_DLYBS_Msk & ((value) << QSPI_SCR_DLYBS_Pos)))
/* -------- QSPI_IAR : (QSPI Offset: 0x30) Instruction Address Register -------- */
#define QSPI_IAR_ADDR_Pos 0
#define QSPI_IAR_ADDR_Msk (0xffffffffu << QSPI_IAR_ADDR_Pos) /**< \brief (QSPI_IAR) Address */
#define QSPI_IAR_ADDR(value) ((QSPI_IAR_ADDR_Msk & ((value) << QSPI_IAR_ADDR_Pos)))
/* -------- QSPI_ICR : (QSPI Offset: 0x34) Instruction Code Register -------- */
#define QSPI_ICR_INST_Pos 0
#define QSPI_ICR_INST_Msk (0xffu << QSPI_ICR_INST_Pos) /**< \brief (QSPI_ICR) Instruction Code */
#define QSPI_ICR_INST(value) ((QSPI_ICR_INST_Msk & ((value) << QSPI_ICR_INST_Pos)))
#define QSPI_ICR_OPT_Pos 16
#define QSPI_ICR_OPT_Msk (0xffu << QSPI_ICR_OPT_Pos) /**< \brief (QSPI_ICR) Option Code */
#define QSPI_ICR_OPT(value) ((QSPI_ICR_OPT_Msk & ((value) << QSPI_ICR_OPT_Pos)))
/* -------- QSPI_IFR : (QSPI Offset: 0x38) Instruction Frame Register -------- */
#define QSPI_IFR_WIDTH_Pos 0
#define QSPI_IFR_WIDTH_Msk (0x7u << QSPI_IFR_WIDTH_Pos) /**< \brief (QSPI_IFR) Width of Instruction Code, Address, Option Code and Data */
#define QSPI_IFR_WIDTH(value) ((QSPI_IFR_WIDTH_Msk & ((value) << QSPI_IFR_WIDTH_Pos)))
#define   QSPI_IFR_WIDTH_SINGLE_BIT_SPI (0x0u << 0) /**< \brief (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Single-bit SPI */
#define   QSPI_IFR_WIDTH_DUAL_OUTPUT (0x1u << 0) /**< \brief (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Dual SPI */
#define   QSPI_IFR_WIDTH_QUAD_OUTPUT (0x2u << 0) /**< \brief (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Quad SPI */
#define   QSPI_IFR_WIDTH_DUAL_IO (0x3u << 0) /**< \brief (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Dual SPI / Data: Dual SPI */
#define   QSPI_IFR_WIDTH_QUAD_IO (0x4u << 0) /**< \brief (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Quad SPI / Data: Quad SPI */
#define   QSPI_IFR_WIDTH_DUAL_CMD (0x5u << 0) /**< \brief (QSPI_IFR) Instruction: Dual SPI / Address-Option: Dual SPI / Data: Dual SPI */
#define   QSPI_IFR_WIDTH_QUAD_CMD (0x6u << 0) /**< \brief (QSPI_IFR) Instruction: Quad SPI / Address-Option: Quad SPI / Data: Quad SPI */
#define QSPI_IFR_INSTEN (0x1u << 4) /**< \brief (QSPI_IFR) Instruction Enable */
#define QSPI_IFR_ADDREN (0x1u << 5) /**< \brief (QSPI_IFR) Address Enable */
#define QSPI_IFR_OPTEN (0x1u << 6) /**< \brief (QSPI_IFR) Option Enable */
#define QSPI_IFR_DATAEN (0x1u << 7) /**< \brief (QSPI_IFR) Data Enable */
#define QSPI_IFR_OPTL_Pos 8
#define QSPI_IFR_OPTL_Msk (0x3u << QSPI_IFR_OPTL_Pos) /**< \brief (QSPI_IFR) Option Code Length */
#define QSPI_IFR_OPTL(value) ((QSPI_IFR_OPTL_Msk & ((value) << QSPI_IFR_OPTL_Pos)))
#define   QSPI_IFR_OPTL_OPTION_1BIT (0x0u << 8) /**< \brief (QSPI_IFR) The option code is 1 bit long. */
#define   QSPI_IFR_OPTL_OPTION_2BIT (0x1u << 8) /**< \brief (QSPI_IFR) The option code is 2 bits long. */
#define   QSPI_IFR_OPTL_OPTION_4BIT (0x2u << 8) /**< \brief (QSPI_IFR) The option code is 4 bits long. */
#define   QSPI_IFR_OPTL_OPTION_8BIT (0x3u << 8) /**< \brief (QSPI_IFR) The option code is 8 bits long. */
#define QSPI_IFR_ADDRL (0x1u << 10) /**< \brief (QSPI_IFR) Address Length */
#define   QSPI_IFR_ADDRL_24_BIT (0x0u << 10) /**< \brief (QSPI_IFR) The address is 24 bits long. */
#define   QSPI_IFR_ADDRL_32_BIT (0x1u << 10) /**< \brief (QSPI_IFR) The address is 32 bits long. */
#define QSPI_IFR_TFRTYP_Pos 12
#define QSPI_IFR_TFRTYP_Msk (0x3u << QSPI_IFR_TFRTYP_Pos) /**< \brief (QSPI_IFR) Data Transfer Type */
#define QSPI_IFR_TFRTYP(value) ((QSPI_IFR_TFRTYP_Msk & ((value) << QSPI_IFR_TFRTYP_Pos)))
#define   QSPI_IFR_TFRTYP_TRSFR_READ (0x0u << 12) /**< \brief (QSPI_IFR) Read transfer from the serial memory.Scrambling is not performed.Read at random location (fetch) in the serial Flash memory is not possible. */
#define   QSPI_IFR_TFRTYP_TRSFR_READ_MEMORY (0x1u << 12) /**< \brief (QSPI_IFR) Read data transfer from the serial memory.If enabled, scrambling is performed.Read at random location (fetch) in the serial Flash memory is possible. */
#define   QSPI_IFR_TFRTYP_TRSFR_WRITE (0x2u << 12) /**< \brief (QSPI_IFR) Write transfer into the serial memory.Scrambling is not performed. */
#define   QSPI_IFR_TFRTYP_TRSFR_WRITE_MEMORY (0x3u << 12) /**< \brief (QSPI_IFR) Write data transfer into the serial memory.If enabled, scrambling is performed. */
#define QSPI_IFR_CRM (0x1u << 14) /**< \brief (QSPI_IFR) Continuous Read Mode */
#define   QSPI_IFR_CRM_DISABLED (0x0u << 14) /**< \brief (QSPI_IFR) The Continuous Read mode is disabled. */
#define   QSPI_IFR_CRM_ENABLED (0x1u << 14) /**< \brief (QSPI_IFR) The Continuous Read mode is enabled. */
#define QSPI_IFR_NBDUM_Pos 16
#define QSPI_IFR_NBDUM_Msk (0x1fu << QSPI_IFR_NBDUM_Pos) /**< \brief (QSPI_IFR) Number Of Dummy Cycles */
#define QSPI_IFR_NBDUM(value) ((QSPI_IFR_NBDUM_Msk & ((value) << QSPI_IFR_NBDUM_Pos)))
/* -------- QSPI_SMR : (QSPI Offset: 0x40) Scrambling Mode Register -------- */
#define QSPI_SMR_SCREN (0x1u << 0) /**< \brief (QSPI_SMR) Scrambling/Unscrambling Enable */
#define   QSPI_SMR_SCREN_DISABLED (0x0u << 0) /**< \brief (QSPI_SMR) The scrambling/unscrambling is disabled. */
#define   QSPI_SMR_SCREN_ENABLED (0x1u << 0) /**< \brief (QSPI_SMR) The scrambling/unscrambling is enabled. */
#define QSPI_SMR_RVDIS (0x1u << 1) /**< \brief (QSPI_SMR) Scrambling/Unscrambling Random Value Disable */
/* -------- QSPI_SKR : (QSPI Offset: 0x44) Scrambling Key Register -------- */
#define QSPI_SKR_USRK_Pos 0
#define QSPI_SKR_USRK_Msk (0xffffffffu << QSPI_SKR_USRK_Pos) /**< \brief (QSPI_SKR) Scrambling User Key */
#define QSPI_SKR_USRK(value) ((QSPI_SKR_USRK_Msk & ((value) << QSPI_SKR_USRK_Pos)))
/* -------- QSPI_WPMR : (QSPI Offset: 0xE4) Write Protection Mode Register -------- */
#define QSPI_WPMR_WPEN (0x1u << 0) /**< \brief (QSPI_WPMR) Write Protection Enable */
#define QSPI_WPMR_WPKEY_Pos 8
#define QSPI_WPMR_WPKEY_Msk (0xffffffu << QSPI_WPMR_WPKEY_Pos) /**< \brief (QSPI_WPMR) Write Protection Key */
#define QSPI_WPMR_WPKEY(value) ((QSPI_WPMR_WPKEY_Msk & ((value) << QSPI_WPMR_WPKEY_Pos)))
#define   QSPI_WPMR_WPKEY_PASSWD (0x515350u << 8) /**< \brief (QSPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. */
/* -------- QSPI_WPSR : (QSPI Offset: 0xE8) Write Protection Status Register -------- */
#define QSPI_WPSR_WPVS (0x1u << 0) /**< \brief (QSPI_WPSR) Write Protection Violation Status */
#define QSPI_WPSR_WPVSRC_Pos 8
#define QSPI_WPSR_WPVSRC_Msk (0xffu << QSPI_WPSR_WPVSRC_Pos) /**< \brief (QSPI_WPSR) Write Protection Violation Source */
/* -------- QSPI_VERSION : (QSPI Offset: 0x00FC) Version Register -------- */
#define QSPI_VERSION_VERSION_Pos 0
#define QSPI_VERSION_VERSION_Msk (0xfffu << QSPI_VERSION_VERSION_Pos) /**< \brief (QSPI_VERSION) Hardware Module Version */
#define QSPI_VERSION_MFN_Pos 16
#define QSPI_VERSION_MFN_Msk (0x7u << QSPI_VERSION_MFN_Pos) /**< \brief (QSPI_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_QSPI_COMPONENT_ */
