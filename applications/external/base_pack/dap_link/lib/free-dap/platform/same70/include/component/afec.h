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

#ifndef _SAME70_AFEC_COMPONENT_
#define _SAME70_AFEC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Analog Front-End Controller */
/* ============================================================================= */
/** \addtogroup SAME70_AFEC Analog Front-End Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Afec hardware registers */
typedef struct {
  __O  uint32_t AFEC_CR;       /**< \brief (Afec Offset: 0x00) AFEC Control Register */
  __IO uint32_t AFEC_MR;       /**< \brief (Afec Offset: 0x04) AFEC Mode Register */
  __IO uint32_t AFEC_EMR;      /**< \brief (Afec Offset: 0x08) AFEC Extended Mode Register */
  __IO uint32_t AFEC_SEQ1R;    /**< \brief (Afec Offset: 0x0C) AFEC Channel Sequence 1 Register */
  __IO uint32_t AFEC_SEQ2R;    /**< \brief (Afec Offset: 0x10) AFEC Channel Sequence 2 Register */
  __O  uint32_t AFEC_CHER;     /**< \brief (Afec Offset: 0x14) AFEC Channel Enable Register */
  __O  uint32_t AFEC_CHDR;     /**< \brief (Afec Offset: 0x18) AFEC Channel Disable Register */
  __I  uint32_t AFEC_CHSR;     /**< \brief (Afec Offset: 0x1C) AFEC Channel Status Register */
  __I  uint32_t AFEC_LCDR;     /**< \brief (Afec Offset: 0x20) AFEC Last Converted Data Register */
  __O  uint32_t AFEC_IER;      /**< \brief (Afec Offset: 0x24) AFEC Interrupt Enable Register */
  __O  uint32_t AFEC_IDR;      /**< \brief (Afec Offset: 0x28) AFEC Interrupt Disable Register */
  __I  uint32_t AFEC_IMR;      /**< \brief (Afec Offset: 0x2C) AFEC Interrupt Mask Register */
  __I  uint32_t AFEC_ISR;      /**< \brief (Afec Offset: 0x30) AFEC Interrupt Status Register */
  __I  uint32_t Reserved1[6];
  __I  uint32_t AFEC_OVER;     /**< \brief (Afec Offset: 0x4C) AFEC Overrun Status Register */
  __IO uint32_t AFEC_CWR;      /**< \brief (Afec Offset: 0x50) AFEC Compare Window Register */
  __IO uint32_t AFEC_CGR;      /**< \brief (Afec Offset: 0x54) AFEC Channel Gain Register */
  __I  uint32_t Reserved2[2];
  __IO uint32_t AFEC_DIFFR;    /**< \brief (Afec Offset: 0x60) AFEC Channel Differential Register */
  __IO uint32_t AFEC_CSELR;    /**< \brief (Afec Offset: 0x64) AFEC Channel Selection Register */
  __I  uint32_t AFEC_CDR;      /**< \brief (Afec Offset: 0x68) AFEC Channel Data Register */
  __IO uint32_t AFEC_COCR;     /**< \brief (Afec Offset: 0x6C) AFEC Channel Offset Compensation Register */
  __IO uint32_t AFEC_TEMPMR;   /**< \brief (Afec Offset: 0x70) AFEC Temperature Sensor Mode Register */
  __IO uint32_t AFEC_TEMPCWR;  /**< \brief (Afec Offset: 0x74) AFEC Temperature Compare Window Register */
  __I  uint32_t Reserved3[7];
  __IO uint32_t AFEC_ACR;      /**< \brief (Afec Offset: 0x94) AFEC Analog Control Register */
  __I  uint32_t Reserved4[2];
  __IO uint32_t AFEC_SHMR;     /**< \brief (Afec Offset: 0xA0) AFEC Sample & Hold Mode Register */
  __I  uint32_t Reserved5[11];
  __IO uint32_t AFEC_COSR;     /**< \brief (Afec Offset: 0xD0) AFEC Correction Select Register */
  __IO uint32_t AFEC_CVR;      /**< \brief (Afec Offset: 0xD4) AFEC Correction Values Register */
  __IO uint32_t AFEC_CECR;     /**< \brief (Afec Offset: 0xD8) AFEC Channel Error Correction Register */
  __I  uint32_t Reserved6[2];
  __IO uint32_t AFEC_WPMR;     /**< \brief (Afec Offset: 0xE4) AFEC Write Protection Mode Register */
  __I  uint32_t AFEC_WPSR;     /**< \brief (Afec Offset: 0xE8) AFEC Write Protection Status Register */
  __I  uint32_t Reserved7[4];
  __I  uint32_t AFEC_VERSION;  /**< \brief (Afec Offset: 0xFC) AFEC Version Register */
} Afec;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- AFEC_CR : (AFEC Offset: 0x00) AFEC Control Register -------- */
#define AFEC_CR_SWRST (0x1u << 0) /**< \brief (AFEC_CR) Software Reset */
#define AFEC_CR_START (0x1u << 1) /**< \brief (AFEC_CR) Start Conversion */
/* -------- AFEC_MR : (AFEC Offset: 0x04) AFEC Mode Register -------- */
#define AFEC_MR_TRGEN (0x1u << 0) /**< \brief (AFEC_MR) Trigger Enable */
#define   AFEC_MR_TRGEN_DIS (0x0u << 0) /**< \brief (AFEC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. */
#define   AFEC_MR_TRGEN_EN (0x1u << 0) /**< \brief (AFEC_MR) Hardware trigger selected by TRGSEL field is enabled. */
#define AFEC_MR_TRGSEL_Pos 1
#define AFEC_MR_TRGSEL_Msk (0x7u << AFEC_MR_TRGSEL_Pos) /**< \brief (AFEC_MR) Trigger Selection */
#define AFEC_MR_TRGSEL(value) ((AFEC_MR_TRGSEL_Msk & ((value) << AFEC_MR_TRGSEL_Pos)))
#define   AFEC_MR_TRGSEL_AFEC_TRIG0 (0x0u << 1) /**< \brief (AFEC_MR) AFE0_ADTRG for AFEC0 / AFE1_ADTRG for AFEC1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG1 (0x1u << 1) /**< \brief (AFEC_MR) TIOA Output of the Timer Counter Channel 0 for AFEC0/TIOA Output of the Timer Counter Channel 3 for AFEC1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG2 (0x2u << 1) /**< \brief (AFEC_MR) TIOA Output of the Timer Counter Channel 1 for AFEC0/TIOA Output of the Timer Counter Channel 4 for AFEC1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG3 (0x3u << 1) /**< \brief (AFEC_MR) TIOA Output of the Timer Counter Channel 2 for AFEC0/TIOA Output of the Timer Counter Channel 5 for AFEC1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG4 (0x4u << 1) /**< \brief (AFEC_MR) PWM0 event line 0 for AFEC0 / PWM1 event line 0 for AFEC1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG5 (0x5u << 1) /**< \brief (AFEC_MR) PWM0 event line 1 for AFEC0 / PWM1 event line 1 for AFEC1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG6 (0x6u << 1) /**< \brief (AFEC_MR) Analog Comparator */
#define AFEC_MR_SLEEP (0x1u << 5) /**< \brief (AFEC_MR) Sleep Mode */
#define   AFEC_MR_SLEEP_NORMAL (0x0u << 5) /**< \brief (AFEC_MR) Normal mode: The AFE and reference voltage circuitry are kept ON between conversions. */
#define   AFEC_MR_SLEEP_SLEEP (0x1u << 5) /**< \brief (AFEC_MR) Sleep mode: The AFE and reference voltage circuitry are OFF between conversions. */
#define AFEC_MR_FWUP (0x1u << 6) /**< \brief (AFEC_MR) Fast Wake-up */
#define   AFEC_MR_FWUP_OFF (0x0u << 6) /**< \brief (AFEC_MR) Normal Sleep mode: The sleep mode is defined by the SLEEP bit. */
#define   AFEC_MR_FWUP_ON (0x1u << 6) /**< \brief (AFEC_MR) Fast wake-up Sleep mode: The voltage reference is ON between conversions and AFE is OFF. */
#define AFEC_MR_FREERUN (0x1u << 7) /**< \brief (AFEC_MR) Free Run Mode */
#define   AFEC_MR_FREERUN_OFF (0x0u << 7) /**< \brief (AFEC_MR) Normal mode */
#define   AFEC_MR_FREERUN_ON (0x1u << 7) /**< \brief (AFEC_MR) Free Run mode: Never wait for any trigger. */
#define AFEC_MR_PRESCAL_Pos 8
#define AFEC_MR_PRESCAL_Msk (0xffu << AFEC_MR_PRESCAL_Pos) /**< \brief (AFEC_MR) Prescaler Rate Selection */
#define AFEC_MR_PRESCAL(value) ((AFEC_MR_PRESCAL_Msk & ((value) << AFEC_MR_PRESCAL_Pos)))
#define AFEC_MR_STARTUP_Pos 16
#define AFEC_MR_STARTUP_Msk (0xfu << AFEC_MR_STARTUP_Pos) /**< \brief (AFEC_MR) Start-up Time */
#define AFEC_MR_STARTUP(value) ((AFEC_MR_STARTUP_Msk & ((value) << AFEC_MR_STARTUP_Pos)))
#define   AFEC_MR_STARTUP_SUT0 (0x0u << 16) /**< \brief (AFEC_MR) 0 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT8 (0x1u << 16) /**< \brief (AFEC_MR) 8 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT16 (0x2u << 16) /**< \brief (AFEC_MR) 16 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT24 (0x3u << 16) /**< \brief (AFEC_MR) 24 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT64 (0x4u << 16) /**< \brief (AFEC_MR) 64 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT80 (0x5u << 16) /**< \brief (AFEC_MR) 80 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT96 (0x6u << 16) /**< \brief (AFEC_MR) 96 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT112 (0x7u << 16) /**< \brief (AFEC_MR) 112 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT512 (0x8u << 16) /**< \brief (AFEC_MR) 512 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT576 (0x9u << 16) /**< \brief (AFEC_MR) 576 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT640 (0xAu << 16) /**< \brief (AFEC_MR) 640 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT704 (0xBu << 16) /**< \brief (AFEC_MR) 704 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT768 (0xCu << 16) /**< \brief (AFEC_MR) 768 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT832 (0xDu << 16) /**< \brief (AFEC_MR) 832 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT896 (0xEu << 16) /**< \brief (AFEC_MR) 896 periods of AFE clock */
#define   AFEC_MR_STARTUP_SUT960 (0xFu << 16) /**< \brief (AFEC_MR) 960 periods of AFE clock */
#define AFEC_MR_ONE (0x1u << 23) /**< \brief (AFEC_MR) One */
#define AFEC_MR_TRACKTIM_Pos 24
#define AFEC_MR_TRACKTIM_Msk (0xfu << AFEC_MR_TRACKTIM_Pos) /**< \brief (AFEC_MR) Tracking Time */
#define AFEC_MR_TRACKTIM(value) ((AFEC_MR_TRACKTIM_Msk & ((value) << AFEC_MR_TRACKTIM_Pos)))
#define AFEC_MR_TRANSFER_Pos 28
#define AFEC_MR_TRANSFER_Msk (0x3u << AFEC_MR_TRANSFER_Pos) /**< \brief (AFEC_MR) Transfer Period */
#define AFEC_MR_TRANSFER(value) ((AFEC_MR_TRANSFER_Msk & ((value) << AFEC_MR_TRANSFER_Pos)))
#define AFEC_MR_USEQ (0x1u << 31) /**< \brief (AFEC_MR) User Sequence Enable */
#define   AFEC_MR_USEQ_NUM_ORDER (0x0u << 31) /**< \brief (AFEC_MR) Normal mode: The controller converts channels in a simple numeric order. */
#define   AFEC_MR_USEQ_REG_ORDER (0x1u << 31) /**< \brief (AFEC_MR) User Sequence mode: The sequence respects what is defined in AFEC_SEQ1R and AFEC_SEQ1R. */
/* -------- AFEC_EMR : (AFEC Offset: 0x08) AFEC Extended Mode Register -------- */
#define AFEC_EMR_CMPMODE_Pos 0
#define AFEC_EMR_CMPMODE_Msk (0x3u << AFEC_EMR_CMPMODE_Pos) /**< \brief (AFEC_EMR) Comparison Mode */
#define AFEC_EMR_CMPMODE(value) ((AFEC_EMR_CMPMODE_Msk & ((value) << AFEC_EMR_CMPMODE_Pos)))
#define   AFEC_EMR_CMPMODE_LOW (0x0u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is lower than the low threshold of the window. */
#define   AFEC_EMR_CMPMODE_HIGH (0x1u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is higher than the high threshold of the window. */
#define   AFEC_EMR_CMPMODE_IN (0x2u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is in the comparison window. */
#define   AFEC_EMR_CMPMODE_OUT (0x3u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is out of the comparison window. */
#define AFEC_EMR_CMPSEL_Pos 3
#define AFEC_EMR_CMPSEL_Msk (0x1fu << AFEC_EMR_CMPSEL_Pos) /**< \brief (AFEC_EMR) Comparison Selected Channel */
#define AFEC_EMR_CMPSEL(value) ((AFEC_EMR_CMPSEL_Msk & ((value) << AFEC_EMR_CMPSEL_Pos)))
#define AFEC_EMR_CMPALL (0x1u << 9) /**< \brief (AFEC_EMR) Compare All Channels */
#define AFEC_EMR_CMPFILTER_Pos 12
#define AFEC_EMR_CMPFILTER_Msk (0x3u << AFEC_EMR_CMPFILTER_Pos) /**< \brief (AFEC_EMR) Compare Event Filtering */
#define AFEC_EMR_CMPFILTER(value) ((AFEC_EMR_CMPFILTER_Msk & ((value) << AFEC_EMR_CMPFILTER_Pos)))
#define AFEC_EMR_RES_Pos 16
#define AFEC_EMR_RES_Msk (0x7u << AFEC_EMR_RES_Pos) /**< \brief (AFEC_EMR) Resolution */
#define AFEC_EMR_RES(value) ((AFEC_EMR_RES_Msk & ((value) << AFEC_EMR_RES_Pos)))
#define   AFEC_EMR_RES_NO_AVERAGE (0x0u << 16) /**< \brief (AFEC_EMR) 12-bit resolution, AFE sample rate is maximum (no averaging). */
#define   AFEC_EMR_RES_OSR4 (0x2u << 16) /**< \brief (AFEC_EMR) 13-bit resolution, AFE sample rate divided by 4 (averaging). */
#define   AFEC_EMR_RES_OSR16 (0x3u << 16) /**< \brief (AFEC_EMR) 14-bit resolution, AFE sample rate divided by 16 (averaging). */
#define   AFEC_EMR_RES_OSR64 (0x4u << 16) /**< \brief (AFEC_EMR) 15-bit resolution, AFE sample rate divided by 64 (averaging). */
#define   AFEC_EMR_RES_OSR256 (0x5u << 16) /**< \brief (AFEC_EMR) 16-bit resolution, AFE sample rate divided by 256 (averaging). */
#define AFEC_EMR_TAG (0x1u << 24) /**< \brief (AFEC_EMR) TAG of the AFEC_LDCR */
#define AFEC_EMR_STM (0x1u << 25) /**< \brief (AFEC_EMR) Single Trigger Mode */
#define AFEC_EMR_SIGNMODE_Pos 28
#define AFEC_EMR_SIGNMODE_Msk (0x3u << AFEC_EMR_SIGNMODE_Pos) /**< \brief (AFEC_EMR) Sign Mode */
#define AFEC_EMR_SIGNMODE(value) ((AFEC_EMR_SIGNMODE_Msk & ((value) << AFEC_EMR_SIGNMODE_Pos)))
#define   AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN (0x0u << 28) /**< \brief (AFEC_EMR) Single-Ended channels: Unsigned conversions.Differential channels: Signed conversions. */
#define   AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG (0x1u << 28) /**< \brief (AFEC_EMR) Single-Ended channels: Signed conversions.Differential channels: Unsigned conversions. */
#define   AFEC_EMR_SIGNMODE_ALL_UNSIGNED (0x2u << 28) /**< \brief (AFEC_EMR) All channels: Unsigned conversions. */
#define   AFEC_EMR_SIGNMODE_ALL_SIGNED (0x3u << 28) /**< \brief (AFEC_EMR) All channels: Signed conversions. */
/* -------- AFEC_SEQ1R : (AFEC Offset: 0x0C) AFEC Channel Sequence 1 Register -------- */
#define AFEC_SEQ1R_USCH0_Pos 0
#define AFEC_SEQ1R_USCH0_Msk (0xfu << AFEC_SEQ1R_USCH0_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 0 */
#define AFEC_SEQ1R_USCH0(value) ((AFEC_SEQ1R_USCH0_Msk & ((value) << AFEC_SEQ1R_USCH0_Pos)))
#define AFEC_SEQ1R_USCH1_Pos 4
#define AFEC_SEQ1R_USCH1_Msk (0xfu << AFEC_SEQ1R_USCH1_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 1 */
#define AFEC_SEQ1R_USCH1(value) ((AFEC_SEQ1R_USCH1_Msk & ((value) << AFEC_SEQ1R_USCH1_Pos)))
#define AFEC_SEQ1R_USCH2_Pos 8
#define AFEC_SEQ1R_USCH2_Msk (0xfu << AFEC_SEQ1R_USCH2_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 2 */
#define AFEC_SEQ1R_USCH2(value) ((AFEC_SEQ1R_USCH2_Msk & ((value) << AFEC_SEQ1R_USCH2_Pos)))
#define AFEC_SEQ1R_USCH3_Pos 12
#define AFEC_SEQ1R_USCH3_Msk (0xfu << AFEC_SEQ1R_USCH3_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 3 */
#define AFEC_SEQ1R_USCH3(value) ((AFEC_SEQ1R_USCH3_Msk & ((value) << AFEC_SEQ1R_USCH3_Pos)))
#define AFEC_SEQ1R_USCH4_Pos 16
#define AFEC_SEQ1R_USCH4_Msk (0xfu << AFEC_SEQ1R_USCH4_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 4 */
#define AFEC_SEQ1R_USCH4(value) ((AFEC_SEQ1R_USCH4_Msk & ((value) << AFEC_SEQ1R_USCH4_Pos)))
#define AFEC_SEQ1R_USCH5_Pos 20
#define AFEC_SEQ1R_USCH5_Msk (0xfu << AFEC_SEQ1R_USCH5_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 5 */
#define AFEC_SEQ1R_USCH5(value) ((AFEC_SEQ1R_USCH5_Msk & ((value) << AFEC_SEQ1R_USCH5_Pos)))
#define AFEC_SEQ1R_USCH6_Pos 24
#define AFEC_SEQ1R_USCH6_Msk (0xfu << AFEC_SEQ1R_USCH6_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 6 */
#define AFEC_SEQ1R_USCH6(value) ((AFEC_SEQ1R_USCH6_Msk & ((value) << AFEC_SEQ1R_USCH6_Pos)))
#define AFEC_SEQ1R_USCH7_Pos 28
#define AFEC_SEQ1R_USCH7_Msk (0xfu << AFEC_SEQ1R_USCH7_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 7 */
#define AFEC_SEQ1R_USCH7(value) ((AFEC_SEQ1R_USCH7_Msk & ((value) << AFEC_SEQ1R_USCH7_Pos)))
/* -------- AFEC_SEQ2R : (AFEC Offset: 0x10) AFEC Channel Sequence 2 Register -------- */
#define AFEC_SEQ2R_USCH8_Pos 0
#define AFEC_SEQ2R_USCH8_Msk (0xfu << AFEC_SEQ2R_USCH8_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 8 */
#define AFEC_SEQ2R_USCH8(value) ((AFEC_SEQ2R_USCH8_Msk & ((value) << AFEC_SEQ2R_USCH8_Pos)))
#define AFEC_SEQ2R_USCH9_Pos 4
#define AFEC_SEQ2R_USCH9_Msk (0xfu << AFEC_SEQ2R_USCH9_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 9 */
#define AFEC_SEQ2R_USCH9(value) ((AFEC_SEQ2R_USCH9_Msk & ((value) << AFEC_SEQ2R_USCH9_Pos)))
#define AFEC_SEQ2R_USCH10_Pos 8
#define AFEC_SEQ2R_USCH10_Msk (0xfu << AFEC_SEQ2R_USCH10_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 10 */
#define AFEC_SEQ2R_USCH10(value) ((AFEC_SEQ2R_USCH10_Msk & ((value) << AFEC_SEQ2R_USCH10_Pos)))
#define AFEC_SEQ2R_USCH11_Pos 12
#define AFEC_SEQ2R_USCH11_Msk (0xfu << AFEC_SEQ2R_USCH11_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 11 */
#define AFEC_SEQ2R_USCH11(value) ((AFEC_SEQ2R_USCH11_Msk & ((value) << AFEC_SEQ2R_USCH11_Pos)))
#define AFEC_SEQ2R_USCH12_Pos 16
#define AFEC_SEQ2R_USCH12_Msk (0xfu << AFEC_SEQ2R_USCH12_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 12 */
#define AFEC_SEQ2R_USCH12(value) ((AFEC_SEQ2R_USCH12_Msk & ((value) << AFEC_SEQ2R_USCH12_Pos)))
#define AFEC_SEQ2R_USCH13_Pos 20
#define AFEC_SEQ2R_USCH13_Msk (0xfu << AFEC_SEQ2R_USCH13_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 13 */
#define AFEC_SEQ2R_USCH13(value) ((AFEC_SEQ2R_USCH13_Msk & ((value) << AFEC_SEQ2R_USCH13_Pos)))
#define AFEC_SEQ2R_USCH14_Pos 24
#define AFEC_SEQ2R_USCH14_Msk (0xfu << AFEC_SEQ2R_USCH14_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 14 */
#define AFEC_SEQ2R_USCH14(value) ((AFEC_SEQ2R_USCH14_Msk & ((value) << AFEC_SEQ2R_USCH14_Pos)))
#define AFEC_SEQ2R_USCH15_Pos 28
#define AFEC_SEQ2R_USCH15_Msk (0xfu << AFEC_SEQ2R_USCH15_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 15 */
#define AFEC_SEQ2R_USCH15(value) ((AFEC_SEQ2R_USCH15_Msk & ((value) << AFEC_SEQ2R_USCH15_Pos)))
/* -------- AFEC_CHER : (AFEC Offset: 0x14) AFEC Channel Enable Register -------- */
#define AFEC_CHER_CH0 (0x1u << 0) /**< \brief (AFEC_CHER) Channel 0 Enable */
#define AFEC_CHER_CH1 (0x1u << 1) /**< \brief (AFEC_CHER) Channel 1 Enable */
#define AFEC_CHER_CH2 (0x1u << 2) /**< \brief (AFEC_CHER) Channel 2 Enable */
#define AFEC_CHER_CH3 (0x1u << 3) /**< \brief (AFEC_CHER) Channel 3 Enable */
#define AFEC_CHER_CH4 (0x1u << 4) /**< \brief (AFEC_CHER) Channel 4 Enable */
#define AFEC_CHER_CH5 (0x1u << 5) /**< \brief (AFEC_CHER) Channel 5 Enable */
#define AFEC_CHER_CH6 (0x1u << 6) /**< \brief (AFEC_CHER) Channel 6 Enable */
#define AFEC_CHER_CH7 (0x1u << 7) /**< \brief (AFEC_CHER) Channel 7 Enable */
#define AFEC_CHER_CH8 (0x1u << 8) /**< \brief (AFEC_CHER) Channel 8 Enable */
#define AFEC_CHER_CH9 (0x1u << 9) /**< \brief (AFEC_CHER) Channel 9 Enable */
#define AFEC_CHER_CH10 (0x1u << 10) /**< \brief (AFEC_CHER) Channel 10 Enable */
#define AFEC_CHER_CH11 (0x1u << 11) /**< \brief (AFEC_CHER) Channel 11 Enable */
/* -------- AFEC_CHDR : (AFEC Offset: 0x18) AFEC Channel Disable Register -------- */
#define AFEC_CHDR_CH0 (0x1u << 0) /**< \brief (AFEC_CHDR) Channel 0 Disable */
#define AFEC_CHDR_CH1 (0x1u << 1) /**< \brief (AFEC_CHDR) Channel 1 Disable */
#define AFEC_CHDR_CH2 (0x1u << 2) /**< \brief (AFEC_CHDR) Channel 2 Disable */
#define AFEC_CHDR_CH3 (0x1u << 3) /**< \brief (AFEC_CHDR) Channel 3 Disable */
#define AFEC_CHDR_CH4 (0x1u << 4) /**< \brief (AFEC_CHDR) Channel 4 Disable */
#define AFEC_CHDR_CH5 (0x1u << 5) /**< \brief (AFEC_CHDR) Channel 5 Disable */
#define AFEC_CHDR_CH6 (0x1u << 6) /**< \brief (AFEC_CHDR) Channel 6 Disable */
#define AFEC_CHDR_CH7 (0x1u << 7) /**< \brief (AFEC_CHDR) Channel 7 Disable */
#define AFEC_CHDR_CH8 (0x1u << 8) /**< \brief (AFEC_CHDR) Channel 8 Disable */
#define AFEC_CHDR_CH9 (0x1u << 9) /**< \brief (AFEC_CHDR) Channel 9 Disable */
#define AFEC_CHDR_CH10 (0x1u << 10) /**< \brief (AFEC_CHDR) Channel 10 Disable */
#define AFEC_CHDR_CH11 (0x1u << 11) /**< \brief (AFEC_CHDR) Channel 11 Disable */
/* -------- AFEC_CHSR : (AFEC Offset: 0x1C) AFEC Channel Status Register -------- */
#define AFEC_CHSR_CH0 (0x1u << 0) /**< \brief (AFEC_CHSR) Channel 0 Status */
#define AFEC_CHSR_CH1 (0x1u << 1) /**< \brief (AFEC_CHSR) Channel 1 Status */
#define AFEC_CHSR_CH2 (0x1u << 2) /**< \brief (AFEC_CHSR) Channel 2 Status */
#define AFEC_CHSR_CH3 (0x1u << 3) /**< \brief (AFEC_CHSR) Channel 3 Status */
#define AFEC_CHSR_CH4 (0x1u << 4) /**< \brief (AFEC_CHSR) Channel 4 Status */
#define AFEC_CHSR_CH5 (0x1u << 5) /**< \brief (AFEC_CHSR) Channel 5 Status */
#define AFEC_CHSR_CH6 (0x1u << 6) /**< \brief (AFEC_CHSR) Channel 6 Status */
#define AFEC_CHSR_CH7 (0x1u << 7) /**< \brief (AFEC_CHSR) Channel 7 Status */
#define AFEC_CHSR_CH8 (0x1u << 8) /**< \brief (AFEC_CHSR) Channel 8 Status */
#define AFEC_CHSR_CH9 (0x1u << 9) /**< \brief (AFEC_CHSR) Channel 9 Status */
#define AFEC_CHSR_CH10 (0x1u << 10) /**< \brief (AFEC_CHSR) Channel 10 Status */
#define AFEC_CHSR_CH11 (0x1u << 11) /**< \brief (AFEC_CHSR) Channel 11 Status */
/* -------- AFEC_LCDR : (AFEC Offset: 0x20) AFEC Last Converted Data Register -------- */
#define AFEC_LCDR_LDATA_Pos 0
#define AFEC_LCDR_LDATA_Msk (0xffffu << AFEC_LCDR_LDATA_Pos) /**< \brief (AFEC_LCDR) Last Data Converted */
#define AFEC_LCDR_CHNB_Pos 24
#define AFEC_LCDR_CHNB_Msk (0xfu << AFEC_LCDR_CHNB_Pos) /**< \brief (AFEC_LCDR) Channel Number */
/* -------- AFEC_IER : (AFEC Offset: 0x24) AFEC Interrupt Enable Register -------- */
#define AFEC_IER_EOC0 (0x1u << 0) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 0 */
#define AFEC_IER_EOC1 (0x1u << 1) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 1 */
#define AFEC_IER_EOC2 (0x1u << 2) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 2 */
#define AFEC_IER_EOC3 (0x1u << 3) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 3 */
#define AFEC_IER_EOC4 (0x1u << 4) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 4 */
#define AFEC_IER_EOC5 (0x1u << 5) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 5 */
#define AFEC_IER_EOC6 (0x1u << 6) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 6 */
#define AFEC_IER_EOC7 (0x1u << 7) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 7 */
#define AFEC_IER_EOC8 (0x1u << 8) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 8 */
#define AFEC_IER_EOC9 (0x1u << 9) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 9 */
#define AFEC_IER_EOC10 (0x1u << 10) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 10 */
#define AFEC_IER_EOC11 (0x1u << 11) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 11 */
#define AFEC_IER_DRDY (0x1u << 24) /**< \brief (AFEC_IER) Data Ready Interrupt Enable */
#define AFEC_IER_GOVRE (0x1u << 25) /**< \brief (AFEC_IER) General Overrun Error Interrupt Enable */
#define AFEC_IER_COMPE (0x1u << 26) /**< \brief (AFEC_IER) Comparison Event Interrupt Enable */
#define AFEC_IER_TEMPCHG (0x1u << 30) /**< \brief (AFEC_IER) Temperature Change Interrupt Enable */
/* -------- AFEC_IDR : (AFEC Offset: 0x28) AFEC Interrupt Disable Register -------- */
#define AFEC_IDR_EOC0 (0x1u << 0) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 0 */
#define AFEC_IDR_EOC1 (0x1u << 1) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 1 */
#define AFEC_IDR_EOC2 (0x1u << 2) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 2 */
#define AFEC_IDR_EOC3 (0x1u << 3) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 3 */
#define AFEC_IDR_EOC4 (0x1u << 4) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 4 */
#define AFEC_IDR_EOC5 (0x1u << 5) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 5 */
#define AFEC_IDR_EOC6 (0x1u << 6) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 6 */
#define AFEC_IDR_EOC7 (0x1u << 7) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 7 */
#define AFEC_IDR_EOC8 (0x1u << 8) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 8 */
#define AFEC_IDR_EOC9 (0x1u << 9) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 9 */
#define AFEC_IDR_EOC10 (0x1u << 10) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 10 */
#define AFEC_IDR_EOC11 (0x1u << 11) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 11 */
#define AFEC_IDR_DRDY (0x1u << 24) /**< \brief (AFEC_IDR) Data Ready Interrupt Disable */
#define AFEC_IDR_GOVRE (0x1u << 25) /**< \brief (AFEC_IDR) General Overrun Error Interrupt Disable */
#define AFEC_IDR_COMPE (0x1u << 26) /**< \brief (AFEC_IDR) Comparison Event Interrupt Disable */
#define AFEC_IDR_TEMPCHG (0x1u << 30) /**< \brief (AFEC_IDR) Temperature Change Interrupt Disable */
/* -------- AFEC_IMR : (AFEC Offset: 0x2C) AFEC Interrupt Mask Register -------- */
#define AFEC_IMR_EOC0 (0x1u << 0) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 0 */
#define AFEC_IMR_EOC1 (0x1u << 1) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 1 */
#define AFEC_IMR_EOC2 (0x1u << 2) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 2 */
#define AFEC_IMR_EOC3 (0x1u << 3) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 3 */
#define AFEC_IMR_EOC4 (0x1u << 4) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 4 */
#define AFEC_IMR_EOC5 (0x1u << 5) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 5 */
#define AFEC_IMR_EOC6 (0x1u << 6) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 6 */
#define AFEC_IMR_EOC7 (0x1u << 7) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 7 */
#define AFEC_IMR_EOC8 (0x1u << 8) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 8 */
#define AFEC_IMR_EOC9 (0x1u << 9) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 9 */
#define AFEC_IMR_EOC10 (0x1u << 10) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 10 */
#define AFEC_IMR_EOC11 (0x1u << 11) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 11 */
#define AFEC_IMR_DRDY (0x1u << 24) /**< \brief (AFEC_IMR) Data Ready Interrupt Mask */
#define AFEC_IMR_GOVRE (0x1u << 25) /**< \brief (AFEC_IMR) General Overrun Error Interrupt Mask */
#define AFEC_IMR_COMPE (0x1u << 26) /**< \brief (AFEC_IMR) Comparison Event Interrupt Mask */
#define AFEC_IMR_TEMPCHG (0x1u << 30) /**< \brief (AFEC_IMR) Temperature Change Interrupt Mask */
/* -------- AFEC_ISR : (AFEC Offset: 0x30) AFEC Interrupt Status Register -------- */
#define AFEC_ISR_EOC0 (0x1u << 0) /**< \brief (AFEC_ISR) End of Conversion 0 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC1 (0x1u << 1) /**< \brief (AFEC_ISR) End of Conversion 1 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC2 (0x1u << 2) /**< \brief (AFEC_ISR) End of Conversion 2 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC3 (0x1u << 3) /**< \brief (AFEC_ISR) End of Conversion 3 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC4 (0x1u << 4) /**< \brief (AFEC_ISR) End of Conversion 4 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC5 (0x1u << 5) /**< \brief (AFEC_ISR) End of Conversion 5 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC6 (0x1u << 6) /**< \brief (AFEC_ISR) End of Conversion 6 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC7 (0x1u << 7) /**< \brief (AFEC_ISR) End of Conversion 7 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC8 (0x1u << 8) /**< \brief (AFEC_ISR) End of Conversion 8 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC9 (0x1u << 9) /**< \brief (AFEC_ISR) End of Conversion 9 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC10 (0x1u << 10) /**< \brief (AFEC_ISR) End of Conversion 10 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC11 (0x1u << 11) /**< \brief (AFEC_ISR) End of Conversion 11 (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_DRDY (0x1u << 24) /**< \brief (AFEC_ISR) Data Ready (cleared by reading AFEC_LCDR) */
#define AFEC_ISR_GOVRE (0x1u << 25) /**< \brief (AFEC_ISR) General Overrun Error (cleared by reading AFEC_ISR) */
#define AFEC_ISR_COMPE (0x1u << 26) /**< \brief (AFEC_ISR) Comparison Error (cleared by reading AFEC_ISR) */
#define AFEC_ISR_TEMPCHG (0x1u << 30) /**< \brief (AFEC_ISR) Temperature Change (cleared on read) */
/* -------- AFEC_OVER : (AFEC Offset: 0x4C) AFEC Overrun Status Register -------- */
#define AFEC_OVER_OVRE0 (0x1u << 0) /**< \brief (AFEC_OVER) Overrun Error 0 */
#define AFEC_OVER_OVRE1 (0x1u << 1) /**< \brief (AFEC_OVER) Overrun Error 1 */
#define AFEC_OVER_OVRE2 (0x1u << 2) /**< \brief (AFEC_OVER) Overrun Error 2 */
#define AFEC_OVER_OVRE3 (0x1u << 3) /**< \brief (AFEC_OVER) Overrun Error 3 */
#define AFEC_OVER_OVRE4 (0x1u << 4) /**< \brief (AFEC_OVER) Overrun Error 4 */
#define AFEC_OVER_OVRE5 (0x1u << 5) /**< \brief (AFEC_OVER) Overrun Error 5 */
#define AFEC_OVER_OVRE6 (0x1u << 6) /**< \brief (AFEC_OVER) Overrun Error 6 */
#define AFEC_OVER_OVRE7 (0x1u << 7) /**< \brief (AFEC_OVER) Overrun Error 7 */
#define AFEC_OVER_OVRE8 (0x1u << 8) /**< \brief (AFEC_OVER) Overrun Error 8 */
#define AFEC_OVER_OVRE9 (0x1u << 9) /**< \brief (AFEC_OVER) Overrun Error 9 */
#define AFEC_OVER_OVRE10 (0x1u << 10) /**< \brief (AFEC_OVER) Overrun Error 10 */
#define AFEC_OVER_OVRE11 (0x1u << 11) /**< \brief (AFEC_OVER) Overrun Error 11 */
/* -------- AFEC_CWR : (AFEC Offset: 0x50) AFEC Compare Window Register -------- */
#define AFEC_CWR_LOWTHRES_Pos 0
#define AFEC_CWR_LOWTHRES_Msk (0xffffu << AFEC_CWR_LOWTHRES_Pos) /**< \brief (AFEC_CWR) Low Threshold */
#define AFEC_CWR_LOWTHRES(value) ((AFEC_CWR_LOWTHRES_Msk & ((value) << AFEC_CWR_LOWTHRES_Pos)))
#define AFEC_CWR_HIGHTHRES_Pos 16
#define AFEC_CWR_HIGHTHRES_Msk (0xffffu << AFEC_CWR_HIGHTHRES_Pos) /**< \brief (AFEC_CWR) High Threshold */
#define AFEC_CWR_HIGHTHRES(value) ((AFEC_CWR_HIGHTHRES_Msk & ((value) << AFEC_CWR_HIGHTHRES_Pos)))
/* -------- AFEC_CGR : (AFEC Offset: 0x54) AFEC Channel Gain Register -------- */
#define AFEC_CGR_GAIN0_Pos 0
#define AFEC_CGR_GAIN0_Msk (0x3u << AFEC_CGR_GAIN0_Pos) /**< \brief (AFEC_CGR) Gain for Channel 0 */
#define AFEC_CGR_GAIN0(value) ((AFEC_CGR_GAIN0_Msk & ((value) << AFEC_CGR_GAIN0_Pos)))
#define AFEC_CGR_GAIN1_Pos 2
#define AFEC_CGR_GAIN1_Msk (0x3u << AFEC_CGR_GAIN1_Pos) /**< \brief (AFEC_CGR) Gain for Channel 1 */
#define AFEC_CGR_GAIN1(value) ((AFEC_CGR_GAIN1_Msk & ((value) << AFEC_CGR_GAIN1_Pos)))
#define AFEC_CGR_GAIN2_Pos 4
#define AFEC_CGR_GAIN2_Msk (0x3u << AFEC_CGR_GAIN2_Pos) /**< \brief (AFEC_CGR) Gain for Channel 2 */
#define AFEC_CGR_GAIN2(value) ((AFEC_CGR_GAIN2_Msk & ((value) << AFEC_CGR_GAIN2_Pos)))
#define AFEC_CGR_GAIN3_Pos 6
#define AFEC_CGR_GAIN3_Msk (0x3u << AFEC_CGR_GAIN3_Pos) /**< \brief (AFEC_CGR) Gain for Channel 3 */
#define AFEC_CGR_GAIN3(value) ((AFEC_CGR_GAIN3_Msk & ((value) << AFEC_CGR_GAIN3_Pos)))
#define AFEC_CGR_GAIN4_Pos 8
#define AFEC_CGR_GAIN4_Msk (0x3u << AFEC_CGR_GAIN4_Pos) /**< \brief (AFEC_CGR) Gain for Channel 4 */
#define AFEC_CGR_GAIN4(value) ((AFEC_CGR_GAIN4_Msk & ((value) << AFEC_CGR_GAIN4_Pos)))
#define AFEC_CGR_GAIN5_Pos 10
#define AFEC_CGR_GAIN5_Msk (0x3u << AFEC_CGR_GAIN5_Pos) /**< \brief (AFEC_CGR) Gain for Channel 5 */
#define AFEC_CGR_GAIN5(value) ((AFEC_CGR_GAIN5_Msk & ((value) << AFEC_CGR_GAIN5_Pos)))
#define AFEC_CGR_GAIN6_Pos 12
#define AFEC_CGR_GAIN6_Msk (0x3u << AFEC_CGR_GAIN6_Pos) /**< \brief (AFEC_CGR) Gain for Channel 6 */
#define AFEC_CGR_GAIN6(value) ((AFEC_CGR_GAIN6_Msk & ((value) << AFEC_CGR_GAIN6_Pos)))
#define AFEC_CGR_GAIN7_Pos 14
#define AFEC_CGR_GAIN7_Msk (0x3u << AFEC_CGR_GAIN7_Pos) /**< \brief (AFEC_CGR) Gain for Channel 7 */
#define AFEC_CGR_GAIN7(value) ((AFEC_CGR_GAIN7_Msk & ((value) << AFEC_CGR_GAIN7_Pos)))
#define AFEC_CGR_GAIN8_Pos 16
#define AFEC_CGR_GAIN8_Msk (0x3u << AFEC_CGR_GAIN8_Pos) /**< \brief (AFEC_CGR) Gain for Channel 8 */
#define AFEC_CGR_GAIN8(value) ((AFEC_CGR_GAIN8_Msk & ((value) << AFEC_CGR_GAIN8_Pos)))
#define AFEC_CGR_GAIN9_Pos 18
#define AFEC_CGR_GAIN9_Msk (0x3u << AFEC_CGR_GAIN9_Pos) /**< \brief (AFEC_CGR) Gain for Channel 9 */
#define AFEC_CGR_GAIN9(value) ((AFEC_CGR_GAIN9_Msk & ((value) << AFEC_CGR_GAIN9_Pos)))
#define AFEC_CGR_GAIN10_Pos 20
#define AFEC_CGR_GAIN10_Msk (0x3u << AFEC_CGR_GAIN10_Pos) /**< \brief (AFEC_CGR) Gain for Channel 10 */
#define AFEC_CGR_GAIN10(value) ((AFEC_CGR_GAIN10_Msk & ((value) << AFEC_CGR_GAIN10_Pos)))
#define AFEC_CGR_GAIN11_Pos 22
#define AFEC_CGR_GAIN11_Msk (0x3u << AFEC_CGR_GAIN11_Pos) /**< \brief (AFEC_CGR) Gain for Channel 11 */
#define AFEC_CGR_GAIN11(value) ((AFEC_CGR_GAIN11_Msk & ((value) << AFEC_CGR_GAIN11_Pos)))
/* -------- AFEC_DIFFR : (AFEC Offset: 0x60) AFEC Channel Differential Register -------- */
#define AFEC_DIFFR_DIFF0 (0x1u << 0) /**< \brief (AFEC_DIFFR) Differential inputs for channel 0 */
#define AFEC_DIFFR_DIFF1 (0x1u << 1) /**< \brief (AFEC_DIFFR) Differential inputs for channel 1 */
#define AFEC_DIFFR_DIFF2 (0x1u << 2) /**< \brief (AFEC_DIFFR) Differential inputs for channel 2 */
#define AFEC_DIFFR_DIFF3 (0x1u << 3) /**< \brief (AFEC_DIFFR) Differential inputs for channel 3 */
#define AFEC_DIFFR_DIFF4 (0x1u << 4) /**< \brief (AFEC_DIFFR) Differential inputs for channel 4 */
#define AFEC_DIFFR_DIFF5 (0x1u << 5) /**< \brief (AFEC_DIFFR) Differential inputs for channel 5 */
#define AFEC_DIFFR_DIFF6 (0x1u << 6) /**< \brief (AFEC_DIFFR) Differential inputs for channel 6 */
#define AFEC_DIFFR_DIFF7 (0x1u << 7) /**< \brief (AFEC_DIFFR) Differential inputs for channel 7 */
#define AFEC_DIFFR_DIFF8 (0x1u << 8) /**< \brief (AFEC_DIFFR) Differential inputs for channel 8 */
#define AFEC_DIFFR_DIFF9 (0x1u << 9) /**< \brief (AFEC_DIFFR) Differential inputs for channel 9 */
#define AFEC_DIFFR_DIFF10 (0x1u << 10) /**< \brief (AFEC_DIFFR) Differential inputs for channel 10 */
#define AFEC_DIFFR_DIFF11 (0x1u << 11) /**< \brief (AFEC_DIFFR) Differential inputs for channel 11 */
/* -------- AFEC_CSELR : (AFEC Offset: 0x64) AFEC Channel Selection Register -------- */
#define AFEC_CSELR_CSEL_Pos 0
#define AFEC_CSELR_CSEL_Msk (0xfu << AFEC_CSELR_CSEL_Pos) /**< \brief (AFEC_CSELR) Channel Selection */
#define AFEC_CSELR_CSEL(value) ((AFEC_CSELR_CSEL_Msk & ((value) << AFEC_CSELR_CSEL_Pos)))
/* -------- AFEC_CDR : (AFEC Offset: 0x68) AFEC Channel Data Register -------- */
#define AFEC_CDR_DATA_Pos 0
#define AFEC_CDR_DATA_Msk (0xffffu << AFEC_CDR_DATA_Pos) /**< \brief (AFEC_CDR) Converted Data */
/* -------- AFEC_COCR : (AFEC Offset: 0x6C) AFEC Channel Offset Compensation Register -------- */
#define AFEC_COCR_AOFF_Pos 0
#define AFEC_COCR_AOFF_Msk (0xfffu << AFEC_COCR_AOFF_Pos) /**< \brief (AFEC_COCR) Analog Offset */
#define AFEC_COCR_AOFF(value) ((AFEC_COCR_AOFF_Msk & ((value) << AFEC_COCR_AOFF_Pos)))
/* -------- AFEC_TEMPMR : (AFEC Offset: 0x70) AFEC Temperature Sensor Mode Register -------- */
#define AFEC_TEMPMR_RTCT (0x1u << 0) /**< \brief (AFEC_TEMPMR) Temperature Sensor RTC Trigger Mode */
#define AFEC_TEMPMR_TEMPCMPMOD_Pos 4
#define AFEC_TEMPMR_TEMPCMPMOD_Msk (0x3u << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< \brief (AFEC_TEMPMR) Temperature Comparison Mode */
#define AFEC_TEMPMR_TEMPCMPMOD(value) ((AFEC_TEMPMR_TEMPCMPMOD_Msk & ((value) << AFEC_TEMPMR_TEMPCMPMOD_Pos)))
#define   AFEC_TEMPMR_TEMPCMPMOD_LOW (0x0u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is lower than the low threshold of the window. */
#define   AFEC_TEMPMR_TEMPCMPMOD_HIGH (0x1u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is higher than the high threshold of the window. */
#define   AFEC_TEMPMR_TEMPCMPMOD_IN (0x2u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is in the comparison window. */
#define   AFEC_TEMPMR_TEMPCMPMOD_OUT (0x3u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is out of the comparison window. */
/* -------- AFEC_TEMPCWR : (AFEC Offset: 0x74) AFEC Temperature Compare Window Register -------- */
#define AFEC_TEMPCWR_TLOWTHRES_Pos 0
#define AFEC_TEMPCWR_TLOWTHRES_Msk (0xffffu << AFEC_TEMPCWR_TLOWTHRES_Pos) /**< \brief (AFEC_TEMPCWR) Temperature Low Threshold */
#define AFEC_TEMPCWR_TLOWTHRES(value) ((AFEC_TEMPCWR_TLOWTHRES_Msk & ((value) << AFEC_TEMPCWR_TLOWTHRES_Pos)))
#define AFEC_TEMPCWR_THIGHTHRES_Pos 16
#define AFEC_TEMPCWR_THIGHTHRES_Msk (0xffffu << AFEC_TEMPCWR_THIGHTHRES_Pos) /**< \brief (AFEC_TEMPCWR) Temperature High Threshold */
#define AFEC_TEMPCWR_THIGHTHRES(value) ((AFEC_TEMPCWR_THIGHTHRES_Msk & ((value) << AFEC_TEMPCWR_THIGHTHRES_Pos)))
/* -------- AFEC_ACR : (AFEC Offset: 0x94) AFEC Analog Control Register -------- */
#define AFEC_ACR_PGA0EN (0x1u << 2) /**< \brief (AFEC_ACR) PGA0 Enable */
#define AFEC_ACR_PGA1EN (0x1u << 3) /**< \brief (AFEC_ACR) PGA1 Enable */
#define AFEC_ACR_IBCTL_Pos 8
#define AFEC_ACR_IBCTL_Msk (0x3u << AFEC_ACR_IBCTL_Pos) /**< \brief (AFEC_ACR) AFE Bias Current Control */
#define AFEC_ACR_IBCTL(value) ((AFEC_ACR_IBCTL_Msk & ((value) << AFEC_ACR_IBCTL_Pos)))
/* -------- AFEC_SHMR : (AFEC Offset: 0xA0) AFEC Sample & Hold Mode Register -------- */
#define AFEC_SHMR_DUAL0 (0x1u << 0) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 0 */
#define AFEC_SHMR_DUAL1 (0x1u << 1) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 1 */
#define AFEC_SHMR_DUAL2 (0x1u << 2) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 2 */
#define AFEC_SHMR_DUAL3 (0x1u << 3) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 3 */
#define AFEC_SHMR_DUAL4 (0x1u << 4) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 4 */
#define AFEC_SHMR_DUAL5 (0x1u << 5) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 5 */
#define AFEC_SHMR_DUAL6 (0x1u << 6) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 6 */
#define AFEC_SHMR_DUAL7 (0x1u << 7) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 7 */
#define AFEC_SHMR_DUAL8 (0x1u << 8) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 8 */
#define AFEC_SHMR_DUAL9 (0x1u << 9) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 9 */
#define AFEC_SHMR_DUAL10 (0x1u << 10) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 10 */
#define AFEC_SHMR_DUAL11 (0x1u << 11) /**< \brief (AFEC_SHMR) Dual Sample & Hold for channel 11 */
/* -------- AFEC_COSR : (AFEC Offset: 0xD0) AFEC Correction Select Register -------- */
#define AFEC_COSR_CSEL (0x1u << 0) /**< \brief (AFEC_COSR) Sample & Hold unit Correction Select */
/* -------- AFEC_CVR : (AFEC Offset: 0xD4) AFEC Correction Values Register -------- */
#define AFEC_CVR_OFFSETCORR_Pos 0
#define AFEC_CVR_OFFSETCORR_Msk (0xffffu << AFEC_CVR_OFFSETCORR_Pos) /**< \brief (AFEC_CVR) Offset Correction */
#define AFEC_CVR_OFFSETCORR(value) ((AFEC_CVR_OFFSETCORR_Msk & ((value) << AFEC_CVR_OFFSETCORR_Pos)))
#define AFEC_CVR_GAINCORR_Pos 16
#define AFEC_CVR_GAINCORR_Msk (0xffffu << AFEC_CVR_GAINCORR_Pos) /**< \brief (AFEC_CVR) Gain Correction */
#define AFEC_CVR_GAINCORR(value) ((AFEC_CVR_GAINCORR_Msk & ((value) << AFEC_CVR_GAINCORR_Pos)))
/* -------- AFEC_CECR : (AFEC Offset: 0xD8) AFEC Channel Error Correction Register -------- */
#define AFEC_CECR_ECORR0 (0x1u << 0) /**< \brief (AFEC_CECR) Error Correction Enable for channel 0 */
#define AFEC_CECR_ECORR1 (0x1u << 1) /**< \brief (AFEC_CECR) Error Correction Enable for channel 1 */
#define AFEC_CECR_ECORR2 (0x1u << 2) /**< \brief (AFEC_CECR) Error Correction Enable for channel 2 */
#define AFEC_CECR_ECORR3 (0x1u << 3) /**< \brief (AFEC_CECR) Error Correction Enable for channel 3 */
#define AFEC_CECR_ECORR4 (0x1u << 4) /**< \brief (AFEC_CECR) Error Correction Enable for channel 4 */
#define AFEC_CECR_ECORR5 (0x1u << 5) /**< \brief (AFEC_CECR) Error Correction Enable for channel 5 */
#define AFEC_CECR_ECORR6 (0x1u << 6) /**< \brief (AFEC_CECR) Error Correction Enable for channel 6 */
#define AFEC_CECR_ECORR7 (0x1u << 7) /**< \brief (AFEC_CECR) Error Correction Enable for channel 7 */
#define AFEC_CECR_ECORR8 (0x1u << 8) /**< \brief (AFEC_CECR) Error Correction Enable for channel 8 */
#define AFEC_CECR_ECORR9 (0x1u << 9) /**< \brief (AFEC_CECR) Error Correction Enable for channel 9 */
#define AFEC_CECR_ECORR10 (0x1u << 10) /**< \brief (AFEC_CECR) Error Correction Enable for channel 10 */
#define AFEC_CECR_ECORR11 (0x1u << 11) /**< \brief (AFEC_CECR) Error Correction Enable for channel 11 */
/* -------- AFEC_WPMR : (AFEC Offset: 0xE4) AFEC Write Protection Mode Register -------- */
#define AFEC_WPMR_WPEN (0x1u << 0) /**< \brief (AFEC_WPMR) Write Protection Enable */
#define AFEC_WPMR_WPKEY_Pos 8
#define AFEC_WPMR_WPKEY_Msk (0xffffffu << AFEC_WPMR_WPKEY_Pos) /**< \brief (AFEC_WPMR) Write Protect KEY */
#define AFEC_WPMR_WPKEY(value) ((AFEC_WPMR_WPKEY_Msk & ((value) << AFEC_WPMR_WPKEY_Pos)))
#define   AFEC_WPMR_WPKEY_PASSWD (0x414443u << 8) /**< \brief (AFEC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. */
/* -------- AFEC_WPSR : (AFEC Offset: 0xE8) AFEC Write Protection Status Register -------- */
#define AFEC_WPSR_WPVS (0x1u << 0) /**< \brief (AFEC_WPSR) Write Protect Violation Status */
#define AFEC_WPSR_WPVSRC_Pos 8
#define AFEC_WPSR_WPVSRC_Msk (0xffffu << AFEC_WPSR_WPVSRC_Pos) /**< \brief (AFEC_WPSR) Write Protect Violation Source */
/* -------- AFEC_VERSION : (AFEC Offset: 0xFC) AFEC Version Register -------- */
#define AFEC_VERSION_VERSION_Pos 0
#define AFEC_VERSION_VERSION_Msk (0xfffu << AFEC_VERSION_VERSION_Pos) /**< \brief (AFEC_VERSION) Version of the Hardware Module */
#define AFEC_VERSION_MFN_Pos 16
#define AFEC_VERSION_MFN_Msk (0x7u << AFEC_VERSION_MFN_Pos) /**< \brief (AFEC_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_AFEC_COMPONENT_ */
