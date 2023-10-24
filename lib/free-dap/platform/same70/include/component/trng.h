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

#ifndef _SAME70_TRNG_COMPONENT_
#define _SAME70_TRNG_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR True Random Number Generator */
/* ============================================================================= */
/** \addtogroup SAME70_TRNG True Random Number Generator */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Trng hardware registers */
typedef struct {
  __O uint32_t TRNG_CR;       /**< \brief (Trng Offset: 0x00) Control Register */
  __I uint32_t Reserved1[3];
  __O uint32_t TRNG_IER;      /**< \brief (Trng Offset: 0x10) Interrupt Enable Register */
  __O uint32_t TRNG_IDR;      /**< \brief (Trng Offset: 0x14) Interrupt Disable Register */
  __I uint32_t TRNG_IMR;      /**< \brief (Trng Offset: 0x18) Interrupt Mask Register */
  __I uint32_t TRNG_ISR;      /**< \brief (Trng Offset: 0x1C) Interrupt Status Register */
  __I uint32_t Reserved2[12];
  __I uint32_t TRNG_ODATA;    /**< \brief (Trng Offset: 0x50) Output Data Register */
  __I uint32_t Reserved3[42];
  __I uint32_t TRNG_VERSION;  /**< \brief (Trng Offset: 0xFC) Version Register */
} Trng;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- TRNG_CR : (TRNG Offset: 0x00) Control Register -------- */
#define TRNG_CR_ENABLE (0x1u << 0) /**< \brief (TRNG_CR) Enables the TRNG to provide random values */
#define TRNG_CR_KEY_Pos 8
#define TRNG_CR_KEY_Msk (0xffffffu << TRNG_CR_KEY_Pos) /**< \brief (TRNG_CR) Security Key. */
#define TRNG_CR_KEY(value) ((TRNG_CR_KEY_Msk & ((value) << TRNG_CR_KEY_Pos)))
#define   TRNG_CR_KEY_PASSWD (0x524E47u << 8) /**< \brief (TRNG_CR) Writing any other value in this field aborts the write operation. */
/* -------- TRNG_IER : (TRNG Offset: 0x10) Interrupt Enable Register -------- */
#define TRNG_IER_DATRDY (0x1u << 0) /**< \brief (TRNG_IER) Data Ready Interrupt Enable */
/* -------- TRNG_IDR : (TRNG Offset: 0x14) Interrupt Disable Register -------- */
#define TRNG_IDR_DATRDY (0x1u << 0) /**< \brief (TRNG_IDR) Data Ready Interrupt Disable */
/* -------- TRNG_IMR : (TRNG Offset: 0x18) Interrupt Mask Register -------- */
#define TRNG_IMR_DATRDY (0x1u << 0) /**< \brief (TRNG_IMR) Data Ready Interrupt Mask */
/* -------- TRNG_ISR : (TRNG Offset: 0x1C) Interrupt Status Register -------- */
#define TRNG_ISR_DATRDY (0x1u << 0) /**< \brief (TRNG_ISR) Data Ready */
/* -------- TRNG_ODATA : (TRNG Offset: 0x50) Output Data Register -------- */
#define TRNG_ODATA_ODATA_Pos 0
#define TRNG_ODATA_ODATA_Msk (0xffffffffu << TRNG_ODATA_ODATA_Pos) /**< \brief (TRNG_ODATA) Output Data */
/* -------- TRNG_VERSION : (TRNG Offset: 0xFC) Version Register -------- */
#define TRNG_VERSION_VERSION_Pos 0
#define TRNG_VERSION_VERSION_Msk (0xfffu << TRNG_VERSION_VERSION_Pos) /**< \brief (TRNG_VERSION) Version of the Hardware Module */
#define TRNG_VERSION_MFN_Pos 16
#define TRNG_VERSION_MFN_Msk (0x7u << TRNG_VERSION_MFN_Pos) /**< \brief (TRNG_VERSION) Metal Fix Number */

/*@}*/


#endif /* _SAME70_TRNG_COMPONENT_ */
