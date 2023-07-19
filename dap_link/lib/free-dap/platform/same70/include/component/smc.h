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

#ifndef _SAME70_SMC_COMPONENT_
#define _SAME70_SMC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Static Memory Controller */
/* ============================================================================= */
/** \addtogroup SAME70_SMC Static Memory Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief SmcCs_number hardware registers */
typedef struct {
  __IO uint32_t SMC_SETUP; /**< \brief (SmcCs_number Offset: 0x0) SMC Setup Register */
  __IO uint32_t SMC_PULSE; /**< \brief (SmcCs_number Offset: 0x4) SMC Pulse Register */
  __IO uint32_t SMC_CYCLE; /**< \brief (SmcCs_number Offset: 0x8) SMC Cycle Register */
  __IO uint32_t SMC_MODE;  /**< \brief (SmcCs_number Offset: 0xC) SMC MODE Register */
} SmcCs_number;
/** \brief Smc hardware registers */
#define SMCCS_NUMBER_NUMBER 4
typedef struct {
       SmcCs_number SMC_CS_NUMBER[SMCCS_NUMBER_NUMBER]; /**< \brief (Smc Offset: 0x0) CS_number = 0 .. 3 */
  __I  uint32_t     Reserved1[16];
  __IO uint32_t     SMC_OCMS;                           /**< \brief (Smc Offset: 0x80) SMC Off-Chip Memory Scrambling Register */
  __O  uint32_t     SMC_KEY1;                           /**< \brief (Smc Offset: 0x84) SMC Off-Chip Memory Scrambling KEY1 Register */
  __O  uint32_t     SMC_KEY2;                           /**< \brief (Smc Offset: 0x88) SMC Off-Chip Memory Scrambling KEY2 Register */
  __I  uint32_t     Reserved2[22];
  __IO uint32_t     SMC_WPMR;                           /**< \brief (Smc Offset: 0xE4) SMC Write Protection Mode Register */
  __I  uint32_t     SMC_WPSR;                           /**< \brief (Smc Offset: 0xE8) SMC Write Protection Status Register */
  __I  uint32_t     Reserved3[4];
  __I  uint32_t     SMC_VERSION;                        /**< \brief (Smc Offset: 0xFC) SMC Version Register */
} Smc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- SMC_SETUP : (SMC Offset: N/A) SMC Setup Register -------- */
#define SMC_SETUP_NWE_SETUP_Pos 0
#define SMC_SETUP_NWE_SETUP_Msk (0x3fu << SMC_SETUP_NWE_SETUP_Pos) /**< \brief (SMC_SETUP) NWE Setup Length */
#define SMC_SETUP_NWE_SETUP(value) ((SMC_SETUP_NWE_SETUP_Msk & ((value) << SMC_SETUP_NWE_SETUP_Pos)))
#define SMC_SETUP_NCS_WR_SETUP_Pos 8
#define SMC_SETUP_NCS_WR_SETUP_Msk (0x3fu << SMC_SETUP_NCS_WR_SETUP_Pos) /**< \brief (SMC_SETUP) NCS Setup Length in WRITE Access */
#define SMC_SETUP_NCS_WR_SETUP(value) ((SMC_SETUP_NCS_WR_SETUP_Msk & ((value) << SMC_SETUP_NCS_WR_SETUP_Pos)))
#define SMC_SETUP_NRD_SETUP_Pos 16
#define SMC_SETUP_NRD_SETUP_Msk (0x3fu << SMC_SETUP_NRD_SETUP_Pos) /**< \brief (SMC_SETUP) NRD Setup Length */
#define SMC_SETUP_NRD_SETUP(value) ((SMC_SETUP_NRD_SETUP_Msk & ((value) << SMC_SETUP_NRD_SETUP_Pos)))
#define SMC_SETUP_NCS_RD_SETUP_Pos 24
#define SMC_SETUP_NCS_RD_SETUP_Msk (0x3fu << SMC_SETUP_NCS_RD_SETUP_Pos) /**< \brief (SMC_SETUP) NCS Setup Length in READ Access */
#define SMC_SETUP_NCS_RD_SETUP(value) ((SMC_SETUP_NCS_RD_SETUP_Msk & ((value) << SMC_SETUP_NCS_RD_SETUP_Pos)))
/* -------- SMC_PULSE : (SMC Offset: N/A) SMC Pulse Register -------- */
#define SMC_PULSE_NWE_PULSE_Pos 0
#define SMC_PULSE_NWE_PULSE_Msk (0x7fu << SMC_PULSE_NWE_PULSE_Pos) /**< \brief (SMC_PULSE) NWE Pulse Length */
#define SMC_PULSE_NWE_PULSE(value) ((SMC_PULSE_NWE_PULSE_Msk & ((value) << SMC_PULSE_NWE_PULSE_Pos)))
#define SMC_PULSE_NCS_WR_PULSE_Pos 8
#define SMC_PULSE_NCS_WR_PULSE_Msk (0x7fu << SMC_PULSE_NCS_WR_PULSE_Pos) /**< \brief (SMC_PULSE) NCS Pulse Length in WRITE Access */
#define SMC_PULSE_NCS_WR_PULSE(value) ((SMC_PULSE_NCS_WR_PULSE_Msk & ((value) << SMC_PULSE_NCS_WR_PULSE_Pos)))
#define SMC_PULSE_NRD_PULSE_Pos 16
#define SMC_PULSE_NRD_PULSE_Msk (0x7fu << SMC_PULSE_NRD_PULSE_Pos) /**< \brief (SMC_PULSE) NRD Pulse Length */
#define SMC_PULSE_NRD_PULSE(value) ((SMC_PULSE_NRD_PULSE_Msk & ((value) << SMC_PULSE_NRD_PULSE_Pos)))
#define SMC_PULSE_NCS_RD_PULSE_Pos 24
#define SMC_PULSE_NCS_RD_PULSE_Msk (0x7fu << SMC_PULSE_NCS_RD_PULSE_Pos) /**< \brief (SMC_PULSE) NCS Pulse Length in READ Access */
#define SMC_PULSE_NCS_RD_PULSE(value) ((SMC_PULSE_NCS_RD_PULSE_Msk & ((value) << SMC_PULSE_NCS_RD_PULSE_Pos)))
/* -------- SMC_CYCLE : (SMC Offset: N/A) SMC Cycle Register -------- */
#define SMC_CYCLE_NWE_CYCLE_Pos 0
#define SMC_CYCLE_NWE_CYCLE_Msk (0x1ffu << SMC_CYCLE_NWE_CYCLE_Pos) /**< \brief (SMC_CYCLE) Total Write Cycle Length */
#define SMC_CYCLE_NWE_CYCLE(value) ((SMC_CYCLE_NWE_CYCLE_Msk & ((value) << SMC_CYCLE_NWE_CYCLE_Pos)))
#define SMC_CYCLE_NRD_CYCLE_Pos 16
#define SMC_CYCLE_NRD_CYCLE_Msk (0x1ffu << SMC_CYCLE_NRD_CYCLE_Pos) /**< \brief (SMC_CYCLE) Total Read Cycle Length */
#define SMC_CYCLE_NRD_CYCLE(value) ((SMC_CYCLE_NRD_CYCLE_Msk & ((value) << SMC_CYCLE_NRD_CYCLE_Pos)))
/* -------- SMC_MODE : (SMC Offset: N/A) SMC MODE Register -------- */
#define SMC_MODE_READ_MODE (0x1u << 0) /**< \brief (SMC_MODE) Read Mode */
#define SMC_MODE_WRITE_MODE (0x1u << 1) /**< \brief (SMC_MODE) Write Mode */
#define SMC_MODE_EXNW_MODE_Pos 4
#define SMC_MODE_EXNW_MODE_Msk (0x3u << SMC_MODE_EXNW_MODE_Pos) /**< \brief (SMC_MODE) NWAIT Mode */
#define SMC_MODE_EXNW_MODE(value) ((SMC_MODE_EXNW_MODE_Msk & ((value) << SMC_MODE_EXNW_MODE_Pos)))
#define   SMC_MODE_EXNW_MODE_DISABLED (0x0u << 4) /**< \brief (SMC_MODE) Disabled-The NWAIT input signal is ignored on the corresponding chip select. */
#define   SMC_MODE_EXNW_MODE_FROZEN (0x2u << 4) /**< \brief (SMC_MODE) Frozen Mode-If asserted, the NWAIT signal freezes the current read or write cycle. After deassertion, the read/write cycle is resumed from the point where it was stopped. */
#define   SMC_MODE_EXNW_MODE_READY (0x3u << 4) /**< \brief (SMC_MODE) Ready Mode-The NWAIT signal indicates the availability of the external device at the end of the pulse of the controlling read or write signal, to complete the access. If high, the access normally completes. If low, the access is extended until NWAIT returns high. */
#define SMC_MODE_BAT (0x1u << 8) /**< \brief (SMC_MODE) Byte Access Type */
#define   SMC_MODE_BAT_BYTE_SELECT (0x0u << 8) /**< \brief (SMC_MODE) Byte select access type:- Write operation is controlled using NCS, NWE, NBS0, NBS1.- Read operation is controlled using NCS, NRD, NBS0, NBS1. */
#define   SMC_MODE_BAT_BYTE_WRITE (0x1u << 8) /**< \brief (SMC_MODE) Byte write access type:- Write operation is controlled using NCS, NWR0, NWR1.- Read operation is controlled using NCS and NRD. */
#define SMC_MODE_DBW (0x1u << 12) /**< \brief (SMC_MODE) Data Bus Width */
#define   SMC_MODE_DBW_8_BIT (0x0u << 12) /**< \brief (SMC_MODE) 8-bit Data Bus */
#define   SMC_MODE_DBW_16_BIT (0x1u << 12) /**< \brief (SMC_MODE) 16-bit Data Bus */
#define SMC_MODE_TDF_CYCLES_Pos 16
#define SMC_MODE_TDF_CYCLES_Msk (0xfu << SMC_MODE_TDF_CYCLES_Pos) /**< \brief (SMC_MODE) Data Float Time */
#define SMC_MODE_TDF_CYCLES(value) ((SMC_MODE_TDF_CYCLES_Msk & ((value) << SMC_MODE_TDF_CYCLES_Pos)))
#define SMC_MODE_TDF_MODE (0x1u << 20) /**< \brief (SMC_MODE) TDF Optimization */
#define SMC_MODE_PMEN (0x1u << 24) /**< \brief (SMC_MODE) Page Mode Enabled */
#define SMC_MODE_PS_Pos 28
#define SMC_MODE_PS_Msk (0x3u << SMC_MODE_PS_Pos) /**< \brief (SMC_MODE) Page Size */
#define SMC_MODE_PS(value) ((SMC_MODE_PS_Msk & ((value) << SMC_MODE_PS_Pos)))
#define   SMC_MODE_PS_4_BYTE (0x0u << 28) /**< \brief (SMC_MODE) 4-byte page */
#define   SMC_MODE_PS_8_BYTE (0x1u << 28) /**< \brief (SMC_MODE) 8-byte page */
#define   SMC_MODE_PS_16_BYTE (0x2u << 28) /**< \brief (SMC_MODE) 16-byte page */
#define   SMC_MODE_PS_32_BYTE (0x3u << 28) /**< \brief (SMC_MODE) 32-byte page */
/* -------- SMC_OCMS : (SMC Offset: 0x80) SMC Off-Chip Memory Scrambling Register -------- */
#define SMC_OCMS_SMSE (0x1u << 0) /**< \brief (SMC_OCMS) Static Memory Controller Scrambling Enable */
#define SMC_OCMS_CS0SE (0x1u << 8) /**< \brief (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable */
#define SMC_OCMS_CS1SE (0x1u << 9) /**< \brief (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable */
#define SMC_OCMS_CS2SE (0x1u << 10) /**< \brief (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable */
#define SMC_OCMS_CS3SE (0x1u << 11) /**< \brief (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable */
/* -------- SMC_KEY1 : (SMC Offset: 0x84) SMC Off-Chip Memory Scrambling KEY1 Register -------- */
#define SMC_KEY1_KEY1_Pos 0
#define SMC_KEY1_KEY1_Msk (0xffffffffu << SMC_KEY1_KEY1_Pos) /**< \brief (SMC_KEY1) Off-Chip Memory Scrambling (OCMS) Key Part 1 */
#define SMC_KEY1_KEY1(value) ((SMC_KEY1_KEY1_Msk & ((value) << SMC_KEY1_KEY1_Pos)))
/* -------- SMC_KEY2 : (SMC Offset: 0x88) SMC Off-Chip Memory Scrambling KEY2 Register -------- */
#define SMC_KEY2_KEY2_Pos 0
#define SMC_KEY2_KEY2_Msk (0xffffffffu << SMC_KEY2_KEY2_Pos) /**< \brief (SMC_KEY2) Off-Chip Memory Scrambling (OCMS) Key Part 2 */
#define SMC_KEY2_KEY2(value) ((SMC_KEY2_KEY2_Msk & ((value) << SMC_KEY2_KEY2_Pos)))
/* -------- SMC_WPMR : (SMC Offset: 0xE4) SMC Write Protection Mode Register -------- */
#define SMC_WPMR_WPEN (0x1u << 0) /**< \brief (SMC_WPMR) Write Protect Enable */
#define SMC_WPMR_WPKEY_Pos 8
#define SMC_WPMR_WPKEY_Msk (0xffffffu << SMC_WPMR_WPKEY_Pos) /**< \brief (SMC_WPMR) Write Protection Key */
#define SMC_WPMR_WPKEY(value) ((SMC_WPMR_WPKEY_Msk & ((value) << SMC_WPMR_WPKEY_Pos)))
#define   SMC_WPMR_WPKEY_PASSWD (0x534D43u << 8) /**< \brief (SMC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. */
/* -------- SMC_WPSR : (SMC Offset: 0xE8) SMC Write Protection Status Register -------- */
#define SMC_WPSR_WPVS (0x1u << 0) /**< \brief (SMC_WPSR) Write Protection Violation Status */
#define SMC_WPSR_WPVSRC_Pos 8
#define SMC_WPSR_WPVSRC_Msk (0xffffu << SMC_WPSR_WPVSRC_Pos) /**< \brief (SMC_WPSR) Write Protection Violation Source */
/* -------- SMC_VERSION : (SMC Offset: 0xFC) SMC Version Register -------- */
#define SMC_VERSION_VERSION_Pos 0
#define SMC_VERSION_VERSION_Msk (0xfffu << SMC_VERSION_VERSION_Pos) /**< \brief (SMC_VERSION) Hardware Module Version */
#define SMC_VERSION_MFN_Pos 16
#define SMC_VERSION_MFN_Msk (0x7u << SMC_VERSION_MFN_Pos) /**< \brief (SMC_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_SMC_COMPONENT_ */
