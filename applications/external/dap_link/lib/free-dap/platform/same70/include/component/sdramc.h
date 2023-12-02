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

#ifndef _SAME70_SDRAMC_COMPONENT_
#define _SAME70_SDRAMC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR SDRAM Controller */
/* ============================================================================= */
/** \addtogroup SAME70_SDRAMC SDRAM Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Sdramc hardware registers */
typedef struct {
  __IO uint32_t SDRAMC_MR;        /**< \brief (Sdramc Offset: 0x00) SDRAMC Mode Register */
  __IO uint32_t SDRAMC_TR;        /**< \brief (Sdramc Offset: 0x04) SDRAMC Refresh Timer Register */
  __IO uint32_t SDRAMC_CR;        /**< \brief (Sdramc Offset: 0x08) SDRAMC Configuration Register */
  __I  uint32_t Reserved1[1];
  __IO uint32_t SDRAMC_LPR;       /**< \brief (Sdramc Offset: 0x10) SDRAMC Low Power Register */
  __O  uint32_t SDRAMC_IER;       /**< \brief (Sdramc Offset: 0x14) SDRAMC Interrupt Enable Register */
  __O  uint32_t SDRAMC_IDR;       /**< \brief (Sdramc Offset: 0x18) SDRAMC Interrupt Disable Register */
  __I  uint32_t SDRAMC_IMR;       /**< \brief (Sdramc Offset: 0x1C) SDRAMC Interrupt Mask Register */
  __I  uint32_t SDRAMC_ISR;       /**< \brief (Sdramc Offset: 0x20) SDRAMC Interrupt Status Register */
  __IO uint32_t SDRAMC_MDR;       /**< \brief (Sdramc Offset: 0x24) SDRAMC Memory Device Register */
  __IO uint32_t SDRAMC_CFR1;      /**< \brief (Sdramc Offset: 0x28) SDRAMC Configuration Register 1 */
  __IO uint32_t SDRAMC_OCMS;      /**< \brief (Sdramc Offset: 0x2C) SDRAMC OCMS Register */
  __O  uint32_t SDRAMC_OCMS_KEY1; /**< \brief (Sdramc Offset: 0x30) SDRAMC OCMS KEY1 Register */
  __O  uint32_t SDRAMC_OCMS_KEY2; /**< \brief (Sdramc Offset: 0x34) SDRAMC OCMS KEY2 Register */
  __I  uint32_t Reserved2[49];
  __I  uint32_t SDRAMC_VERSION;   /**< \brief (Sdramc Offset: 0xFC) SDRAMC Version Register */
} Sdramc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- SDRAMC_MR : (SDRAMC Offset: 0x00) SDRAMC Mode Register -------- */
#define SDRAMC_MR_MODE_Pos 0
#define SDRAMC_MR_MODE_Msk (0x7u << SDRAMC_MR_MODE_Pos) /**< \brief (SDRAMC_MR) SDRAMC Command Mode */
#define SDRAMC_MR_MODE(value) ((SDRAMC_MR_MODE_Msk & ((value) << SDRAMC_MR_MODE_Pos)))
#define   SDRAMC_MR_MODE_NORMAL (0x0u << 0) /**< \brief (SDRAMC_MR) Normal mode. Any access to the SDRAM is decoded normally. To activate this mode, command must be followed by a write to the SDRAM. */
#define   SDRAMC_MR_MODE_NOP (0x1u << 0) /**< \brief (SDRAMC_MR) The SDRAMC issues a NOP command when the SDRAM device is accessed regardless of the cycle. To activate this mode, command must be followed by a write to the SDRAM. */
#define   SDRAMC_MR_MODE_ALLBANKS_PRECHARGE (0x2u << 0) /**< \brief (SDRAMC_MR) The SDRAMC issues an "All Banks Precharge" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, command must be followed by a write to the SDRAM. */
#define   SDRAMC_MR_MODE_LOAD_MODEREG (0x3u << 0) /**< \brief (SDRAMC_MR) The SDRAMC issues a "Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, command must be followed by a write to the SDRAM. */
#define   SDRAMC_MR_MODE_AUTO_REFRESH (0x4u << 0) /**< \brief (SDRAMC_MR) The SDRAMC issues an "Auto-Refresh" Command when the SDRAM device is accessed regardless of the cycle. Previously, an "All Banks Precharge" command must be issued. To activate this mode, command must be followed by a write to the SDRAM. */
#define   SDRAMC_MR_MODE_EXT_LOAD_MODEREG (0x5u << 0) /**< \brief (SDRAMC_MR) The SDRAMC issues an "Extended Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the "Extended Load Mode Register" command must be followed by a write to the SDRAM. The write in the SDRAM must be done in the appropriate bank; most low-power SDRAM devices use the bank 1. */
#define   SDRAMC_MR_MODE_DEEP_POWERDOWN (0x6u << 0) /**< \brief (SDRAMC_MR) Deep power-down mode. Enters deep power-down mode. */
/* -------- SDRAMC_TR : (SDRAMC Offset: 0x04) SDRAMC Refresh Timer Register -------- */
#define SDRAMC_TR_COUNT_Pos 0
#define SDRAMC_TR_COUNT_Msk (0xfffu << SDRAMC_TR_COUNT_Pos) /**< \brief (SDRAMC_TR) SDRAMC Refresh Timer Count */
#define SDRAMC_TR_COUNT(value) ((SDRAMC_TR_COUNT_Msk & ((value) << SDRAMC_TR_COUNT_Pos)))
/* -------- SDRAMC_CR : (SDRAMC Offset: 0x08) SDRAMC Configuration Register -------- */
#define SDRAMC_CR_NC_Pos 0
#define SDRAMC_CR_NC_Msk (0x3u << SDRAMC_CR_NC_Pos) /**< \brief (SDRAMC_CR) Number of Column Bits */
#define SDRAMC_CR_NC(value) ((SDRAMC_CR_NC_Msk & ((value) << SDRAMC_CR_NC_Pos)))
#define   SDRAMC_CR_NC_COL8 (0x0u << 0) /**< \brief (SDRAMC_CR) 8 column bits */
#define   SDRAMC_CR_NC_COL9 (0x1u << 0) /**< \brief (SDRAMC_CR) 9 column bits */
#define   SDRAMC_CR_NC_COL10 (0x2u << 0) /**< \brief (SDRAMC_CR) 10 column bits */
#define   SDRAMC_CR_NC_COL11 (0x3u << 0) /**< \brief (SDRAMC_CR) 11 column bits */
#define SDRAMC_CR_NR_Pos 2
#define SDRAMC_CR_NR_Msk (0x3u << SDRAMC_CR_NR_Pos) /**< \brief (SDRAMC_CR) Number of Row Bits */
#define SDRAMC_CR_NR(value) ((SDRAMC_CR_NR_Msk & ((value) << SDRAMC_CR_NR_Pos)))
#define   SDRAMC_CR_NR_ROW11 (0x0u << 2) /**< \brief (SDRAMC_CR) 11 row bits */
#define   SDRAMC_CR_NR_ROW12 (0x1u << 2) /**< \brief (SDRAMC_CR) 12 row bits */
#define   SDRAMC_CR_NR_ROW13 (0x2u << 2) /**< \brief (SDRAMC_CR) 13 row bits */
#define SDRAMC_CR_NB (0x1u << 4) /**< \brief (SDRAMC_CR) Number of Banks */
#define   SDRAMC_CR_NB_BANK2 (0x0u << 4) /**< \brief (SDRAMC_CR) 2 banks */
#define   SDRAMC_CR_NB_BANK4 (0x1u << 4) /**< \brief (SDRAMC_CR) 4 banks */
#define SDRAMC_CR_CAS_Pos 5
#define SDRAMC_CR_CAS_Msk (0x3u << SDRAMC_CR_CAS_Pos) /**< \brief (SDRAMC_CR) CAS Latency */
#define SDRAMC_CR_CAS(value) ((SDRAMC_CR_CAS_Msk & ((value) << SDRAMC_CR_CAS_Pos)))
#define   SDRAMC_CR_CAS_LATENCY1 (0x1u << 5) /**< \brief (SDRAMC_CR) 1 cycle CAS latency */
#define   SDRAMC_CR_CAS_LATENCY2 (0x2u << 5) /**< \brief (SDRAMC_CR) 2 cycle CAS latency */
#define   SDRAMC_CR_CAS_LATENCY3 (0x3u << 5) /**< \brief (SDRAMC_CR) 3 cycle CAS latency */
#define SDRAMC_CR_DBW (0x1u << 7) /**< \brief (SDRAMC_CR) Data Bus Width */
#define SDRAMC_CR_TWR_Pos 8
#define SDRAMC_CR_TWR_Msk (0xfu << SDRAMC_CR_TWR_Pos) /**< \brief (SDRAMC_CR) Write Recovery Delay */
#define SDRAMC_CR_TWR(value) ((SDRAMC_CR_TWR_Msk & ((value) << SDRAMC_CR_TWR_Pos)))
#define SDRAMC_CR_TRC_TRFC_Pos 12
#define SDRAMC_CR_TRC_TRFC_Msk (0xfu << SDRAMC_CR_TRC_TRFC_Pos) /**< \brief (SDRAMC_CR) Row Cycle Delay and Row Refresh Cycle */
#define SDRAMC_CR_TRC_TRFC(value) ((SDRAMC_CR_TRC_TRFC_Msk & ((value) << SDRAMC_CR_TRC_TRFC_Pos)))
#define SDRAMC_CR_TRP_Pos 16
#define SDRAMC_CR_TRP_Msk (0xfu << SDRAMC_CR_TRP_Pos) /**< \brief (SDRAMC_CR) Row Precharge Delay */
#define SDRAMC_CR_TRP(value) ((SDRAMC_CR_TRP_Msk & ((value) << SDRAMC_CR_TRP_Pos)))
#define SDRAMC_CR_TRCD_Pos 20
#define SDRAMC_CR_TRCD_Msk (0xfu << SDRAMC_CR_TRCD_Pos) /**< \brief (SDRAMC_CR) Row to Column Delay */
#define SDRAMC_CR_TRCD(value) ((SDRAMC_CR_TRCD_Msk & ((value) << SDRAMC_CR_TRCD_Pos)))
#define SDRAMC_CR_TRAS_Pos 24
#define SDRAMC_CR_TRAS_Msk (0xfu << SDRAMC_CR_TRAS_Pos) /**< \brief (SDRAMC_CR) Active to Precharge Delay */
#define SDRAMC_CR_TRAS(value) ((SDRAMC_CR_TRAS_Msk & ((value) << SDRAMC_CR_TRAS_Pos)))
#define SDRAMC_CR_TXSR_Pos 28
#define SDRAMC_CR_TXSR_Msk (0xfu << SDRAMC_CR_TXSR_Pos) /**< \brief (SDRAMC_CR) Exit Self Refresh to Active Delay */
#define SDRAMC_CR_TXSR(value) ((SDRAMC_CR_TXSR_Msk & ((value) << SDRAMC_CR_TXSR_Pos)))
/* -------- SDRAMC_LPR : (SDRAMC Offset: 0x10) SDRAMC Low Power Register -------- */
#define SDRAMC_LPR_LPCB_Pos 0
#define SDRAMC_LPR_LPCB_Msk (0x3u << SDRAMC_LPR_LPCB_Pos) /**< \brief (SDRAMC_LPR) Low-power Configuration Bits */
#define SDRAMC_LPR_LPCB(value) ((SDRAMC_LPR_LPCB_Msk & ((value) << SDRAMC_LPR_LPCB_Pos)))
#define   SDRAMC_LPR_LPCB_DISABLED (0x0u << 0) /**< \brief (SDRAMC_LPR) Low Power Feature is inhibited: no Power-down, Self-refresh or Deep Power-down command is issued to the SDRAM device. */
#define   SDRAMC_LPR_LPCB_SELF_REFRESH (0x1u << 0) /**< \brief (SDRAMC_LPR) The SDRAMC issues a Self-refresh command to the SDRAM device, the SDCK clock is deactivated and the SDCKE signal is set low. The SDRAM device leaves the Self Refresh Mode when accessed and enters it after the access. */
#define   SDRAMC_LPR_LPCB_POWER_DOWN (0x2u << 0) /**< \brief (SDRAMC_LPR) The SDRAMC issues a Power-down Command to the SDRAM device after each access, the SDCKE signal is set to low. The SDRAM device leaves the Power-down Mode when accessed and enters it after the access. */
#define   SDRAMC_LPR_LPCB_DEEP_POWER_DOWN (0x3u << 0) /**< \brief (SDRAMC_LPR) The SDRAMC issues a Deep Power-down command to the SDRAM device. This mode is unique to low-power SDRAM. */
#define SDRAMC_LPR_PASR_Pos 4
#define SDRAMC_LPR_PASR_Msk (0x7u << SDRAMC_LPR_PASR_Pos) /**< \brief (SDRAMC_LPR) Partial Array Self-refresh (only for low-power SDRAM) */
#define SDRAMC_LPR_PASR(value) ((SDRAMC_LPR_PASR_Msk & ((value) << SDRAMC_LPR_PASR_Pos)))
#define SDRAMC_LPR_TCSR_Pos 8
#define SDRAMC_LPR_TCSR_Msk (0x3u << SDRAMC_LPR_TCSR_Pos) /**< \brief (SDRAMC_LPR) Temperature Compensated Self-Refresh (only for low-power SDRAM) */
#define SDRAMC_LPR_TCSR(value) ((SDRAMC_LPR_TCSR_Msk & ((value) << SDRAMC_LPR_TCSR_Pos)))
#define SDRAMC_LPR_DS_Pos 10
#define SDRAMC_LPR_DS_Msk (0x3u << SDRAMC_LPR_DS_Pos) /**< \brief (SDRAMC_LPR) Drive Strength (only for low-power SDRAM) */
#define SDRAMC_LPR_DS(value) ((SDRAMC_LPR_DS_Msk & ((value) << SDRAMC_LPR_DS_Pos)))
#define SDRAMC_LPR_TIMEOUT_Pos 12
#define SDRAMC_LPR_TIMEOUT_Msk (0x3u << SDRAMC_LPR_TIMEOUT_Pos) /**< \brief (SDRAMC_LPR) Time to Define When Low-power Mode Is Enabled */
#define SDRAMC_LPR_TIMEOUT(value) ((SDRAMC_LPR_TIMEOUT_Msk & ((value) << SDRAMC_LPR_TIMEOUT_Pos)))
#define   SDRAMC_LPR_TIMEOUT_LP_LAST_XFER (0x0u << 12) /**< \brief (SDRAMC_LPR) The SDRAMC activates the SDRAM low-power mode immediately after the end of the last transfer. */
#define   SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_64 (0x1u << 12) /**< \brief (SDRAMC_LPR) The SDRAMC activates the SDRAM low-power mode 64 clock cycles after the end of the last transfer. */
#define   SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_128 (0x2u << 12) /**< \brief (SDRAMC_LPR) The SDRAMC activates the SDRAM low-power mode 128 clock cycles after the end of the last transfer. */
/* -------- SDRAMC_IER : (SDRAMC Offset: 0x14) SDRAMC Interrupt Enable Register -------- */
#define SDRAMC_IER_RES (0x1u << 0) /**< \brief (SDRAMC_IER) Refresh Error Status */
/* -------- SDRAMC_IDR : (SDRAMC Offset: 0x18) SDRAMC Interrupt Disable Register -------- */
#define SDRAMC_IDR_RES (0x1u << 0) /**< \brief (SDRAMC_IDR) Refresh Error Status */
/* -------- SDRAMC_IMR : (SDRAMC Offset: 0x1C) SDRAMC Interrupt Mask Register -------- */
#define SDRAMC_IMR_RES (0x1u << 0) /**< \brief (SDRAMC_IMR) Refresh Error Status */
/* -------- SDRAMC_ISR : (SDRAMC Offset: 0x20) SDRAMC Interrupt Status Register -------- */
#define SDRAMC_ISR_RES (0x1u << 0) /**< \brief (SDRAMC_ISR) Refresh Error Status (cleared on read) */
/* -------- SDRAMC_MDR : (SDRAMC Offset: 0x24) SDRAMC Memory Device Register -------- */
#define SDRAMC_MDR_MD_Pos 0
#define SDRAMC_MDR_MD_Msk (0x3u << SDRAMC_MDR_MD_Pos) /**< \brief (SDRAMC_MDR) Memory Device Type */
#define SDRAMC_MDR_MD(value) ((SDRAMC_MDR_MD_Msk & ((value) << SDRAMC_MDR_MD_Pos)))
#define   SDRAMC_MDR_MD_SDRAM (0x0u << 0) /**< \brief (SDRAMC_MDR) SDRAM */
#define   SDRAMC_MDR_MD_LPSDRAM (0x1u << 0) /**< \brief (SDRAMC_MDR) Low-power SDRAM */
/* -------- SDRAMC_CFR1 : (SDRAMC Offset: 0x28) SDRAMC Configuration Register 1 -------- */
#define SDRAMC_CFR1_TMRD_Pos 0
#define SDRAMC_CFR1_TMRD_Msk (0xfu << SDRAMC_CFR1_TMRD_Pos) /**< \brief (SDRAMC_CFR1) Load Mode Register Command to Active or Refresh Command */
#define SDRAMC_CFR1_TMRD(value) ((SDRAMC_CFR1_TMRD_Msk & ((value) << SDRAMC_CFR1_TMRD_Pos)))
#define SDRAMC_CFR1_UNAL (0x1u << 8) /**< \brief (SDRAMC_CFR1) Support Unaligned Access */
#define   SDRAMC_CFR1_UNAL_UNSUPPORTED (0x0u << 8) /**< \brief (SDRAMC_CFR1) Unaligned access is not supported. */
#define   SDRAMC_CFR1_UNAL_SUPPORTED (0x1u << 8) /**< \brief (SDRAMC_CFR1) Unaligned access is supported. */
/* -------- SDRAMC_OCMS : (SDRAMC Offset: 0x2C) SDRAMC OCMS Register -------- */
#define SDRAMC_OCMS_SDR_SE (0x1u << 0) /**< \brief (SDRAMC_OCMS) SDRAM Memory Controller Scrambling Enable */
/* -------- SDRAMC_OCMS_KEY1 : (SDRAMC Offset: 0x30) SDRAMC OCMS KEY1 Register -------- */
#define SDRAMC_OCMS_KEY1_KEY1_Pos 0
#define SDRAMC_OCMS_KEY1_KEY1_Msk (0xffffffffu << SDRAMC_OCMS_KEY1_KEY1_Pos) /**< \brief (SDRAMC_OCMS_KEY1) Off-chip Memory Scrambling (OCMS) Key Part 1 */
#define SDRAMC_OCMS_KEY1_KEY1(value) ((SDRAMC_OCMS_KEY1_KEY1_Msk & ((value) << SDRAMC_OCMS_KEY1_KEY1_Pos)))
/* -------- SDRAMC_OCMS_KEY2 : (SDRAMC Offset: 0x34) SDRAMC OCMS KEY2 Register -------- */
#define SDRAMC_OCMS_KEY2_KEY2_Pos 0
#define SDRAMC_OCMS_KEY2_KEY2_Msk (0xffffffffu << SDRAMC_OCMS_KEY2_KEY2_Pos) /**< \brief (SDRAMC_OCMS_KEY2) Off-chip Memory Scrambling (OCMS) Key Part 2 */
#define SDRAMC_OCMS_KEY2_KEY2(value) ((SDRAMC_OCMS_KEY2_KEY2_Msk & ((value) << SDRAMC_OCMS_KEY2_KEY2_Pos)))
/* -------- SDRAMC_VERSION : (SDRAMC Offset: 0xFC) SDRAMC Version Register -------- */
#define SDRAMC_VERSION_VERSION_Pos 0
#define SDRAMC_VERSION_VERSION_Msk (0xfffu << SDRAMC_VERSION_VERSION_Pos) /**< \brief (SDRAMC_VERSION) Version of the Hardware Module */
#define SDRAMC_VERSION_MFN_Pos 16
#define SDRAMC_VERSION_MFN_Msk (0x7u << SDRAMC_VERSION_MFN_Pos) /**< \brief (SDRAMC_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_SDRAMC_COMPONENT_ */
