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

#ifndef _SAME70_UTMI_COMPONENT_
#define _SAME70_UTMI_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR USB Transmitter Interface Macrocell */
/* ============================================================================= */
/** \addtogroup SAME70_UTMI USB Transmitter Interface Macrocell */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Utmi hardware registers */
typedef struct {
  __I  uint32_t Reserved1[4];
  __IO uint32_t UTMI_OHCIICR; /**< \brief (Utmi Offset: 0x10) OHCI Interrupt Configuration Register */
  __I  uint32_t Reserved2[7];
  __IO uint32_t UTMI_CKTRIM;  /**< \brief (Utmi Offset: 0x30) UTMI Clock Trimming Register */
} Utmi;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- UTMI_OHCIICR : (UTMI Offset: 0x10) OHCI Interrupt Configuration Register -------- */
#define UTMI_OHCIICR_RES0 (0x1u << 0) /**< \brief (UTMI_OHCIICR) USB PORTx Reset */
#define UTMI_OHCIICR_ARIE (0x1u << 4) /**< \brief (UTMI_OHCIICR) OHCI Asynchronous Resume Interrupt Enable */
#define UTMI_OHCIICR_APPSTART (0x1u << 5) /**< \brief (UTMI_OHCIICR) Reserved */
#define UTMI_OHCIICR_UDPPUDIS (0x1u << 23) /**< \brief (UTMI_OHCIICR) USB Device Pull-up Disable */
/* -------- UTMI_CKTRIM : (UTMI Offset: 0x30) UTMI Clock Trimming Register -------- */
#define UTMI_CKTRIM_FREQ_Pos 0
#define UTMI_CKTRIM_FREQ_Msk (0x3u << UTMI_CKTRIM_FREQ_Pos) /**< \brief (UTMI_CKTRIM) UTMI Reference Clock Frequency */
#define UTMI_CKTRIM_FREQ(value) ((UTMI_CKTRIM_FREQ_Msk & ((value) << UTMI_CKTRIM_FREQ_Pos)))
#define   UTMI_CKTRIM_FREQ_XTAL12 (0x0u << 0) /**< \brief (UTMI_CKTRIM) 12 MHz reference clock */
#define   UTMI_CKTRIM_FREQ_XTAL16 (0x1u << 0) /**< \brief (UTMI_CKTRIM) 16 MHz reference clock */

/*@}*/


#endif /* _SAME70_UTMI_COMPONENT_ */
