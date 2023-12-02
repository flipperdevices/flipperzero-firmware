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

#ifndef _SAME70_PMC_COMPONENT_
#define _SAME70_PMC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Power Management Controller */
/* ============================================================================= */
/** \addtogroup SAME70_PMC Power Management Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Pmc hardware registers */
typedef struct {
  __O  uint32_t PMC_SCER;       /**< \brief (Pmc Offset: 0x0000) System Clock Enable Register */
  __O  uint32_t PMC_SCDR;       /**< \brief (Pmc Offset: 0x0004) System Clock Disable Register */
  __I  uint32_t PMC_SCSR;       /**< \brief (Pmc Offset: 0x0008) System Clock Status Register */
  __I  uint32_t Reserved1[1];
  __O  uint32_t PMC_PCER0;      /**< \brief (Pmc Offset: 0x0010) Peripheral Clock Enable Register 0 */
  __O  uint32_t PMC_PCDR0;      /**< \brief (Pmc Offset: 0x0014) Peripheral Clock Disable Register 0 */
  __I  uint32_t PMC_PCSR0;      /**< \brief (Pmc Offset: 0x0018) Peripheral Clock Status Register 0 */
  __IO uint32_t CKGR_UCKR;      /**< \brief (Pmc Offset: 0x001C) UTMI Clock Register */
  __IO uint32_t CKGR_MOR;       /**< \brief (Pmc Offset: 0x0020) Main Oscillator Register */
  __IO uint32_t CKGR_MCFR;      /**< \brief (Pmc Offset: 0x0024) Main Clock Frequency Register */
  __IO uint32_t CKGR_PLLAR;     /**< \brief (Pmc Offset: 0x0028) PLLA Register */
  __I  uint32_t Reserved2[1];
  __IO uint32_t PMC_MCKR;       /**< \brief (Pmc Offset: 0x0030) Master Clock Register */
  __I  uint32_t Reserved3[1];
  __IO uint32_t PMC_USB;        /**< \brief (Pmc Offset: 0x0038) USB Clock Register */
  __I  uint32_t Reserved4[1];
  __IO uint32_t PMC_PCK[8];     /**< \brief (Pmc Offset: 0x40) Programmable Clock Register (chid = 0) */
  __O  uint32_t PMC_IER;        /**< \brief (Pmc Offset: 0x0060) Interrupt Enable Register */
  __O  uint32_t PMC_IDR;        /**< \brief (Pmc Offset: 0x0064) Interrupt Disable Register */
  __I  uint32_t PMC_SR;         /**< \brief (Pmc Offset: 0x0068) Status Register */
  __I  uint32_t PMC_IMR;        /**< \brief (Pmc Offset: 0x006C) Interrupt Mask Register */
  __IO uint32_t PMC_FSMR;       /**< \brief (Pmc Offset: 0x0070) Fast Startup Mode Register */
  __IO uint32_t PMC_FSPR;       /**< \brief (Pmc Offset: 0x0074) Fast Startup Polarity Register */
  __O  uint32_t PMC_FOCR;       /**< \brief (Pmc Offset: 0x0078) Fault Output Clear Register */
  __I  uint32_t Reserved5[26];
  __IO uint32_t PMC_WPMR;       /**< \brief (Pmc Offset: 0x00E4) Write Protection Mode Register */
  __I  uint32_t PMC_WPSR;       /**< \brief (Pmc Offset: 0x00E8) Write Protection Status Register */
  __I  uint32_t Reserved6[4];
  __I  uint32_t PMC_VERSION;    /**< \brief (Pmc Offset: 0x00FC) Version Register */
  __O  uint32_t PMC_PCER1;      /**< \brief (Pmc Offset: 0x0100) Peripheral Clock Enable Register 1 */
  __O  uint32_t PMC_PCDR1;      /**< \brief (Pmc Offset: 0x0104) Peripheral Clock Disable Register 1 */
  __I  uint32_t PMC_PCSR1;      /**< \brief (Pmc Offset: 0x0108) Peripheral Clock Status Register 1 */
  __IO uint32_t PMC_PCR;        /**< \brief (Pmc Offset: 0x010C) Peripheral Control Register */
  __IO uint32_t PMC_OCR;        /**< \brief (Pmc Offset: 0x0110) Oscillator Calibration Register */
  __O  uint32_t PMC_SLPWK_ER0;  /**< \brief (Pmc Offset: 0x0114) SleepWalking Enable Register 0 */
  __O  uint32_t PMC_SLPWK_DR0;  /**< \brief (Pmc Offset: 0x0118) SleepWalking Disable Register 0 */
  __I  uint32_t PMC_SLPWK_SR0;  /**< \brief (Pmc Offset: 0x011C) SleepWalking Status Register 0 */
  __I  uint32_t PMC_SLPWK_ASR0; /**< \brief (Pmc Offset: 0x0120) SleepWalking Activity Status Register 0 */
  __I  uint32_t Reserved7[3];
  __IO uint32_t PMC_PMMR;       /**< \brief (Pmc Offset: 0x0130) PLL Maximum Multiplier Value Register */
  __O  uint32_t PMC_SLPWK_ER1;  /**< \brief (Pmc Offset: 0x0134) SleepWalking Enable Register 1 */
  __O  uint32_t PMC_SLPWK_DR1;  /**< \brief (Pmc Offset: 0x0138) SleepWalking Disable Register 1 */
  __I  uint32_t PMC_SLPWK_SR1;  /**< \brief (Pmc Offset: 0x013C) SleepWalking Status Register 1 */
  __I  uint32_t PMC_SLPWK_ASR1; /**< \brief (Pmc Offset: 0x0140) SleepWalking Activity Status Register 1 */
  __I  uint32_t PMC_SLPWK_AIPR; /**< \brief (Pmc Offset: 0x0144) SleepWalking Activity In Progress Register */
  __I  uint32_t Reserved8[4];
  __IO uint32_t PMC_APLLACR;    /**< \brief (Pmc Offset: 0x0158) Audio PLL Analog Configuration Register */
  __IO uint32_t PMC_WMST;       /**< \brief (Pmc Offset: 0x015C) Wait Mode Startup Time Register */
} Pmc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- PMC_SCER : (PMC Offset: 0x0000) System Clock Enable Register -------- */
#define PMC_SCER_USBCLK (0x1u << 5) /**< \brief (PMC_SCER) Enable USB FS Clock */
#define PMC_SCER_PCK0 (0x1u << 8) /**< \brief (PMC_SCER) Programmable Clock 0 Output Enable */
#define PMC_SCER_PCK1 (0x1u << 9) /**< \brief (PMC_SCER) Programmable Clock 1 Output Enable */
#define PMC_SCER_PCK2 (0x1u << 10) /**< \brief (PMC_SCER) Programmable Clock 2 Output Enable */
#define PMC_SCER_PCK3 (0x1u << 11) /**< \brief (PMC_SCER) Programmable Clock 3 Output Enable */
#define PMC_SCER_PCK4 (0x1u << 12) /**< \brief (PMC_SCER) Programmable Clock 4 Output Enable */
#define PMC_SCER_PCK5 (0x1u << 13) /**< \brief (PMC_SCER) Programmable Clock 5 Output Enable */
#define PMC_SCER_PCK6 (0x1u << 14) /**< \brief (PMC_SCER) Programmable Clock 6 Output Enable */
/* -------- PMC_SCDR : (PMC Offset: 0x0004) System Clock Disable Register -------- */
#define PMC_SCDR_USBCLK (0x1u << 5) /**< \brief (PMC_SCDR) Disable USB FS Clock */
#define PMC_SCDR_PCK0 (0x1u << 8) /**< \brief (PMC_SCDR) Programmable Clock 0 Output Disable */
#define PMC_SCDR_PCK1 (0x1u << 9) /**< \brief (PMC_SCDR) Programmable Clock 1 Output Disable */
#define PMC_SCDR_PCK2 (0x1u << 10) /**< \brief (PMC_SCDR) Programmable Clock 2 Output Disable */
#define PMC_SCDR_PCK3 (0x1u << 11) /**< \brief (PMC_SCDR) Programmable Clock 3 Output Disable */
#define PMC_SCDR_PCK4 (0x1u << 12) /**< \brief (PMC_SCDR) Programmable Clock 4 Output Disable */
#define PMC_SCDR_PCK5 (0x1u << 13) /**< \brief (PMC_SCDR) Programmable Clock 5 Output Disable */
#define PMC_SCDR_PCK6 (0x1u << 14) /**< \brief (PMC_SCDR) Programmable Clock 6 Output Disable */
/* -------- PMC_SCSR : (PMC Offset: 0x0008) System Clock Status Register -------- */
#define PMC_SCSR_HCLKS (0x1u << 0) /**< \brief (PMC_SCSR) Processor Clock Status */
#define PMC_SCSR_USBCLK (0x1u << 5) /**< \brief (PMC_SCSR) USB FS Clock Status */
#define PMC_SCSR_PCK0 (0x1u << 8) /**< \brief (PMC_SCSR) Programmable Clock 0 Output Status */
#define PMC_SCSR_PCK1 (0x1u << 9) /**< \brief (PMC_SCSR) Programmable Clock 1 Output Status */
#define PMC_SCSR_PCK2 (0x1u << 10) /**< \brief (PMC_SCSR) Programmable Clock 2 Output Status */
#define PMC_SCSR_PCK3 (0x1u << 11) /**< \brief (PMC_SCSR) Programmable Clock 3 Output Status */
#define PMC_SCSR_PCK4 (0x1u << 12) /**< \brief (PMC_SCSR) Programmable Clock 4 Output Status */
#define PMC_SCSR_PCK5 (0x1u << 13) /**< \brief (PMC_SCSR) Programmable Clock 5 Output Status */
#define PMC_SCSR_PCK6 (0x1u << 14) /**< \brief (PMC_SCSR) Programmable Clock 6 Output Status */
/* -------- PMC_PCER0 : (PMC Offset: 0x0010) Peripheral Clock Enable Register 0 -------- */
#define PMC_PCER0_PID7 (0x1u << 7) /**< \brief (PMC_PCER0) Peripheral Clock 7 Enable */
#define PMC_PCER0_PID8 (0x1u << 8) /**< \brief (PMC_PCER0) Peripheral Clock 8 Enable */
#define PMC_PCER0_PID9 (0x1u << 9) /**< \brief (PMC_PCER0) Peripheral Clock 9 Enable */
#define PMC_PCER0_PID10 (0x1u << 10) /**< \brief (PMC_PCER0) Peripheral Clock 10 Enable */
#define PMC_PCER0_PID11 (0x1u << 11) /**< \brief (PMC_PCER0) Peripheral Clock 11 Enable */
#define PMC_PCER0_PID12 (0x1u << 12) /**< \brief (PMC_PCER0) Peripheral Clock 12 Enable */
#define PMC_PCER0_PID13 (0x1u << 13) /**< \brief (PMC_PCER0) Peripheral Clock 13 Enable */
#define PMC_PCER0_PID14 (0x1u << 14) /**< \brief (PMC_PCER0) Peripheral Clock 14 Enable */
#define PMC_PCER0_PID15 (0x1u << 15) /**< \brief (PMC_PCER0) Peripheral Clock 15 Enable */
#define PMC_PCER0_PID16 (0x1u << 16) /**< \brief (PMC_PCER0) Peripheral Clock 16 Enable */
#define PMC_PCER0_PID17 (0x1u << 17) /**< \brief (PMC_PCER0) Peripheral Clock 17 Enable */
#define PMC_PCER0_PID18 (0x1u << 18) /**< \brief (PMC_PCER0) Peripheral Clock 18 Enable */
#define PMC_PCER0_PID19 (0x1u << 19) /**< \brief (PMC_PCER0) Peripheral Clock 19 Enable */
#define PMC_PCER0_PID20 (0x1u << 20) /**< \brief (PMC_PCER0) Peripheral Clock 20 Enable */
#define PMC_PCER0_PID21 (0x1u << 21) /**< \brief (PMC_PCER0) Peripheral Clock 21 Enable */
#define PMC_PCER0_PID22 (0x1u << 22) /**< \brief (PMC_PCER0) Peripheral Clock 22 Enable */
#define PMC_PCER0_PID23 (0x1u << 23) /**< \brief (PMC_PCER0) Peripheral Clock 23 Enable */
#define PMC_PCER0_PID24 (0x1u << 24) /**< \brief (PMC_PCER0) Peripheral Clock 24 Enable */
#define PMC_PCER0_PID25 (0x1u << 25) /**< \brief (PMC_PCER0) Peripheral Clock 25 Enable */
#define PMC_PCER0_PID26 (0x1u << 26) /**< \brief (PMC_PCER0) Peripheral Clock 26 Enable */
#define PMC_PCER0_PID27 (0x1u << 27) /**< \brief (PMC_PCER0) Peripheral Clock 27 Enable */
#define PMC_PCER0_PID28 (0x1u << 28) /**< \brief (PMC_PCER0) Peripheral Clock 28 Enable */
#define PMC_PCER0_PID29 (0x1u << 29) /**< \brief (PMC_PCER0) Peripheral Clock 29 Enable */
#define PMC_PCER0_PID30 (0x1u << 30) /**< \brief (PMC_PCER0) Peripheral Clock 30 Enable */
#define PMC_PCER0_PID31 (0x1u << 31) /**< \brief (PMC_PCER0) Peripheral Clock 31 Enable */
/* -------- PMC_PCDR0 : (PMC Offset: 0x0014) Peripheral Clock Disable Register 0 -------- */
#define PMC_PCDR0_PID7 (0x1u << 7) /**< \brief (PMC_PCDR0) Peripheral Clock 7 Disable */
#define PMC_PCDR0_PID8 (0x1u << 8) /**< \brief (PMC_PCDR0) Peripheral Clock 8 Disable */
#define PMC_PCDR0_PID9 (0x1u << 9) /**< \brief (PMC_PCDR0) Peripheral Clock 9 Disable */
#define PMC_PCDR0_PID10 (0x1u << 10) /**< \brief (PMC_PCDR0) Peripheral Clock 10 Disable */
#define PMC_PCDR0_PID11 (0x1u << 11) /**< \brief (PMC_PCDR0) Peripheral Clock 11 Disable */
#define PMC_PCDR0_PID12 (0x1u << 12) /**< \brief (PMC_PCDR0) Peripheral Clock 12 Disable */
#define PMC_PCDR0_PID13 (0x1u << 13) /**< \brief (PMC_PCDR0) Peripheral Clock 13 Disable */
#define PMC_PCDR0_PID14 (0x1u << 14) /**< \brief (PMC_PCDR0) Peripheral Clock 14 Disable */
#define PMC_PCDR0_PID15 (0x1u << 15) /**< \brief (PMC_PCDR0) Peripheral Clock 15 Disable */
#define PMC_PCDR0_PID16 (0x1u << 16) /**< \brief (PMC_PCDR0) Peripheral Clock 16 Disable */
#define PMC_PCDR0_PID17 (0x1u << 17) /**< \brief (PMC_PCDR0) Peripheral Clock 17 Disable */
#define PMC_PCDR0_PID18 (0x1u << 18) /**< \brief (PMC_PCDR0) Peripheral Clock 18 Disable */
#define PMC_PCDR0_PID19 (0x1u << 19) /**< \brief (PMC_PCDR0) Peripheral Clock 19 Disable */
#define PMC_PCDR0_PID20 (0x1u << 20) /**< \brief (PMC_PCDR0) Peripheral Clock 20 Disable */
#define PMC_PCDR0_PID21 (0x1u << 21) /**< \brief (PMC_PCDR0) Peripheral Clock 21 Disable */
#define PMC_PCDR0_PID22 (0x1u << 22) /**< \brief (PMC_PCDR0) Peripheral Clock 22 Disable */
#define PMC_PCDR0_PID23 (0x1u << 23) /**< \brief (PMC_PCDR0) Peripheral Clock 23 Disable */
#define PMC_PCDR0_PID24 (0x1u << 24) /**< \brief (PMC_PCDR0) Peripheral Clock 24 Disable */
#define PMC_PCDR0_PID25 (0x1u << 25) /**< \brief (PMC_PCDR0) Peripheral Clock 25 Disable */
#define PMC_PCDR0_PID26 (0x1u << 26) /**< \brief (PMC_PCDR0) Peripheral Clock 26 Disable */
#define PMC_PCDR0_PID27 (0x1u << 27) /**< \brief (PMC_PCDR0) Peripheral Clock 27 Disable */
#define PMC_PCDR0_PID28 (0x1u << 28) /**< \brief (PMC_PCDR0) Peripheral Clock 28 Disable */
#define PMC_PCDR0_PID29 (0x1u << 29) /**< \brief (PMC_PCDR0) Peripheral Clock 29 Disable */
#define PMC_PCDR0_PID30 (0x1u << 30) /**< \brief (PMC_PCDR0) Peripheral Clock 30 Disable */
#define PMC_PCDR0_PID31 (0x1u << 31) /**< \brief (PMC_PCDR0) Peripheral Clock 31 Disable */
/* -------- PMC_PCSR0 : (PMC Offset: 0x0018) Peripheral Clock Status Register 0 -------- */
#define PMC_PCSR0_PID7 (0x1u << 7) /**< \brief (PMC_PCSR0) Peripheral Clock 7 Status */
#define PMC_PCSR0_PID8 (0x1u << 8) /**< \brief (PMC_PCSR0) Peripheral Clock 8 Status */
#define PMC_PCSR0_PID9 (0x1u << 9) /**< \brief (PMC_PCSR0) Peripheral Clock 9 Status */
#define PMC_PCSR0_PID10 (0x1u << 10) /**< \brief (PMC_PCSR0) Peripheral Clock 10 Status */
#define PMC_PCSR0_PID11 (0x1u << 11) /**< \brief (PMC_PCSR0) Peripheral Clock 11 Status */
#define PMC_PCSR0_PID12 (0x1u << 12) /**< \brief (PMC_PCSR0) Peripheral Clock 12 Status */
#define PMC_PCSR0_PID13 (0x1u << 13) /**< \brief (PMC_PCSR0) Peripheral Clock 13 Status */
#define PMC_PCSR0_PID14 (0x1u << 14) /**< \brief (PMC_PCSR0) Peripheral Clock 14 Status */
#define PMC_PCSR0_PID15 (0x1u << 15) /**< \brief (PMC_PCSR0) Peripheral Clock 15 Status */
#define PMC_PCSR0_PID16 (0x1u << 16) /**< \brief (PMC_PCSR0) Peripheral Clock 16 Status */
#define PMC_PCSR0_PID17 (0x1u << 17) /**< \brief (PMC_PCSR0) Peripheral Clock 17 Status */
#define PMC_PCSR0_PID18 (0x1u << 18) /**< \brief (PMC_PCSR0) Peripheral Clock 18 Status */
#define PMC_PCSR0_PID19 (0x1u << 19) /**< \brief (PMC_PCSR0) Peripheral Clock 19 Status */
#define PMC_PCSR0_PID20 (0x1u << 20) /**< \brief (PMC_PCSR0) Peripheral Clock 20 Status */
#define PMC_PCSR0_PID21 (0x1u << 21) /**< \brief (PMC_PCSR0) Peripheral Clock 21 Status */
#define PMC_PCSR0_PID22 (0x1u << 22) /**< \brief (PMC_PCSR0) Peripheral Clock 22 Status */
#define PMC_PCSR0_PID23 (0x1u << 23) /**< \brief (PMC_PCSR0) Peripheral Clock 23 Status */
#define PMC_PCSR0_PID24 (0x1u << 24) /**< \brief (PMC_PCSR0) Peripheral Clock 24 Status */
#define PMC_PCSR0_PID25 (0x1u << 25) /**< \brief (PMC_PCSR0) Peripheral Clock 25 Status */
#define PMC_PCSR0_PID26 (0x1u << 26) /**< \brief (PMC_PCSR0) Peripheral Clock 26 Status */
#define PMC_PCSR0_PID27 (0x1u << 27) /**< \brief (PMC_PCSR0) Peripheral Clock 27 Status */
#define PMC_PCSR0_PID28 (0x1u << 28) /**< \brief (PMC_PCSR0) Peripheral Clock 28 Status */
#define PMC_PCSR0_PID29 (0x1u << 29) /**< \brief (PMC_PCSR0) Peripheral Clock 29 Status */
#define PMC_PCSR0_PID30 (0x1u << 30) /**< \brief (PMC_PCSR0) Peripheral Clock 30 Status */
#define PMC_PCSR0_PID31 (0x1u << 31) /**< \brief (PMC_PCSR0) Peripheral Clock 31 Status */
/* -------- CKGR_UCKR : (PMC Offset: 0x001C) UTMI Clock Register -------- */
#define CKGR_UCKR_UPLLEN (0x1u << 16) /**< \brief (CKGR_UCKR) UTMI PLL Enable */
#define CKGR_UCKR_UPLLCOUNT_Pos 20
#define CKGR_UCKR_UPLLCOUNT_Msk (0xfu << CKGR_UCKR_UPLLCOUNT_Pos) /**< \brief (CKGR_UCKR) UTMI PLL Start-up Time */
#define CKGR_UCKR_UPLLCOUNT(value) ((CKGR_UCKR_UPLLCOUNT_Msk & ((value) << CKGR_UCKR_UPLLCOUNT_Pos)))
/* -------- CKGR_MOR : (PMC Offset: 0x0020) Main Oscillator Register -------- */
#define CKGR_MOR_MOSCXTEN (0x1u << 0) /**< \brief (CKGR_MOR) Main Crystal Oscillator Enable */
#define CKGR_MOR_MOSCXTBY (0x1u << 1) /**< \brief (CKGR_MOR) Main Crystal Oscillator Bypass */
#define CKGR_MOR_WAITMODE (0x1u << 2) /**< \brief (CKGR_MOR) Wait Mode Command (Write-only) */
#define CKGR_MOR_MOSCRCEN (0x1u << 3) /**< \brief (CKGR_MOR) Main On-Chip RC Oscillator Enable */
#define CKGR_MOR_MOSCRCF_Pos 4
#define CKGR_MOR_MOSCRCF_Msk (0x7u << CKGR_MOR_MOSCRCF_Pos) /**< \brief (CKGR_MOR) Main On-Chip RC Oscillator Frequency Selection */
#define CKGR_MOR_MOSCRCF(value) ((CKGR_MOR_MOSCRCF_Msk & ((value) << CKGR_MOR_MOSCRCF_Pos)))
#define   CKGR_MOR_MOSCRCF_4_MHz (0x0u << 4) /**< \brief (CKGR_MOR) Fast RC oscillator frequency is at 4 MHz (default) */
#define   CKGR_MOR_MOSCRCF_8_MHz (0x1u << 4) /**< \brief (CKGR_MOR) Fast RC oscillator frequency is at 8 MHz */
#define   CKGR_MOR_MOSCRCF_12_MHz (0x2u << 4) /**< \brief (CKGR_MOR) Fast RC oscillator frequency is at 12 MHz */
#define CKGR_MOR_MOSCXTST_Pos 8
#define CKGR_MOR_MOSCXTST_Msk (0xffu << CKGR_MOR_MOSCXTST_Pos) /**< \brief (CKGR_MOR) Main Crystal Oscillator Start-up Time */
#define CKGR_MOR_MOSCXTST(value) ((CKGR_MOR_MOSCXTST_Msk & ((value) << CKGR_MOR_MOSCXTST_Pos)))
#define CKGR_MOR_KEY_Pos 16
#define CKGR_MOR_KEY_Msk (0xffu << CKGR_MOR_KEY_Pos) /**< \brief (CKGR_MOR) Write Access Password */
#define CKGR_MOR_KEY(value) ((CKGR_MOR_KEY_Msk & ((value) << CKGR_MOR_KEY_Pos)))
#define   CKGR_MOR_KEY_PASSWD (0x37u << 16) /**< \brief (CKGR_MOR) Writing any other value in this field aborts the write operation.Always reads as 0. */
#define CKGR_MOR_MOSCSEL (0x1u << 24) /**< \brief (CKGR_MOR) Main Oscillator Selection */
#define CKGR_MOR_CFDEN (0x1u << 25) /**< \brief (CKGR_MOR) Clock Failure Detector Enable */
#define CKGR_MOR_XT32KFME (0x1u << 26) /**< \brief (CKGR_MOR) Slow Crystal Oscillator Frequency Monitoring Enable */
/* -------- CKGR_MCFR : (PMC Offset: 0x0024) Main Clock Frequency Register -------- */
#define CKGR_MCFR_MAINF_Pos 0
#define CKGR_MCFR_MAINF_Msk (0xffffu << CKGR_MCFR_MAINF_Pos) /**< \brief (CKGR_MCFR) Main Clock Frequency */
#define CKGR_MCFR_MAINF(value) ((CKGR_MCFR_MAINF_Msk & ((value) << CKGR_MCFR_MAINF_Pos)))
#define CKGR_MCFR_MAINFRDY (0x1u << 16) /**< \brief (CKGR_MCFR) Main Clock Frequency Measure Ready */
#define CKGR_MCFR_RCMEAS (0x1u << 20) /**< \brief (CKGR_MCFR) RC Oscillator Frequency Measure (write-only) */
#define CKGR_MCFR_CCSS (0x1u << 24) /**< \brief (CKGR_MCFR) Counter Clock Source Selection */
/* -------- CKGR_PLLAR : (PMC Offset: 0x0028) PLLA Register -------- */
#define CKGR_PLLAR_DIVA_Pos 0
#define CKGR_PLLAR_DIVA_Msk (0xffu << CKGR_PLLAR_DIVA_Pos) /**< \brief (CKGR_PLLAR) PLLA Front End Divider */
#define CKGR_PLLAR_DIVA(value) ((CKGR_PLLAR_DIVA_Msk & ((value) << CKGR_PLLAR_DIVA_Pos)))
#define   CKGR_PLLAR_DIVA_0 (0x0u << 0) /**< \brief (CKGR_PLLAR) Divider output is 0 and PLLA is disabled. */
#define   CKGR_PLLAR_DIVA_BYPASS (0x1u << 0) /**< \brief (CKGR_PLLAR) Divider is bypassed (divide by 1) and PLLA is enabled. */
#define CKGR_PLLAR_PLLACOUNT_Pos 8
#define CKGR_PLLAR_PLLACOUNT_Msk (0x3fu << CKGR_PLLAR_PLLACOUNT_Pos) /**< \brief (CKGR_PLLAR) PLLA Counter */
#define CKGR_PLLAR_PLLACOUNT(value) ((CKGR_PLLAR_PLLACOUNT_Msk & ((value) << CKGR_PLLAR_PLLACOUNT_Pos)))
#define CKGR_PLLAR_MULA_Pos 16
#define CKGR_PLLAR_MULA_Msk (0x7ffu << CKGR_PLLAR_MULA_Pos) /**< \brief (CKGR_PLLAR) PLLA Multiplier */
#define CKGR_PLLAR_MULA(value) ((CKGR_PLLAR_MULA_Msk & ((value) << CKGR_PLLAR_MULA_Pos)))
#define CKGR_PLLAR_ONE (0x1u << 29) /**< \brief (CKGR_PLLAR) Must Be Set to 1 */
/* -------- PMC_MCKR : (PMC Offset: 0x0030) Master Clock Register -------- */
#define PMC_MCKR_CSS_Pos 0
#define PMC_MCKR_CSS_Msk (0x3u << PMC_MCKR_CSS_Pos) /**< \brief (PMC_MCKR) Master Clock Source Selection */
#define PMC_MCKR_CSS(value) ((PMC_MCKR_CSS_Msk & ((value) << PMC_MCKR_CSS_Pos)))
#define   PMC_MCKR_CSS_SLOW_CLK (0x0u << 0) /**< \brief (PMC_MCKR) Slow Clock is selected */
#define   PMC_MCKR_CSS_MAIN_CLK (0x1u << 0) /**< \brief (PMC_MCKR) Main Clock is selected */
#define   PMC_MCKR_CSS_PLLA_CLK (0x2u << 0) /**< \brief (PMC_MCKR) PLLA Clock is selected */
#define   PMC_MCKR_CSS_UPLL_CLK (0x3u << 0) /**< \brief (PMC_MCKR) Divided UPLL Clock  is selected */
#define PMC_MCKR_PRES_Pos 4
#define PMC_MCKR_PRES_Msk (0x7u << PMC_MCKR_PRES_Pos) /**< \brief (PMC_MCKR) Processor Clock Prescaler */
#define PMC_MCKR_PRES(value) ((PMC_MCKR_PRES_Msk & ((value) << PMC_MCKR_PRES_Pos)))
#define   PMC_MCKR_PRES_CLK_1 (0x0u << 4) /**< \brief (PMC_MCKR) Selected clock */
#define   PMC_MCKR_PRES_CLK_2 (0x1u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 2 */
#define   PMC_MCKR_PRES_CLK_4 (0x2u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 4 */
#define   PMC_MCKR_PRES_CLK_8 (0x3u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 8 */
#define   PMC_MCKR_PRES_CLK_16 (0x4u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 16 */
#define   PMC_MCKR_PRES_CLK_32 (0x5u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 32 */
#define   PMC_MCKR_PRES_CLK_64 (0x6u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 64 */
#define   PMC_MCKR_PRES_CLK_3 (0x7u << 4) /**< \brief (PMC_MCKR) Selected clock divided by 3 */
#define PMC_MCKR_MDIV_Pos 8
#define PMC_MCKR_MDIV_Msk (0x3u << PMC_MCKR_MDIV_Pos) /**< \brief (PMC_MCKR) Master Clock Division */
#define PMC_MCKR_MDIV(value) ((PMC_MCKR_MDIV_Msk & ((value) << PMC_MCKR_MDIV_Pos)))
#define   PMC_MCKR_MDIV_EQ_PCK (0x0u << 8) /**< \brief (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 1. */
#define   PMC_MCKR_MDIV_PCK_DIV2 (0x1u << 8) /**< \brief (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 2. */
#define   PMC_MCKR_MDIV_PCK_DIV4 (0x2u << 8) /**< \brief (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 4. */
#define   PMC_MCKR_MDIV_PCK_DIV3 (0x3u << 8) /**< \brief (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 3. */
#define PMC_MCKR_UPLLDIV2 (0x1u << 13) /**< \brief (PMC_MCKR) UPLL Divisor by 2 */
/* -------- PMC_USB : (PMC Offset: 0x0038) USB Clock Register -------- */
#define PMC_USB_USBS (0x1u << 0) /**< \brief (PMC_USB) USB Input Clock Selection */
#define PMC_USB_USBDIV_Pos 8
#define PMC_USB_USBDIV_Msk (0xfu << PMC_USB_USBDIV_Pos) /**< \brief (PMC_USB) Divider for USB Clock */
#define PMC_USB_USBDIV(value) ((PMC_USB_USBDIV_Msk & ((value) << PMC_USB_USBDIV_Pos)))
/* -------- PMC_PCK[8] : (PMC Offset: 0x40) Programmable Clock Register (chid = 0) -------- */
#define PMC_PCK_CSS_Pos 0
#define PMC_PCK_CSS_Msk (0x7u << PMC_PCK_CSS_Pos) /**< \brief (PMC_PCK[8]) Programmable Clock Source Selection */
#define PMC_PCK_CSS(value) ((PMC_PCK_CSS_Msk & ((value) << PMC_PCK_CSS_Pos)))
#define   PMC_PCK_CSS_SLOW_CLK (0x0u << 0) /**< \brief (PMC_PCK[8]) Slow Clock is selected */
#define   PMC_PCK_CSS_MAIN_CLK (0x1u << 0) /**< \brief (PMC_PCK[8]) Main Clock is selected */
#define   PMC_PCK_CSS_PLLA_CLK (0x2u << 0) /**< \brief (PMC_PCK[8]) PLLA Clock is selected */
#define   PMC_PCK_CSS_UPLL_CLK (0x3u << 0) /**< \brief (PMC_PCK[8]) Divided UPLL Clock is selected */
#define   PMC_PCK_CSS_MCK (0x4u << 0) /**< \brief (PMC_PCK[8]) Master Clock is selected */
#define PMC_PCK_PRES_Pos 4
#define PMC_PCK_PRES_Msk (0xffu << PMC_PCK_PRES_Pos) /**< \brief (PMC_PCK[8]) Programmable Clock Prescaler */
#define PMC_PCK_PRES(value) ((PMC_PCK_PRES_Msk & ((value) << PMC_PCK_PRES_Pos)))
/* -------- PMC_IER : (PMC Offset: 0x0060) Interrupt Enable Register -------- */
#define PMC_IER_MOSCXTS (0x1u << 0) /**< \brief (PMC_IER) Main Crystal Oscillator Status Interrupt Enable */
#define PMC_IER_LOCKA (0x1u << 1) /**< \brief (PMC_IER) PLLA Lock Interrupt Enable */
#define PMC_IER_MCKRDY (0x1u << 3) /**< \brief (PMC_IER) Master Clock Ready Interrupt Enable */
#define PMC_IER_LOCKU (0x1u << 6) /**< \brief (PMC_IER) UTMI PLL Lock Interrupt Enable */
#define PMC_IER_PCKRDY0 (0x1u << 8) /**< \brief (PMC_IER) Programmable Clock Ready 0 Interrupt Enable */
#define PMC_IER_PCKRDY1 (0x1u << 9) /**< \brief (PMC_IER) Programmable Clock Ready 1 Interrupt Enable */
#define PMC_IER_PCKRDY2 (0x1u << 10) /**< \brief (PMC_IER) Programmable Clock Ready 2 Interrupt Enable */
#define PMC_IER_PCKRDY3 (0x1u << 11) /**< \brief (PMC_IER) Programmable Clock Ready 3 Interrupt Enable */
#define PMC_IER_PCKRDY4 (0x1u << 12) /**< \brief (PMC_IER) Programmable Clock Ready 4 Interrupt Enable */
#define PMC_IER_PCKRDY5 (0x1u << 13) /**< \brief (PMC_IER) Programmable Clock Ready 5 Interrupt Enable */
#define PMC_IER_PCKRDY6 (0x1u << 14) /**< \brief (PMC_IER) Programmable Clock Ready 6 Interrupt Enable */
#define PMC_IER_MOSCSELS (0x1u << 16) /**< \brief (PMC_IER) Main Oscillator Selection Status Interrupt Enable */
#define PMC_IER_MOSCRCS (0x1u << 17) /**< \brief (PMC_IER) Main On-Chip RC Status Interrupt Enable */
#define PMC_IER_CFDEV (0x1u << 18) /**< \brief (PMC_IER) Clock Failure Detector Event Interrupt Enable */
#define PMC_IER_XT32KERR (0x1u << 21) /**< \brief (PMC_IER) Slow Crystal Oscillator Error Interrupt Enable */
/* -------- PMC_IDR : (PMC Offset: 0x0064) Interrupt Disable Register -------- */
#define PMC_IDR_MOSCXTS (0x1u << 0) /**< \brief (PMC_IDR) Main Crystal Oscillator Status Interrupt Disable */
#define PMC_IDR_LOCKA (0x1u << 1) /**< \brief (PMC_IDR) PLLA Lock Interrupt Disable */
#define PMC_IDR_MCKRDY (0x1u << 3) /**< \brief (PMC_IDR) Master Clock Ready Interrupt Disable */
#define PMC_IDR_LOCKU (0x1u << 6) /**< \brief (PMC_IDR) UTMI PLL Lock Interrupt Disable */
#define PMC_IDR_PCKRDY0 (0x1u << 8) /**< \brief (PMC_IDR) Programmable Clock Ready 0 Interrupt Disable */
#define PMC_IDR_PCKRDY1 (0x1u << 9) /**< \brief (PMC_IDR) Programmable Clock Ready 1 Interrupt Disable */
#define PMC_IDR_PCKRDY2 (0x1u << 10) /**< \brief (PMC_IDR) Programmable Clock Ready 2 Interrupt Disable */
#define PMC_IDR_PCKRDY3 (0x1u << 11) /**< \brief (PMC_IDR) Programmable Clock Ready 3 Interrupt Disable */
#define PMC_IDR_PCKRDY4 (0x1u << 12) /**< \brief (PMC_IDR) Programmable Clock Ready 4 Interrupt Disable */
#define PMC_IDR_PCKRDY5 (0x1u << 13) /**< \brief (PMC_IDR) Programmable Clock Ready 5 Interrupt Disable */
#define PMC_IDR_PCKRDY6 (0x1u << 14) /**< \brief (PMC_IDR) Programmable Clock Ready 6 Interrupt Disable */
#define PMC_IDR_MOSCSELS (0x1u << 16) /**< \brief (PMC_IDR) Main Oscillator Selection Status Interrupt Disable */
#define PMC_IDR_MOSCRCS (0x1u << 17) /**< \brief (PMC_IDR) Main On-Chip RC Status Interrupt Disable */
#define PMC_IDR_CFDEV (0x1u << 18) /**< \brief (PMC_IDR) Clock Failure Detector Event Interrupt Disable */
#define PMC_IDR_XT32KERR (0x1u << 21) /**< \brief (PMC_IDR) Slow Crystal Oscillator Error Interrupt Disable */
/* -------- PMC_SR : (PMC Offset: 0x0068) Status Register -------- */
#define PMC_SR_MOSCXTS (0x1u << 0) /**< \brief (PMC_SR) Main Crystal Oscillator Status */
#define PMC_SR_LOCKA (0x1u << 1) /**< \brief (PMC_SR) PLLA Lock Status */
#define PMC_SR_MCKRDY (0x1u << 3) /**< \brief (PMC_SR) Master Clock Status */
#define PMC_SR_LOCKU (0x1u << 6) /**< \brief (PMC_SR) UTMI PLL Lock Status */
#define PMC_SR_OSCSELS (0x1u << 7) /**< \brief (PMC_SR) Slow Clock Oscillator Selection */
#define PMC_SR_PCKRDY0 (0x1u << 8) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_PCKRDY1 (0x1u << 9) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_PCKRDY2 (0x1u << 10) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_PCKRDY3 (0x1u << 11) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_PCKRDY4 (0x1u << 12) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_PCKRDY5 (0x1u << 13) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_PCKRDY6 (0x1u << 14) /**< \brief (PMC_SR) Programmable Clock Ready Status */
#define PMC_SR_MOSCSELS (0x1u << 16) /**< \brief (PMC_SR) Main Oscillator Selection Status */
#define PMC_SR_MOSCRCS (0x1u << 17) /**< \brief (PMC_SR) Main On-Chip RC Oscillator Status */
#define PMC_SR_CFDEV (0x1u << 18) /**< \brief (PMC_SR) Clock Failure Detector Event */
#define PMC_SR_CFDS (0x1u << 19) /**< \brief (PMC_SR) Clock Failure Detector Status */
#define PMC_SR_FOS (0x1u << 20) /**< \brief (PMC_SR) Clock Failure Detector Fault Output Status */
#define PMC_SR_XT32KERR (0x1u << 21) /**< \brief (PMC_SR) Slow Crystal Oscillator Error */
/* -------- PMC_IMR : (PMC Offset: 0x006C) Interrupt Mask Register -------- */
#define PMC_IMR_MOSCXTS (0x1u << 0) /**< \brief (PMC_IMR) Main Crystal Oscillator Status Interrupt Mask */
#define PMC_IMR_LOCKA (0x1u << 1) /**< \brief (PMC_IMR) PLLA Lock Interrupt Mask */
#define PMC_IMR_MCKRDY (0x1u << 3) /**< \brief (PMC_IMR) Master Clock Ready Interrupt Mask */
#define PMC_IMR_LOCKU (0x1u << 6) /**< \brief (PMC_IMR) UTMI PLL Lock Interrupt Mask */
#define PMC_IMR_PCKRDY0 (0x1u << 8) /**< \brief (PMC_IMR) Programmable Clock Ready 0 Interrupt Mask */
#define PMC_IMR_PCKRDY1 (0x1u << 9) /**< \brief (PMC_IMR) Programmable Clock Ready 1 Interrupt Mask */
#define PMC_IMR_PCKRDY2 (0x1u << 10) /**< \brief (PMC_IMR) Programmable Clock Ready 2 Interrupt Mask */
#define PMC_IMR_PCKRDY3 (0x1u << 11) /**< \brief (PMC_IMR) Programmable Clock Ready 3 Interrupt Mask */
#define PMC_IMR_PCKRDY4 (0x1u << 12) /**< \brief (PMC_IMR) Programmable Clock Ready 4 Interrupt Mask */
#define PMC_IMR_PCKRDY5 (0x1u << 13) /**< \brief (PMC_IMR) Programmable Clock Ready 5 Interrupt Mask */
#define PMC_IMR_PCKRDY6 (0x1u << 14) /**< \brief (PMC_IMR) Programmable Clock Ready 6 Interrupt Mask */
#define PMC_IMR_MOSCSELS (0x1u << 16) /**< \brief (PMC_IMR) Main Oscillator Selection Status Interrupt Mask */
#define PMC_IMR_MOSCRCS (0x1u << 17) /**< \brief (PMC_IMR) Main On-Chip RC Status Interrupt Mask */
#define PMC_IMR_CFDEV (0x1u << 18) /**< \brief (PMC_IMR) Clock Failure Detector Event Interrupt Mask */
#define PMC_IMR_XT32KERR (0x1u << 21) /**< \brief (PMC_IMR) Slow Crystal Oscillator Error Interrupt Mask */
/* -------- PMC_FSMR : (PMC Offset: 0x0070) Fast Startup Mode Register -------- */
#define PMC_FSMR_FSTT0 (0x1u << 0) /**< \brief (PMC_FSMR) Fast Startup Input Enable 0 */
#define PMC_FSMR_FSTT1 (0x1u << 1) /**< \brief (PMC_FSMR) Fast Startup Input Enable 1 */
#define PMC_FSMR_FSTT2 (0x1u << 2) /**< \brief (PMC_FSMR) Fast Startup Input Enable 2 */
#define PMC_FSMR_FSTT3 (0x1u << 3) /**< \brief (PMC_FSMR) Fast Startup Input Enable 3 */
#define PMC_FSMR_FSTT4 (0x1u << 4) /**< \brief (PMC_FSMR) Fast Startup Input Enable 4 */
#define PMC_FSMR_FSTT5 (0x1u << 5) /**< \brief (PMC_FSMR) Fast Startup Input Enable 5 */
#define PMC_FSMR_FSTT6 (0x1u << 6) /**< \brief (PMC_FSMR) Fast Startup Input Enable 6 */
#define PMC_FSMR_FSTT7 (0x1u << 7) /**< \brief (PMC_FSMR) Fast Startup Input Enable 7 */
#define PMC_FSMR_FSTT8 (0x1u << 8) /**< \brief (PMC_FSMR) Fast Startup Input Enable 8 */
#define PMC_FSMR_FSTT9 (0x1u << 9) /**< \brief (PMC_FSMR) Fast Startup Input Enable 9 */
#define PMC_FSMR_FSTT10 (0x1u << 10) /**< \brief (PMC_FSMR) Fast Startup Input Enable 10 */
#define PMC_FSMR_FSTT11 (0x1u << 11) /**< \brief (PMC_FSMR) Fast Startup Input Enable 11 */
#define PMC_FSMR_FSTT12 (0x1u << 12) /**< \brief (PMC_FSMR) Fast Startup Input Enable 12 */
#define PMC_FSMR_FSTT13 (0x1u << 13) /**< \brief (PMC_FSMR) Fast Startup Input Enable 13 */
#define PMC_FSMR_FSTT14 (0x1u << 14) /**< \brief (PMC_FSMR) Fast Startup Input Enable 14 */
#define PMC_FSMR_FSTT15 (0x1u << 15) /**< \brief (PMC_FSMR) Fast Startup Input Enable 15 */
#define PMC_FSMR_RTTAL (0x1u << 16) /**< \brief (PMC_FSMR) RTT Alarm Enable */
#define PMC_FSMR_RTCAL (0x1u << 17) /**< \brief (PMC_FSMR) RTC Alarm Enable */
#define PMC_FSMR_USBAL (0x1u << 18) /**< \brief (PMC_FSMR) USB Alarm Enable */
#define PMC_FSMR_LPM (0x1u << 20) /**< \brief (PMC_FSMR) Low-power Mode */
#define PMC_FSMR_FLPM_Pos 21
#define PMC_FSMR_FLPM_Msk (0x3u << PMC_FSMR_FLPM_Pos) /**< \brief (PMC_FSMR) Flash Low-power Mode */
#define PMC_FSMR_FLPM(value) ((PMC_FSMR_FLPM_Msk & ((value) << PMC_FSMR_FLPM_Pos)))
#define   PMC_FSMR_FLPM_FLASH_STANDBY (0x0u << 21) /**< \brief (PMC_FSMR) Flash is in Standby Mode when system enters Wait Mode */
#define   PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN (0x1u << 21) /**< \brief (PMC_FSMR) Flash is in Deep-power-down mode when system enters Wait Mode */
#define   PMC_FSMR_FLPM_FLASH_IDLE (0x2u << 21) /**< \brief (PMC_FSMR) Idle mode */
#define PMC_FSMR_FFLPM (0x1u << 23) /**< \brief (PMC_FSMR) Force Flash Low-power Mode */
/* -------- PMC_FSPR : (PMC Offset: 0x0074) Fast Startup Polarity Register -------- */
#define PMC_FSPR_FSTP0 (0x1u << 0) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 0 */
#define PMC_FSPR_FSTP1 (0x1u << 1) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 1 */
#define PMC_FSPR_FSTP2 (0x1u << 2) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 2 */
#define PMC_FSPR_FSTP3 (0x1u << 3) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 3 */
#define PMC_FSPR_FSTP4 (0x1u << 4) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 4 */
#define PMC_FSPR_FSTP5 (0x1u << 5) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 5 */
#define PMC_FSPR_FSTP6 (0x1u << 6) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 6 */
#define PMC_FSPR_FSTP7 (0x1u << 7) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 7 */
#define PMC_FSPR_FSTP8 (0x1u << 8) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 8 */
#define PMC_FSPR_FSTP9 (0x1u << 9) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 9 */
#define PMC_FSPR_FSTP10 (0x1u << 10) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 10 */
#define PMC_FSPR_FSTP11 (0x1u << 11) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 11 */
#define PMC_FSPR_FSTP12 (0x1u << 12) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 12 */
#define PMC_FSPR_FSTP13 (0x1u << 13) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 13 */
#define PMC_FSPR_FSTP14 (0x1u << 14) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 14 */
#define PMC_FSPR_FSTP15 (0x1u << 15) /**< \brief (PMC_FSPR) Fast Startup Input Polarity 15 */
/* -------- PMC_FOCR : (PMC Offset: 0x0078) Fault Output Clear Register -------- */
#define PMC_FOCR_FOCLR (0x1u << 0) /**< \brief (PMC_FOCR) Fault Output Clear */
/* -------- PMC_WPMR : (PMC Offset: 0x00E4) Write Protection Mode Register -------- */
#define PMC_WPMR_WPEN (0x1u << 0) /**< \brief (PMC_WPMR) Write Protection Enable */
#define PMC_WPMR_WPKEY_Pos 8
#define PMC_WPMR_WPKEY_Msk (0xffffffu << PMC_WPMR_WPKEY_Pos) /**< \brief (PMC_WPMR) Write Protection Key */
#define PMC_WPMR_WPKEY(value) ((PMC_WPMR_WPKEY_Msk & ((value) << PMC_WPMR_WPKEY_Pos)))
#define   PMC_WPMR_WPKEY_PASSWD (0x504D43u << 8) /**< \brief (PMC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. */
/* -------- PMC_WPSR : (PMC Offset: 0x00E8) Write Protection Status Register -------- */
#define PMC_WPSR_WPVS (0x1u << 0) /**< \brief (PMC_WPSR) Write Protection Violation Status */
#define PMC_WPSR_WPVSRC_Pos 8
#define PMC_WPSR_WPVSRC_Msk (0xffffu << PMC_WPSR_WPVSRC_Pos) /**< \brief (PMC_WPSR) Write Protection Violation Source */
/* -------- PMC_VERSION : (PMC Offset: 0x00FC) Version Register -------- */
#define PMC_VERSION_VERSION_Pos 0
#define PMC_VERSION_VERSION_Msk (0xfffu << PMC_VERSION_VERSION_Pos) /**< \brief (PMC_VERSION) Version of the Hardware Module */
#define PMC_VERSION_MFN_Pos 16
#define PMC_VERSION_MFN_Msk (0x7u << PMC_VERSION_MFN_Pos) /**< \brief (PMC_VERSION) Metal Fix Number */
/* -------- PMC_PCER1 : (PMC Offset: 0x0100) Peripheral Clock Enable Register 1 -------- */
#define PMC_PCER1_PID32 (0x1u << 0) /**< \brief (PMC_PCER1) Peripheral Clock 32 Enable */
#define PMC_PCER1_PID33 (0x1u << 1) /**< \brief (PMC_PCER1) Peripheral Clock 33 Enable */
#define PMC_PCER1_PID34 (0x1u << 2) /**< \brief (PMC_PCER1) Peripheral Clock 34 Enable */
#define PMC_PCER1_PID35 (0x1u << 3) /**< \brief (PMC_PCER1) Peripheral Clock 35 Enable */
#define PMC_PCER1_PID37 (0x1u << 5) /**< \brief (PMC_PCER1) Peripheral Clock 37 Enable */
#define PMC_PCER1_PID39 (0x1u << 7) /**< \brief (PMC_PCER1) Peripheral Clock 39 Enable */
#define PMC_PCER1_PID40 (0x1u << 8) /**< \brief (PMC_PCER1) Peripheral Clock 40 Enable */
#define PMC_PCER1_PID41 (0x1u << 9) /**< \brief (PMC_PCER1) Peripheral Clock 41 Enable */
#define PMC_PCER1_PID42 (0x1u << 10) /**< \brief (PMC_PCER1) Peripheral Clock 42 Enable */
#define PMC_PCER1_PID43 (0x1u << 11) /**< \brief (PMC_PCER1) Peripheral Clock 43 Enable */
#define PMC_PCER1_PID44 (0x1u << 12) /**< \brief (PMC_PCER1) Peripheral Clock 44 Enable */
#define PMC_PCER1_PID45 (0x1u << 13) /**< \brief (PMC_PCER1) Peripheral Clock 45 Enable */
#define PMC_PCER1_PID46 (0x1u << 14) /**< \brief (PMC_PCER1) Peripheral Clock 46 Enable */
#define PMC_PCER1_PID47 (0x1u << 15) /**< \brief (PMC_PCER1) Peripheral Clock 47 Enable */
#define PMC_PCER1_PID48 (0x1u << 16) /**< \brief (PMC_PCER1) Peripheral Clock 48 Enable */
#define PMC_PCER1_PID49 (0x1u << 17) /**< \brief (PMC_PCER1) Peripheral Clock 49 Enable */
#define PMC_PCER1_PID50 (0x1u << 18) /**< \brief (PMC_PCER1) Peripheral Clock 50 Enable */
#define PMC_PCER1_PID51 (0x1u << 19) /**< \brief (PMC_PCER1) Peripheral Clock 51 Enable */
#define PMC_PCER1_PID52 (0x1u << 20) /**< \brief (PMC_PCER1) Peripheral Clock 52 Enable */
#define PMC_PCER1_PID53 (0x1u << 21) /**< \brief (PMC_PCER1) Peripheral Clock 53 Enable */
#define PMC_PCER1_PID56 (0x1u << 24) /**< \brief (PMC_PCER1) Peripheral Clock 56 Enable */
#define PMC_PCER1_PID57 (0x1u << 25) /**< \brief (PMC_PCER1) Peripheral Clock 57 Enable */
#define PMC_PCER1_PID58 (0x1u << 26) /**< \brief (PMC_PCER1) Peripheral Clock 58 Enable */
#define PMC_PCER1_PID59 (0x1u << 27) /**< \brief (PMC_PCER1) Peripheral Clock 59 Enable */
#define PMC_PCER1_PID60 (0x1u << 28) /**< \brief (PMC_PCER1) Peripheral Clock 60 Enable */
/* -------- PMC_PCDR1 : (PMC Offset: 0x0104) Peripheral Clock Disable Register 1 -------- */
#define PMC_PCDR1_PID32 (0x1u << 0) /**< \brief (PMC_PCDR1) Peripheral Clock 32 Disable */
#define PMC_PCDR1_PID33 (0x1u << 1) /**< \brief (PMC_PCDR1) Peripheral Clock 33 Disable */
#define PMC_PCDR1_PID34 (0x1u << 2) /**< \brief (PMC_PCDR1) Peripheral Clock 34 Disable */
#define PMC_PCDR1_PID35 (0x1u << 3) /**< \brief (PMC_PCDR1) Peripheral Clock 35 Disable */
#define PMC_PCDR1_PID37 (0x1u << 5) /**< \brief (PMC_PCDR1) Peripheral Clock 37 Disable */
#define PMC_PCDR1_PID39 (0x1u << 7) /**< \brief (PMC_PCDR1) Peripheral Clock 39 Disable */
#define PMC_PCDR1_PID40 (0x1u << 8) /**< \brief (PMC_PCDR1) Peripheral Clock 40 Disable */
#define PMC_PCDR1_PID41 (0x1u << 9) /**< \brief (PMC_PCDR1) Peripheral Clock 41 Disable */
#define PMC_PCDR1_PID42 (0x1u << 10) /**< \brief (PMC_PCDR1) Peripheral Clock 42 Disable */
#define PMC_PCDR1_PID43 (0x1u << 11) /**< \brief (PMC_PCDR1) Peripheral Clock 43 Disable */
#define PMC_PCDR1_PID44 (0x1u << 12) /**< \brief (PMC_PCDR1) Peripheral Clock 44 Disable */
#define PMC_PCDR1_PID45 (0x1u << 13) /**< \brief (PMC_PCDR1) Peripheral Clock 45 Disable */
#define PMC_PCDR1_PID46 (0x1u << 14) /**< \brief (PMC_PCDR1) Peripheral Clock 46 Disable */
#define PMC_PCDR1_PID47 (0x1u << 15) /**< \brief (PMC_PCDR1) Peripheral Clock 47 Disable */
#define PMC_PCDR1_PID48 (0x1u << 16) /**< \brief (PMC_PCDR1) Peripheral Clock 48 Disable */
#define PMC_PCDR1_PID49 (0x1u << 17) /**< \brief (PMC_PCDR1) Peripheral Clock 49 Disable */
#define PMC_PCDR1_PID50 (0x1u << 18) /**< \brief (PMC_PCDR1) Peripheral Clock 50 Disable */
#define PMC_PCDR1_PID51 (0x1u << 19) /**< \brief (PMC_PCDR1) Peripheral Clock 51 Disable */
#define PMC_PCDR1_PID52 (0x1u << 20) /**< \brief (PMC_PCDR1) Peripheral Clock 52 Disable */
#define PMC_PCDR1_PID53 (0x1u << 21) /**< \brief (PMC_PCDR1) Peripheral Clock 53 Disable */
#define PMC_PCDR1_PID56 (0x1u << 24) /**< \brief (PMC_PCDR1) Peripheral Clock 56 Disable */
#define PMC_PCDR1_PID57 (0x1u << 25) /**< \brief (PMC_PCDR1) Peripheral Clock 57 Disable */
#define PMC_PCDR1_PID58 (0x1u << 26) /**< \brief (PMC_PCDR1) Peripheral Clock 58 Disable */
#define PMC_PCDR1_PID59 (0x1u << 27) /**< \brief (PMC_PCDR1) Peripheral Clock 59 Disable */
#define PMC_PCDR1_PID60 (0x1u << 28) /**< \brief (PMC_PCDR1) Peripheral Clock 60 Disable */
/* -------- PMC_PCSR1 : (PMC Offset: 0x0108) Peripheral Clock Status Register 1 -------- */
#define PMC_PCSR1_PID32 (0x1u << 0) /**< \brief (PMC_PCSR1) Peripheral Clock 32 Status */
#define PMC_PCSR1_PID33 (0x1u << 1) /**< \brief (PMC_PCSR1) Peripheral Clock 33 Status */
#define PMC_PCSR1_PID34 (0x1u << 2) /**< \brief (PMC_PCSR1) Peripheral Clock 34 Status */
#define PMC_PCSR1_PID35 (0x1u << 3) /**< \brief (PMC_PCSR1) Peripheral Clock 35 Status */
#define PMC_PCSR1_PID37 (0x1u << 5) /**< \brief (PMC_PCSR1) Peripheral Clock 37 Status */
#define PMC_PCSR1_PID39 (0x1u << 7) /**< \brief (PMC_PCSR1) Peripheral Clock 39 Status */
#define PMC_PCSR1_PID40 (0x1u << 8) /**< \brief (PMC_PCSR1) Peripheral Clock 40 Status */
#define PMC_PCSR1_PID41 (0x1u << 9) /**< \brief (PMC_PCSR1) Peripheral Clock 41 Status */
#define PMC_PCSR1_PID42 (0x1u << 10) /**< \brief (PMC_PCSR1) Peripheral Clock 42 Status */
#define PMC_PCSR1_PID43 (0x1u << 11) /**< \brief (PMC_PCSR1) Peripheral Clock 43 Status */
#define PMC_PCSR1_PID44 (0x1u << 12) /**< \brief (PMC_PCSR1) Peripheral Clock 44 Status */
#define PMC_PCSR1_PID45 (0x1u << 13) /**< \brief (PMC_PCSR1) Peripheral Clock 45 Status */
#define PMC_PCSR1_PID46 (0x1u << 14) /**< \brief (PMC_PCSR1) Peripheral Clock 46 Status */
#define PMC_PCSR1_PID47 (0x1u << 15) /**< \brief (PMC_PCSR1) Peripheral Clock 47 Status */
#define PMC_PCSR1_PID48 (0x1u << 16) /**< \brief (PMC_PCSR1) Peripheral Clock 48 Status */
#define PMC_PCSR1_PID49 (0x1u << 17) /**< \brief (PMC_PCSR1) Peripheral Clock 49 Status */
#define PMC_PCSR1_PID50 (0x1u << 18) /**< \brief (PMC_PCSR1) Peripheral Clock 50 Status */
#define PMC_PCSR1_PID51 (0x1u << 19) /**< \brief (PMC_PCSR1) Peripheral Clock 51 Status */
#define PMC_PCSR1_PID52 (0x1u << 20) /**< \brief (PMC_PCSR1) Peripheral Clock 52 Status */
#define PMC_PCSR1_PID53 (0x1u << 21) /**< \brief (PMC_PCSR1) Peripheral Clock 53 Status */
#define PMC_PCSR1_PID56 (0x1u << 24) /**< \brief (PMC_PCSR1) Peripheral Clock 56 Status */
#define PMC_PCSR1_PID57 (0x1u << 25) /**< \brief (PMC_PCSR1) Peripheral Clock 57 Status */
#define PMC_PCSR1_PID58 (0x1u << 26) /**< \brief (PMC_PCSR1) Peripheral Clock 58 Status */
#define PMC_PCSR1_PID59 (0x1u << 27) /**< \brief (PMC_PCSR1) Peripheral Clock 59 Status */
#define PMC_PCSR1_PID60 (0x1u << 28) /**< \brief (PMC_PCSR1) Peripheral Clock 60 Status */
/* -------- PMC_PCR : (PMC Offset: 0x010C) Peripheral Control Register -------- */
#define PMC_PCR_PID_Pos 0
#define PMC_PCR_PID_Msk (0x7fu << PMC_PCR_PID_Pos) /**< \brief (PMC_PCR) Peripheral ID */
#define PMC_PCR_PID(value) ((PMC_PCR_PID_Msk & ((value) << PMC_PCR_PID_Pos)))
#define PMC_PCR_GCLKCSS_Pos 8
#define PMC_PCR_GCLKCSS_Msk (0x7u << PMC_PCR_GCLKCSS_Pos) /**< \brief (PMC_PCR) Generic Clock Source Selection */
#define PMC_PCR_GCLKCSS(value) ((PMC_PCR_GCLKCSS_Msk & ((value) << PMC_PCR_GCLKCSS_Pos)))
#define   PMC_PCR_GCLKCSS_SLOW_CLK (0x0u << 8) /**< \brief (PMC_PCR) Slow clock is selected */
#define   PMC_PCR_GCLKCSS_MAIN_CLK (0x1u << 8) /**< \brief (PMC_PCR) Main clock is selected */
#define   PMC_PCR_GCLKCSS_PLLA_CLK (0x2u << 8) /**< \brief (PMC_PCR) PLLACK is selected */
#define   PMC_PCR_GCLKCSS_UPLL_CLK (0x3u << 8) /**< \brief (PMC_PCR) UPLL Clock is selected */
#define   PMC_PCR_GCLKCSS_MCK_CLK (0x4u << 8) /**< \brief (PMC_PCR) Master Clock is selected */
#define PMC_PCR_CMD (0x1u << 12) /**< \brief (PMC_PCR) Command */
#define PMC_PCR_DIV_Pos 16
#define PMC_PCR_DIV_Msk (0x3u << PMC_PCR_DIV_Pos) /**< \brief (PMC_PCR) Divisor Value */
#define PMC_PCR_DIV(value) ((PMC_PCR_DIV_Msk & ((value) << PMC_PCR_DIV_Pos)))
#define   PMC_PCR_DIV_PERIPH_DIV_MCK (0x0u << 16) /**< \brief (PMC_PCR) Peripheral clock is MCK */
#define   PMC_PCR_DIV_PERIPH_DIV2_MCK (0x1u << 16) /**< \brief (PMC_PCR) Peripheral clock is MCK/2 */
#define   PMC_PCR_DIV_PERIPH_DIV4_MCK (0x2u << 16) /**< \brief (PMC_PCR) Peripheral clock is MCK/4 */
#define   PMC_PCR_DIV_PERIPH_DIV8_MCK (0x3u << 16) /**< \brief (PMC_PCR) Peripheral clock is MCK/8 */
#define PMC_PCR_GCLKDIV_Pos 20
#define PMC_PCR_GCLKDIV_Msk (0xffu << PMC_PCR_GCLKDIV_Pos) /**< \brief (PMC_PCR) Generic Clock Division Ratio */
#define PMC_PCR_GCLKDIV(value) ((PMC_PCR_GCLKDIV_Msk & ((value) << PMC_PCR_GCLKDIV_Pos)))
#define PMC_PCR_EN (0x1u << 28) /**< \brief (PMC_PCR) Enable */
#define PMC_PCR_GCLKEN (0x1u << 29) /**< \brief (PMC_PCR) Generic Clock Enable */
/* -------- PMC_OCR : (PMC Offset: 0x0110) Oscillator Calibration Register -------- */
#define PMC_OCR_CAL4_Pos 0
#define PMC_OCR_CAL4_Msk (0x7fu << PMC_OCR_CAL4_Pos) /**< \brief (PMC_OCR) RC Oscillator Calibration bits for 4 MHz */
#define PMC_OCR_CAL4(value) ((PMC_OCR_CAL4_Msk & ((value) << PMC_OCR_CAL4_Pos)))
#define PMC_OCR_SEL4 (0x1u << 7) /**< \brief (PMC_OCR) Selection of RC Oscillator Calibration bits for 4 MHz */
#define PMC_OCR_CAL8_Pos 8
#define PMC_OCR_CAL8_Msk (0x7fu << PMC_OCR_CAL8_Pos) /**< \brief (PMC_OCR) RC Oscillator Calibration bits for 8 MHz */
#define PMC_OCR_CAL8(value) ((PMC_OCR_CAL8_Msk & ((value) << PMC_OCR_CAL8_Pos)))
#define PMC_OCR_SEL8 (0x1u << 15) /**< \brief (PMC_OCR) Selection of RC Oscillator Calibration bits for 8 MHz */
#define PMC_OCR_CAL12_Pos 16
#define PMC_OCR_CAL12_Msk (0x7fu << PMC_OCR_CAL12_Pos) /**< \brief (PMC_OCR) RC Oscillator Calibration bits for 12 MHz */
#define PMC_OCR_CAL12(value) ((PMC_OCR_CAL12_Msk & ((value) << PMC_OCR_CAL12_Pos)))
#define PMC_OCR_SEL12 (0x1u << 23) /**< \brief (PMC_OCR) Selection of RC Oscillator Calibration bits for 12 MHz */
/* -------- PMC_SLPWK_ER0 : (PMC Offset: 0x0114) SleepWalking Enable Register 0 -------- */
#define PMC_SLPWK_ER0_PID7 (0x1u << 7) /**< \brief (PMC_SLPWK_ER0) Peripheral 7 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID8 (0x1u << 8) /**< \brief (PMC_SLPWK_ER0) Peripheral 8 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID9 (0x1u << 9) /**< \brief (PMC_SLPWK_ER0) Peripheral 9 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID10 (0x1u << 10) /**< \brief (PMC_SLPWK_ER0) Peripheral 10 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID11 (0x1u << 11) /**< \brief (PMC_SLPWK_ER0) Peripheral 11 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID12 (0x1u << 12) /**< \brief (PMC_SLPWK_ER0) Peripheral 12 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID13 (0x1u << 13) /**< \brief (PMC_SLPWK_ER0) Peripheral 13 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID14 (0x1u << 14) /**< \brief (PMC_SLPWK_ER0) Peripheral 14 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID15 (0x1u << 15) /**< \brief (PMC_SLPWK_ER0) Peripheral 15 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID16 (0x1u << 16) /**< \brief (PMC_SLPWK_ER0) Peripheral 16 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID17 (0x1u << 17) /**< \brief (PMC_SLPWK_ER0) Peripheral 17 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID18 (0x1u << 18) /**< \brief (PMC_SLPWK_ER0) Peripheral 18 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID19 (0x1u << 19) /**< \brief (PMC_SLPWK_ER0) Peripheral 19 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID20 (0x1u << 20) /**< \brief (PMC_SLPWK_ER0) Peripheral 20 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID21 (0x1u << 21) /**< \brief (PMC_SLPWK_ER0) Peripheral 21 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID22 (0x1u << 22) /**< \brief (PMC_SLPWK_ER0) Peripheral 22 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID23 (0x1u << 23) /**< \brief (PMC_SLPWK_ER0) Peripheral 23 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID24 (0x1u << 24) /**< \brief (PMC_SLPWK_ER0) Peripheral 24 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID25 (0x1u << 25) /**< \brief (PMC_SLPWK_ER0) Peripheral 25 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID26 (0x1u << 26) /**< \brief (PMC_SLPWK_ER0) Peripheral 26 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID27 (0x1u << 27) /**< \brief (PMC_SLPWK_ER0) Peripheral 27 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID28 (0x1u << 28) /**< \brief (PMC_SLPWK_ER0) Peripheral 28 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID29 (0x1u << 29) /**< \brief (PMC_SLPWK_ER0) Peripheral 29 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID30 (0x1u << 30) /**< \brief (PMC_SLPWK_ER0) Peripheral 30 SleepWalking Enable */
#define PMC_SLPWK_ER0_PID31 (0x1u << 31) /**< \brief (PMC_SLPWK_ER0) Peripheral 31 SleepWalking Enable */
/* -------- PMC_SLPWK_DR0 : (PMC Offset: 0x0118) SleepWalking Disable Register 0 -------- */
#define PMC_SLPWK_DR0_PID7 (0x1u << 7) /**< \brief (PMC_SLPWK_DR0) Peripheral 7 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID8 (0x1u << 8) /**< \brief (PMC_SLPWK_DR0) Peripheral 8 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID9 (0x1u << 9) /**< \brief (PMC_SLPWK_DR0) Peripheral 9 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID10 (0x1u << 10) /**< \brief (PMC_SLPWK_DR0) Peripheral 10 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID11 (0x1u << 11) /**< \brief (PMC_SLPWK_DR0) Peripheral 11 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID12 (0x1u << 12) /**< \brief (PMC_SLPWK_DR0) Peripheral 12 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID13 (0x1u << 13) /**< \brief (PMC_SLPWK_DR0) Peripheral 13 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID14 (0x1u << 14) /**< \brief (PMC_SLPWK_DR0) Peripheral 14 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID15 (0x1u << 15) /**< \brief (PMC_SLPWK_DR0) Peripheral 15 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID16 (0x1u << 16) /**< \brief (PMC_SLPWK_DR0) Peripheral 16 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID17 (0x1u << 17) /**< \brief (PMC_SLPWK_DR0) Peripheral 17 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID18 (0x1u << 18) /**< \brief (PMC_SLPWK_DR0) Peripheral 18 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID19 (0x1u << 19) /**< \brief (PMC_SLPWK_DR0) Peripheral 19 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID20 (0x1u << 20) /**< \brief (PMC_SLPWK_DR0) Peripheral 20 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID21 (0x1u << 21) /**< \brief (PMC_SLPWK_DR0) Peripheral 21 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID22 (0x1u << 22) /**< \brief (PMC_SLPWK_DR0) Peripheral 22 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID23 (0x1u << 23) /**< \brief (PMC_SLPWK_DR0) Peripheral 23 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID24 (0x1u << 24) /**< \brief (PMC_SLPWK_DR0) Peripheral 24 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID25 (0x1u << 25) /**< \brief (PMC_SLPWK_DR0) Peripheral 25 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID26 (0x1u << 26) /**< \brief (PMC_SLPWK_DR0) Peripheral 26 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID27 (0x1u << 27) /**< \brief (PMC_SLPWK_DR0) Peripheral 27 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID28 (0x1u << 28) /**< \brief (PMC_SLPWK_DR0) Peripheral 28 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID29 (0x1u << 29) /**< \brief (PMC_SLPWK_DR0) Peripheral 29 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID30 (0x1u << 30) /**< \brief (PMC_SLPWK_DR0) Peripheral 30 SleepWalking Disable */
#define PMC_SLPWK_DR0_PID31 (0x1u << 31) /**< \brief (PMC_SLPWK_DR0) Peripheral 31 SleepWalking Disable */
/* -------- PMC_SLPWK_SR0 : (PMC Offset: 0x011C) SleepWalking Status Register 0 -------- */
#define PMC_SLPWK_SR0_PID7 (0x1u << 7) /**< \brief (PMC_SLPWK_SR0) Peripheral 7 SleepWalking Status */
#define PMC_SLPWK_SR0_PID8 (0x1u << 8) /**< \brief (PMC_SLPWK_SR0) Peripheral 8 SleepWalking Status */
#define PMC_SLPWK_SR0_PID9 (0x1u << 9) /**< \brief (PMC_SLPWK_SR0) Peripheral 9 SleepWalking Status */
#define PMC_SLPWK_SR0_PID10 (0x1u << 10) /**< \brief (PMC_SLPWK_SR0) Peripheral 10 SleepWalking Status */
#define PMC_SLPWK_SR0_PID11 (0x1u << 11) /**< \brief (PMC_SLPWK_SR0) Peripheral 11 SleepWalking Status */
#define PMC_SLPWK_SR0_PID12 (0x1u << 12) /**< \brief (PMC_SLPWK_SR0) Peripheral 12 SleepWalking Status */
#define PMC_SLPWK_SR0_PID13 (0x1u << 13) /**< \brief (PMC_SLPWK_SR0) Peripheral 13 SleepWalking Status */
#define PMC_SLPWK_SR0_PID14 (0x1u << 14) /**< \brief (PMC_SLPWK_SR0) Peripheral 14 SleepWalking Status */
#define PMC_SLPWK_SR0_PID15 (0x1u << 15) /**< \brief (PMC_SLPWK_SR0) Peripheral 15 SleepWalking Status */
#define PMC_SLPWK_SR0_PID16 (0x1u << 16) /**< \brief (PMC_SLPWK_SR0) Peripheral 16 SleepWalking Status */
#define PMC_SLPWK_SR0_PID17 (0x1u << 17) /**< \brief (PMC_SLPWK_SR0) Peripheral 17 SleepWalking Status */
#define PMC_SLPWK_SR0_PID18 (0x1u << 18) /**< \brief (PMC_SLPWK_SR0) Peripheral 18 SleepWalking Status */
#define PMC_SLPWK_SR0_PID19 (0x1u << 19) /**< \brief (PMC_SLPWK_SR0) Peripheral 19 SleepWalking Status */
#define PMC_SLPWK_SR0_PID20 (0x1u << 20) /**< \brief (PMC_SLPWK_SR0) Peripheral 20 SleepWalking Status */
#define PMC_SLPWK_SR0_PID21 (0x1u << 21) /**< \brief (PMC_SLPWK_SR0) Peripheral 21 SleepWalking Status */
#define PMC_SLPWK_SR0_PID22 (0x1u << 22) /**< \brief (PMC_SLPWK_SR0) Peripheral 22 SleepWalking Status */
#define PMC_SLPWK_SR0_PID23 (0x1u << 23) /**< \brief (PMC_SLPWK_SR0) Peripheral 23 SleepWalking Status */
#define PMC_SLPWK_SR0_PID24 (0x1u << 24) /**< \brief (PMC_SLPWK_SR0) Peripheral 24 SleepWalking Status */
#define PMC_SLPWK_SR0_PID25 (0x1u << 25) /**< \brief (PMC_SLPWK_SR0) Peripheral 25 SleepWalking Status */
#define PMC_SLPWK_SR0_PID26 (0x1u << 26) /**< \brief (PMC_SLPWK_SR0) Peripheral 26 SleepWalking Status */
#define PMC_SLPWK_SR0_PID27 (0x1u << 27) /**< \brief (PMC_SLPWK_SR0) Peripheral 27 SleepWalking Status */
#define PMC_SLPWK_SR0_PID28 (0x1u << 28) /**< \brief (PMC_SLPWK_SR0) Peripheral 28 SleepWalking Status */
#define PMC_SLPWK_SR0_PID29 (0x1u << 29) /**< \brief (PMC_SLPWK_SR0) Peripheral 29 SleepWalking Status */
#define PMC_SLPWK_SR0_PID30 (0x1u << 30) /**< \brief (PMC_SLPWK_SR0) Peripheral 30 SleepWalking Status */
#define PMC_SLPWK_SR0_PID31 (0x1u << 31) /**< \brief (PMC_SLPWK_SR0) Peripheral 31 SleepWalking Status */
/* -------- PMC_SLPWK_ASR0 : (PMC Offset: 0x0120) SleepWalking Activity Status Register 0 -------- */
#define PMC_SLPWK_ASR0_PID7 (0x1u << 7) /**< \brief (PMC_SLPWK_ASR0) Peripheral 7 Activity Status */
#define PMC_SLPWK_ASR0_PID8 (0x1u << 8) /**< \brief (PMC_SLPWK_ASR0) Peripheral 8 Activity Status */
#define PMC_SLPWK_ASR0_PID9 (0x1u << 9) /**< \brief (PMC_SLPWK_ASR0) Peripheral 9 Activity Status */
#define PMC_SLPWK_ASR0_PID10 (0x1u << 10) /**< \brief (PMC_SLPWK_ASR0) Peripheral 10 Activity Status */
#define PMC_SLPWK_ASR0_PID11 (0x1u << 11) /**< \brief (PMC_SLPWK_ASR0) Peripheral 11 Activity Status */
#define PMC_SLPWK_ASR0_PID12 (0x1u << 12) /**< \brief (PMC_SLPWK_ASR0) Peripheral 12 Activity Status */
#define PMC_SLPWK_ASR0_PID13 (0x1u << 13) /**< \brief (PMC_SLPWK_ASR0) Peripheral 13 Activity Status */
#define PMC_SLPWK_ASR0_PID14 (0x1u << 14) /**< \brief (PMC_SLPWK_ASR0) Peripheral 14 Activity Status */
#define PMC_SLPWK_ASR0_PID15 (0x1u << 15) /**< \brief (PMC_SLPWK_ASR0) Peripheral 15 Activity Status */
#define PMC_SLPWK_ASR0_PID16 (0x1u << 16) /**< \brief (PMC_SLPWK_ASR0) Peripheral 16 Activity Status */
#define PMC_SLPWK_ASR0_PID17 (0x1u << 17) /**< \brief (PMC_SLPWK_ASR0) Peripheral 17 Activity Status */
#define PMC_SLPWK_ASR0_PID18 (0x1u << 18) /**< \brief (PMC_SLPWK_ASR0) Peripheral 18 Activity Status */
#define PMC_SLPWK_ASR0_PID19 (0x1u << 19) /**< \brief (PMC_SLPWK_ASR0) Peripheral 19 Activity Status */
#define PMC_SLPWK_ASR0_PID20 (0x1u << 20) /**< \brief (PMC_SLPWK_ASR0) Peripheral 20 Activity Status */
#define PMC_SLPWK_ASR0_PID21 (0x1u << 21) /**< \brief (PMC_SLPWK_ASR0) Peripheral 21 Activity Status */
#define PMC_SLPWK_ASR0_PID22 (0x1u << 22) /**< \brief (PMC_SLPWK_ASR0) Peripheral 22 Activity Status */
#define PMC_SLPWK_ASR0_PID23 (0x1u << 23) /**< \brief (PMC_SLPWK_ASR0) Peripheral 23 Activity Status */
#define PMC_SLPWK_ASR0_PID24 (0x1u << 24) /**< \brief (PMC_SLPWK_ASR0) Peripheral 24 Activity Status */
#define PMC_SLPWK_ASR0_PID25 (0x1u << 25) /**< \brief (PMC_SLPWK_ASR0) Peripheral 25 Activity Status */
#define PMC_SLPWK_ASR0_PID26 (0x1u << 26) /**< \brief (PMC_SLPWK_ASR0) Peripheral 26 Activity Status */
#define PMC_SLPWK_ASR0_PID27 (0x1u << 27) /**< \brief (PMC_SLPWK_ASR0) Peripheral 27 Activity Status */
#define PMC_SLPWK_ASR0_PID28 (0x1u << 28) /**< \brief (PMC_SLPWK_ASR0) Peripheral 28 Activity Status */
#define PMC_SLPWK_ASR0_PID29 (0x1u << 29) /**< \brief (PMC_SLPWK_ASR0) Peripheral 29 Activity Status */
#define PMC_SLPWK_ASR0_PID30 (0x1u << 30) /**< \brief (PMC_SLPWK_ASR0) Peripheral 30 Activity Status */
#define PMC_SLPWK_ASR0_PID31 (0x1u << 31) /**< \brief (PMC_SLPWK_ASR0) Peripheral 31 Activity Status */
/* -------- PMC_PMMR : (PMC Offset: 0x0130) PLL Maximum Multiplier Value Register -------- */
#define PMC_PMMR_PLLA_MMAX_Pos 0
#define PMC_PMMR_PLLA_MMAX_Msk (0x7ffu << PMC_PMMR_PLLA_MMAX_Pos) /**< \brief (PMC_PMMR) PLLA Maximum Allowed Multiplier Value */
#define PMC_PMMR_PLLA_MMAX(value) ((PMC_PMMR_PLLA_MMAX_Msk & ((value) << PMC_PMMR_PLLA_MMAX_Pos)))
/* -------- PMC_SLPWK_ER1 : (PMC Offset: 0x0134) SleepWalking Enable Register 1 -------- */
#define PMC_SLPWK_ER1_PID32 (0x1u << 0) /**< \brief (PMC_SLPWK_ER1) Peripheral 32 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID33 (0x1u << 1) /**< \brief (PMC_SLPWK_ER1) Peripheral 33 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID34 (0x1u << 2) /**< \brief (PMC_SLPWK_ER1) Peripheral 34 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID35 (0x1u << 3) /**< \brief (PMC_SLPWK_ER1) Peripheral 35 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID37 (0x1u << 5) /**< \brief (PMC_SLPWK_ER1) Peripheral 37 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID39 (0x1u << 7) /**< \brief (PMC_SLPWK_ER1) Peripheral 39 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID40 (0x1u << 8) /**< \brief (PMC_SLPWK_ER1) Peripheral 40 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID41 (0x1u << 9) /**< \brief (PMC_SLPWK_ER1) Peripheral 41 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID42 (0x1u << 10) /**< \brief (PMC_SLPWK_ER1) Peripheral 42 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID43 (0x1u << 11) /**< \brief (PMC_SLPWK_ER1) Peripheral 43 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID44 (0x1u << 12) /**< \brief (PMC_SLPWK_ER1) Peripheral 44 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID45 (0x1u << 13) /**< \brief (PMC_SLPWK_ER1) Peripheral 45 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID46 (0x1u << 14) /**< \brief (PMC_SLPWK_ER1) Peripheral 46 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID47 (0x1u << 15) /**< \brief (PMC_SLPWK_ER1) Peripheral 47 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID48 (0x1u << 16) /**< \brief (PMC_SLPWK_ER1) Peripheral 48 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID49 (0x1u << 17) /**< \brief (PMC_SLPWK_ER1) Peripheral 49 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID50 (0x1u << 18) /**< \brief (PMC_SLPWK_ER1) Peripheral 50 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID51 (0x1u << 19) /**< \brief (PMC_SLPWK_ER1) Peripheral 51 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID52 (0x1u << 20) /**< \brief (PMC_SLPWK_ER1) Peripheral 52 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID53 (0x1u << 21) /**< \brief (PMC_SLPWK_ER1) Peripheral 53 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID56 (0x1u << 24) /**< \brief (PMC_SLPWK_ER1) Peripheral 56 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID57 (0x1u << 25) /**< \brief (PMC_SLPWK_ER1) Peripheral 57 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID58 (0x1u << 26) /**< \brief (PMC_SLPWK_ER1) Peripheral 58 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID59 (0x1u << 27) /**< \brief (PMC_SLPWK_ER1) Peripheral 59 SleepWalking Enable */
#define PMC_SLPWK_ER1_PID60 (0x1u << 28) /**< \brief (PMC_SLPWK_ER1) Peripheral 60 SleepWalking Enable */
/* -------- PMC_SLPWK_DR1 : (PMC Offset: 0x0138) SleepWalking Disable Register 1 -------- */
#define PMC_SLPWK_DR1_PID32 (0x1u << 0) /**< \brief (PMC_SLPWK_DR1) Peripheral 32 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID33 (0x1u << 1) /**< \brief (PMC_SLPWK_DR1) Peripheral 33 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID34 (0x1u << 2) /**< \brief (PMC_SLPWK_DR1) Peripheral 34 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID35 (0x1u << 3) /**< \brief (PMC_SLPWK_DR1) Peripheral 35 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID37 (0x1u << 5) /**< \brief (PMC_SLPWK_DR1) Peripheral 37 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID39 (0x1u << 7) /**< \brief (PMC_SLPWK_DR1) Peripheral 39 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID40 (0x1u << 8) /**< \brief (PMC_SLPWK_DR1) Peripheral 40 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID41 (0x1u << 9) /**< \brief (PMC_SLPWK_DR1) Peripheral 41 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID42 (0x1u << 10) /**< \brief (PMC_SLPWK_DR1) Peripheral 42 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID43 (0x1u << 11) /**< \brief (PMC_SLPWK_DR1) Peripheral 43 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID44 (0x1u << 12) /**< \brief (PMC_SLPWK_DR1) Peripheral 44 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID45 (0x1u << 13) /**< \brief (PMC_SLPWK_DR1) Peripheral 45 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID46 (0x1u << 14) /**< \brief (PMC_SLPWK_DR1) Peripheral 46 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID47 (0x1u << 15) /**< \brief (PMC_SLPWK_DR1) Peripheral 47 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID48 (0x1u << 16) /**< \brief (PMC_SLPWK_DR1) Peripheral 48 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID49 (0x1u << 17) /**< \brief (PMC_SLPWK_DR1) Peripheral 49 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID50 (0x1u << 18) /**< \brief (PMC_SLPWK_DR1) Peripheral 50 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID51 (0x1u << 19) /**< \brief (PMC_SLPWK_DR1) Peripheral 51 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID52 (0x1u << 20) /**< \brief (PMC_SLPWK_DR1) Peripheral 52 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID53 (0x1u << 21) /**< \brief (PMC_SLPWK_DR1) Peripheral 53 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID56 (0x1u << 24) /**< \brief (PMC_SLPWK_DR1) Peripheral 56 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID57 (0x1u << 25) /**< \brief (PMC_SLPWK_DR1) Peripheral 57 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID58 (0x1u << 26) /**< \brief (PMC_SLPWK_DR1) Peripheral 58 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID59 (0x1u << 27) /**< \brief (PMC_SLPWK_DR1) Peripheral 59 SleepWalking Disable */
#define PMC_SLPWK_DR1_PID60 (0x1u << 28) /**< \brief (PMC_SLPWK_DR1) Peripheral 60 SleepWalking Disable */
/* -------- PMC_SLPWK_SR1 : (PMC Offset: 0x013C) SleepWalking Status Register 1 -------- */
#define PMC_SLPWK_SR1_PID32 (0x1u << 0) /**< \brief (PMC_SLPWK_SR1) Peripheral 32 SleepWalking Status */
#define PMC_SLPWK_SR1_PID33 (0x1u << 1) /**< \brief (PMC_SLPWK_SR1) Peripheral 33 SleepWalking Status */
#define PMC_SLPWK_SR1_PID34 (0x1u << 2) /**< \brief (PMC_SLPWK_SR1) Peripheral 34 SleepWalking Status */
#define PMC_SLPWK_SR1_PID35 (0x1u << 3) /**< \brief (PMC_SLPWK_SR1) Peripheral 35 SleepWalking Status */
#define PMC_SLPWK_SR1_PID37 (0x1u << 5) /**< \brief (PMC_SLPWK_SR1) Peripheral 37 SleepWalking Status */
#define PMC_SLPWK_SR1_PID39 (0x1u << 7) /**< \brief (PMC_SLPWK_SR1) Peripheral 39 SleepWalking Status */
#define PMC_SLPWK_SR1_PID40 (0x1u << 8) /**< \brief (PMC_SLPWK_SR1) Peripheral 40 SleepWalking Status */
#define PMC_SLPWK_SR1_PID41 (0x1u << 9) /**< \brief (PMC_SLPWK_SR1) Peripheral 41 SleepWalking Status */
#define PMC_SLPWK_SR1_PID42 (0x1u << 10) /**< \brief (PMC_SLPWK_SR1) Peripheral 42 SleepWalking Status */
#define PMC_SLPWK_SR1_PID43 (0x1u << 11) /**< \brief (PMC_SLPWK_SR1) Peripheral 43 SleepWalking Status */
#define PMC_SLPWK_SR1_PID44 (0x1u << 12) /**< \brief (PMC_SLPWK_SR1) Peripheral 44 SleepWalking Status */
#define PMC_SLPWK_SR1_PID45 (0x1u << 13) /**< \brief (PMC_SLPWK_SR1) Peripheral 45 SleepWalking Status */
#define PMC_SLPWK_SR1_PID46 (0x1u << 14) /**< \brief (PMC_SLPWK_SR1) Peripheral 46 SleepWalking Status */
#define PMC_SLPWK_SR1_PID47 (0x1u << 15) /**< \brief (PMC_SLPWK_SR1) Peripheral 47 SleepWalking Status */
#define PMC_SLPWK_SR1_PID48 (0x1u << 16) /**< \brief (PMC_SLPWK_SR1) Peripheral 48 SleepWalking Status */
#define PMC_SLPWK_SR1_PID49 (0x1u << 17) /**< \brief (PMC_SLPWK_SR1) Peripheral 49 SleepWalking Status */
#define PMC_SLPWK_SR1_PID50 (0x1u << 18) /**< \brief (PMC_SLPWK_SR1) Peripheral 50 SleepWalking Status */
#define PMC_SLPWK_SR1_PID51 (0x1u << 19) /**< \brief (PMC_SLPWK_SR1) Peripheral 51 SleepWalking Status */
#define PMC_SLPWK_SR1_PID52 (0x1u << 20) /**< \brief (PMC_SLPWK_SR1) Peripheral 52 SleepWalking Status */
#define PMC_SLPWK_SR1_PID53 (0x1u << 21) /**< \brief (PMC_SLPWK_SR1) Peripheral 53 SleepWalking Status */
#define PMC_SLPWK_SR1_PID56 (0x1u << 24) /**< \brief (PMC_SLPWK_SR1) Peripheral 56 SleepWalking Status */
#define PMC_SLPWK_SR1_PID57 (0x1u << 25) /**< \brief (PMC_SLPWK_SR1) Peripheral 57 SleepWalking Status */
#define PMC_SLPWK_SR1_PID58 (0x1u << 26) /**< \brief (PMC_SLPWK_SR1) Peripheral 58 SleepWalking Status */
#define PMC_SLPWK_SR1_PID59 (0x1u << 27) /**< \brief (PMC_SLPWK_SR1) Peripheral 59 SleepWalking Status */
#define PMC_SLPWK_SR1_PID60 (0x1u << 28) /**< \brief (PMC_SLPWK_SR1) Peripheral 60 SleepWalking Status */
/* -------- PMC_SLPWK_ASR1 : (PMC Offset: 0x0140) SleepWalking Activity Status Register 1 -------- */
#define PMC_SLPWK_ASR1_PID32 (0x1u << 0) /**< \brief (PMC_SLPWK_ASR1) Peripheral 32 Activity Status */
#define PMC_SLPWK_ASR1_PID33 (0x1u << 1) /**< \brief (PMC_SLPWK_ASR1) Peripheral 33 Activity Status */
#define PMC_SLPWK_ASR1_PID34 (0x1u << 2) /**< \brief (PMC_SLPWK_ASR1) Peripheral 34 Activity Status */
#define PMC_SLPWK_ASR1_PID35 (0x1u << 3) /**< \brief (PMC_SLPWK_ASR1) Peripheral 35 Activity Status */
#define PMC_SLPWK_ASR1_PID37 (0x1u << 5) /**< \brief (PMC_SLPWK_ASR1) Peripheral 37 Activity Status */
#define PMC_SLPWK_ASR1_PID39 (0x1u << 7) /**< \brief (PMC_SLPWK_ASR1) Peripheral 39 Activity Status */
#define PMC_SLPWK_ASR1_PID40 (0x1u << 8) /**< \brief (PMC_SLPWK_ASR1) Peripheral 40 Activity Status */
#define PMC_SLPWK_ASR1_PID41 (0x1u << 9) /**< \brief (PMC_SLPWK_ASR1) Peripheral 41 Activity Status */
#define PMC_SLPWK_ASR1_PID42 (0x1u << 10) /**< \brief (PMC_SLPWK_ASR1) Peripheral 42 Activity Status */
#define PMC_SLPWK_ASR1_PID43 (0x1u << 11) /**< \brief (PMC_SLPWK_ASR1) Peripheral 43 Activity Status */
#define PMC_SLPWK_ASR1_PID44 (0x1u << 12) /**< \brief (PMC_SLPWK_ASR1) Peripheral 44 Activity Status */
#define PMC_SLPWK_ASR1_PID45 (0x1u << 13) /**< \brief (PMC_SLPWK_ASR1) Peripheral 45 Activity Status */
#define PMC_SLPWK_ASR1_PID46 (0x1u << 14) /**< \brief (PMC_SLPWK_ASR1) Peripheral 46 Activity Status */
#define PMC_SLPWK_ASR1_PID47 (0x1u << 15) /**< \brief (PMC_SLPWK_ASR1) Peripheral 47 Activity Status */
#define PMC_SLPWK_ASR1_PID48 (0x1u << 16) /**< \brief (PMC_SLPWK_ASR1) Peripheral 48 Activity Status */
#define PMC_SLPWK_ASR1_PID49 (0x1u << 17) /**< \brief (PMC_SLPWK_ASR1) Peripheral 49 Activity Status */
#define PMC_SLPWK_ASR1_PID50 (0x1u << 18) /**< \brief (PMC_SLPWK_ASR1) Peripheral 50 Activity Status */
#define PMC_SLPWK_ASR1_PID51 (0x1u << 19) /**< \brief (PMC_SLPWK_ASR1) Peripheral 51 Activity Status */
#define PMC_SLPWK_ASR1_PID52 (0x1u << 20) /**< \brief (PMC_SLPWK_ASR1) Peripheral 52 Activity Status */
#define PMC_SLPWK_ASR1_PID53 (0x1u << 21) /**< \brief (PMC_SLPWK_ASR1) Peripheral 53 Activity Status */
#define PMC_SLPWK_ASR1_PID56 (0x1u << 24) /**< \brief (PMC_SLPWK_ASR1) Peripheral 56 Activity Status */
#define PMC_SLPWK_ASR1_PID57 (0x1u << 25) /**< \brief (PMC_SLPWK_ASR1) Peripheral 57 Activity Status */
#define PMC_SLPWK_ASR1_PID58 (0x1u << 26) /**< \brief (PMC_SLPWK_ASR1) Peripheral 58 Activity Status */
#define PMC_SLPWK_ASR1_PID59 (0x1u << 27) /**< \brief (PMC_SLPWK_ASR1) Peripheral 59 Activity Status */
#define PMC_SLPWK_ASR1_PID60 (0x1u << 28) /**< \brief (PMC_SLPWK_ASR1) Peripheral 60 Activity Status */
/* -------- PMC_SLPWK_AIPR : (PMC Offset: 0x0144) SleepWalking Activity In Progress Register -------- */
#define PMC_SLPWK_AIPR_AIP (0x1u << 0) /**< \brief (PMC_SLPWK_AIPR) Activity In Progress */
/* -------- PMC_APLLACR : (PMC Offset: 0x0158) Audio PLL Analog Configuration Register -------- */
#define PMC_APLLACR_DCOFLTSEL_Pos 0
#define PMC_APLLACR_DCOFLTSEL_Msk (0xfu << PMC_APLLACR_DCOFLTSEL_Pos) /**< \brief (PMC_APLLACR) DCO Filter Selection */
#define PMC_APLLACR_DCOFLTSEL(value) ((PMC_APLLACR_DCOFLTSEL_Msk & ((value) << PMC_APLLACR_DCOFLTSEL_Pos)))
#define PMC_APLLACR_FLTSEL_Pos 4
#define PMC_APLLACR_FLTSEL_Msk (0xfu << PMC_APLLACR_FLTSEL_Pos) /**< \brief (PMC_APLLACR) PLL Filter Selection */
#define PMC_APLLACR_FLTSEL(value) ((PMC_APLLACR_FLTSEL_Msk & ((value) << PMC_APLLACR_FLTSEL_Pos)))
#define PMC_APLLACR_BIAS_Pos 8
#define PMC_APLLACR_BIAS_Msk (0x3u << PMC_APLLACR_BIAS_Pos) /**< \brief (PMC_APLLACR) Bias Voltage Selection */
#define PMC_APLLACR_BIAS(value) ((PMC_APLLACR_BIAS_Msk & ((value) << PMC_APLLACR_BIAS_Pos)))
/* -------- PMC_WMST : (PMC Offset: 0x015C) Wait Mode Startup Time Register -------- */
#define PMC_WMST_WMST_Pos 0
#define PMC_WMST_WMST_Msk (0xffu << PMC_WMST_WMST_Pos) /**< \brief (PMC_WMST) Wait Mode Startup Time */
#define PMC_WMST_WMST(value) ((PMC_WMST_WMST_Msk & ((value) << PMC_WMST_WMST_Pos)))
#define PMC_WMST_KEY_Pos 24
#define PMC_WMST_KEY_Msk (0xffu << PMC_WMST_KEY_Pos) /**< \brief (PMC_WMST) Write Access Password */
#define PMC_WMST_KEY(value) ((PMC_WMST_KEY_Msk & ((value) << PMC_WMST_KEY_Pos)))
#define   PMC_WMST_KEY_PASSWD (0x5Au << 24) /**< \brief (PMC_WMST) Writing any other value in this field aborts the write operation.Always reads as 0. */

/*@}*/


#endif /* _SAME70_PMC_COMPONENT_ */
