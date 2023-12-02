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

#ifndef _SAME70_TRNG_INSTANCE_
#define _SAME70_TRNG_INSTANCE_

/* ========== Register definition for TRNG peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_TRNG_CR                      (0x40070000U) /**< \brief (TRNG) Control Register */
  #define REG_TRNG_IER                     (0x40070010U) /**< \brief (TRNG) Interrupt Enable Register */
  #define REG_TRNG_IDR                     (0x40070014U) /**< \brief (TRNG) Interrupt Disable Register */
  #define REG_TRNG_IMR                     (0x40070018U) /**< \brief (TRNG) Interrupt Mask Register */
  #define REG_TRNG_ISR                     (0x4007001CU) /**< \brief (TRNG) Interrupt Status Register */
  #define REG_TRNG_ODATA                   (0x40070050U) /**< \brief (TRNG) Output Data Register */
  #define REG_TRNG_VERSION                 (0x400700FCU) /**< \brief (TRNG) Version Register */
#else
  #define REG_TRNG_CR      (*(__O uint32_t*)0x40070000U) /**< \brief (TRNG) Control Register */
  #define REG_TRNG_IER     (*(__O uint32_t*)0x40070010U) /**< \brief (TRNG) Interrupt Enable Register */
  #define REG_TRNG_IDR     (*(__O uint32_t*)0x40070014U) /**< \brief (TRNG) Interrupt Disable Register */
  #define REG_TRNG_IMR     (*(__I uint32_t*)0x40070018U) /**< \brief (TRNG) Interrupt Mask Register */
  #define REG_TRNG_ISR     (*(__I uint32_t*)0x4007001CU) /**< \brief (TRNG) Interrupt Status Register */
  #define REG_TRNG_ODATA   (*(__I uint32_t*)0x40070050U) /**< \brief (TRNG) Output Data Register */
  #define REG_TRNG_VERSION (*(__I uint32_t*)0x400700FCU) /**< \brief (TRNG) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_TRNG_INSTANCE_ */
