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

#ifndef _SAME70_AES_INSTANCE_
#define _SAME70_AES_INSTANCE_

/* ========== Register definition for AES peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_AES_CR                       (0x4006C000U) /**< \brief (AES) Control Register */
  #define REG_AES_MR                       (0x4006C004U) /**< \brief (AES) Mode Register */
  #define REG_AES_IER                      (0x4006C010U) /**< \brief (AES) Interrupt Enable Register */
  #define REG_AES_IDR                      (0x4006C014U) /**< \brief (AES) Interrupt Disable Register */
  #define REG_AES_IMR                      (0x4006C018U) /**< \brief (AES) Interrupt Mask Register */
  #define REG_AES_ISR                      (0x4006C01CU) /**< \brief (AES) Interrupt Status Register */
  #define REG_AES_KEYWR                    (0x4006C020U) /**< \brief (AES) Key Word Register */
  #define REG_AES_IDATAR                   (0x4006C040U) /**< \brief (AES) Input Data Register */
  #define REG_AES_ODATAR                   (0x4006C050U) /**< \brief (AES) Output Data Register */
  #define REG_AES_IVR                      (0x4006C060U) /**< \brief (AES) Initialization Vector Register */
  #define REG_AES_AADLENR                  (0x4006C070U) /**< \brief (AES) Additional Authenticated Data Length Register */
  #define REG_AES_CLENR                    (0x4006C074U) /**< \brief (AES) Plaintext/Ciphertext Length Register */
  #define REG_AES_GHASHR                   (0x4006C078U) /**< \brief (AES) GCM Intermediate Hash Word Register */
  #define REG_AES_TAGR                     (0x4006C088U) /**< \brief (AES) GCM Authentication Tag Word Register */
  #define REG_AES_CTRR                     (0x4006C098U) /**< \brief (AES) GCM Encryption Counter Value Register */
  #define REG_AES_GCMHR                    (0x4006C09CU) /**< \brief (AES) GCM H Word Register */
  #define REG_AES_VERSION                  (0x4006C0FCU) /**< \brief (AES) Version Register */
#else
  #define REG_AES_CR      (*(__O  uint32_t*)0x4006C000U) /**< \brief (AES) Control Register */
  #define REG_AES_MR      (*(__IO uint32_t*)0x4006C004U) /**< \brief (AES) Mode Register */
  #define REG_AES_IER     (*(__O  uint32_t*)0x4006C010U) /**< \brief (AES) Interrupt Enable Register */
  #define REG_AES_IDR     (*(__O  uint32_t*)0x4006C014U) /**< \brief (AES) Interrupt Disable Register */
  #define REG_AES_IMR     (*(__I  uint32_t*)0x4006C018U) /**< \brief (AES) Interrupt Mask Register */
  #define REG_AES_ISR     (*(__I  uint32_t*)0x4006C01CU) /**< \brief (AES) Interrupt Status Register */
  #define REG_AES_KEYWR   (*(__O  uint32_t*)0x4006C020U) /**< \brief (AES) Key Word Register */
  #define REG_AES_IDATAR  (*(__O  uint32_t*)0x4006C040U) /**< \brief (AES) Input Data Register */
  #define REG_AES_ODATAR  (*(__I  uint32_t*)0x4006C050U) /**< \brief (AES) Output Data Register */
  #define REG_AES_IVR     (*(__O  uint32_t*)0x4006C060U) /**< \brief (AES) Initialization Vector Register */
  #define REG_AES_AADLENR (*(__IO uint32_t*)0x4006C070U) /**< \brief (AES) Additional Authenticated Data Length Register */
  #define REG_AES_CLENR   (*(__IO uint32_t*)0x4006C074U) /**< \brief (AES) Plaintext/Ciphertext Length Register */
  #define REG_AES_GHASHR  (*(__IO uint32_t*)0x4006C078U) /**< \brief (AES) GCM Intermediate Hash Word Register */
  #define REG_AES_TAGR    (*(__I  uint32_t*)0x4006C088U) /**< \brief (AES) GCM Authentication Tag Word Register */
  #define REG_AES_CTRR    (*(__I  uint32_t*)0x4006C098U) /**< \brief (AES) GCM Encryption Counter Value Register */
  #define REG_AES_GCMHR   (*(__IO uint32_t*)0x4006C09CU) /**< \brief (AES) GCM H Word Register */
  #define REG_AES_VERSION (*(__I  uint32_t*)0x4006C0FCU) /**< \brief (AES) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_AES_INSTANCE_ */
