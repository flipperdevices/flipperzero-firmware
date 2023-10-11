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

#ifndef _SAME70_AFEC0_INSTANCE_
#define _SAME70_AFEC0_INSTANCE_

/* ========== Register definition for AFEC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_AFEC0_CR                       (0x4003C000U) /**< \brief (AFEC0) AFEC Control Register */
  #define REG_AFEC0_MR                       (0x4003C004U) /**< \brief (AFEC0) AFEC Mode Register */
  #define REG_AFEC0_EMR                      (0x4003C008U) /**< \brief (AFEC0) AFEC Extended Mode Register */
  #define REG_AFEC0_SEQ1R                    (0x4003C00CU) /**< \brief (AFEC0) AFEC Channel Sequence 1 Register */
  #define REG_AFEC0_SEQ2R                    (0x4003C010U) /**< \brief (AFEC0) AFEC Channel Sequence 2 Register */
  #define REG_AFEC0_CHER                     (0x4003C014U) /**< \brief (AFEC0) AFEC Channel Enable Register */
  #define REG_AFEC0_CHDR                     (0x4003C018U) /**< \brief (AFEC0) AFEC Channel Disable Register */
  #define REG_AFEC0_CHSR                     (0x4003C01CU) /**< \brief (AFEC0) AFEC Channel Status Register */
  #define REG_AFEC0_LCDR                     (0x4003C020U) /**< \brief (AFEC0) AFEC Last Converted Data Register */
  #define REG_AFEC0_IER                      (0x4003C024U) /**< \brief (AFEC0) AFEC Interrupt Enable Register */
  #define REG_AFEC0_IDR                      (0x4003C028U) /**< \brief (AFEC0) AFEC Interrupt Disable Register */
  #define REG_AFEC0_IMR                      (0x4003C02CU) /**< \brief (AFEC0) AFEC Interrupt Mask Register */
  #define REG_AFEC0_ISR                      (0x4003C030U) /**< \brief (AFEC0) AFEC Interrupt Status Register */
  #define REG_AFEC0_OVER                     (0x4003C04CU) /**< \brief (AFEC0) AFEC Overrun Status Register */
  #define REG_AFEC0_CWR                      (0x4003C050U) /**< \brief (AFEC0) AFEC Compare Window Register */
  #define REG_AFEC0_CGR                      (0x4003C054U) /**< \brief (AFEC0) AFEC Channel Gain Register */
  #define REG_AFEC0_DIFFR                    (0x4003C060U) /**< \brief (AFEC0) AFEC Channel Differential Register */
  #define REG_AFEC0_CSELR                    (0x4003C064U) /**< \brief (AFEC0) AFEC Channel Selection Register */
  #define REG_AFEC0_CDR                      (0x4003C068U) /**< \brief (AFEC0) AFEC Channel Data Register */
  #define REG_AFEC0_COCR                     (0x4003C06CU) /**< \brief (AFEC0) AFEC Channel Offset Compensation Register */
  #define REG_AFEC0_TEMPMR                   (0x4003C070U) /**< \brief (AFEC0) AFEC Temperature Sensor Mode Register */
  #define REG_AFEC0_TEMPCWR                  (0x4003C074U) /**< \brief (AFEC0) AFEC Temperature Compare Window Register */
  #define REG_AFEC0_ACR                      (0x4003C094U) /**< \brief (AFEC0) AFEC Analog Control Register */
  #define REG_AFEC0_SHMR                     (0x4003C0A0U) /**< \brief (AFEC0) AFEC Sample & Hold Mode Register */
  #define REG_AFEC0_COSR                     (0x4003C0D0U) /**< \brief (AFEC0) AFEC Correction Select Register */
  #define REG_AFEC0_CVR                      (0x4003C0D4U) /**< \brief (AFEC0) AFEC Correction Values Register */
  #define REG_AFEC0_CECR                     (0x4003C0D8U) /**< \brief (AFEC0) AFEC Channel Error Correction Register */
  #define REG_AFEC0_WPMR                     (0x4003C0E4U) /**< \brief (AFEC0) AFEC Write Protection Mode Register */
  #define REG_AFEC0_WPSR                     (0x4003C0E8U) /**< \brief (AFEC0) AFEC Write Protection Status Register */
  #define REG_AFEC0_VERSION                  (0x4003C0FCU) /**< \brief (AFEC0) AFEC Version Register */
#else
  #define REG_AFEC0_CR      (*(__O  uint32_t*)0x4003C000U) /**< \brief (AFEC0) AFEC Control Register */
  #define REG_AFEC0_MR      (*(__IO uint32_t*)0x4003C004U) /**< \brief (AFEC0) AFEC Mode Register */
  #define REG_AFEC0_EMR     (*(__IO uint32_t*)0x4003C008U) /**< \brief (AFEC0) AFEC Extended Mode Register */
  #define REG_AFEC0_SEQ1R   (*(__IO uint32_t*)0x4003C00CU) /**< \brief (AFEC0) AFEC Channel Sequence 1 Register */
  #define REG_AFEC0_SEQ2R   (*(__IO uint32_t*)0x4003C010U) /**< \brief (AFEC0) AFEC Channel Sequence 2 Register */
  #define REG_AFEC0_CHER    (*(__O  uint32_t*)0x4003C014U) /**< \brief (AFEC0) AFEC Channel Enable Register */
  #define REG_AFEC0_CHDR    (*(__O  uint32_t*)0x4003C018U) /**< \brief (AFEC0) AFEC Channel Disable Register */
  #define REG_AFEC0_CHSR    (*(__I  uint32_t*)0x4003C01CU) /**< \brief (AFEC0) AFEC Channel Status Register */
  #define REG_AFEC0_LCDR    (*(__I  uint32_t*)0x4003C020U) /**< \brief (AFEC0) AFEC Last Converted Data Register */
  #define REG_AFEC0_IER     (*(__O  uint32_t*)0x4003C024U) /**< \brief (AFEC0) AFEC Interrupt Enable Register */
  #define REG_AFEC0_IDR     (*(__O  uint32_t*)0x4003C028U) /**< \brief (AFEC0) AFEC Interrupt Disable Register */
  #define REG_AFEC0_IMR     (*(__I  uint32_t*)0x4003C02CU) /**< \brief (AFEC0) AFEC Interrupt Mask Register */
  #define REG_AFEC0_ISR     (*(__I  uint32_t*)0x4003C030U) /**< \brief (AFEC0) AFEC Interrupt Status Register */
  #define REG_AFEC0_OVER    (*(__I  uint32_t*)0x4003C04CU) /**< \brief (AFEC0) AFEC Overrun Status Register */
  #define REG_AFEC0_CWR     (*(__IO uint32_t*)0x4003C050U) /**< \brief (AFEC0) AFEC Compare Window Register */
  #define REG_AFEC0_CGR     (*(__IO uint32_t*)0x4003C054U) /**< \brief (AFEC0) AFEC Channel Gain Register */
  #define REG_AFEC0_DIFFR   (*(__IO uint32_t*)0x4003C060U) /**< \brief (AFEC0) AFEC Channel Differential Register */
  #define REG_AFEC0_CSELR   (*(__IO uint32_t*)0x4003C064U) /**< \brief (AFEC0) AFEC Channel Selection Register */
  #define REG_AFEC0_CDR     (*(__I  uint32_t*)0x4003C068U) /**< \brief (AFEC0) AFEC Channel Data Register */
  #define REG_AFEC0_COCR    (*(__IO uint32_t*)0x4003C06CU) /**< \brief (AFEC0) AFEC Channel Offset Compensation Register */
  #define REG_AFEC0_TEMPMR  (*(__IO uint32_t*)0x4003C070U) /**< \brief (AFEC0) AFEC Temperature Sensor Mode Register */
  #define REG_AFEC0_TEMPCWR (*(__IO uint32_t*)0x4003C074U) /**< \brief (AFEC0) AFEC Temperature Compare Window Register */
  #define REG_AFEC0_ACR     (*(__IO uint32_t*)0x4003C094U) /**< \brief (AFEC0) AFEC Analog Control Register */
  #define REG_AFEC0_SHMR    (*(__IO uint32_t*)0x4003C0A0U) /**< \brief (AFEC0) AFEC Sample & Hold Mode Register */
  #define REG_AFEC0_COSR    (*(__IO uint32_t*)0x4003C0D0U) /**< \brief (AFEC0) AFEC Correction Select Register */
  #define REG_AFEC0_CVR     (*(__IO uint32_t*)0x4003C0D4U) /**< \brief (AFEC0) AFEC Correction Values Register */
  #define REG_AFEC0_CECR    (*(__IO uint32_t*)0x4003C0D8U) /**< \brief (AFEC0) AFEC Channel Error Correction Register */
  #define REG_AFEC0_WPMR    (*(__IO uint32_t*)0x4003C0E4U) /**< \brief (AFEC0) AFEC Write Protection Mode Register */
  #define REG_AFEC0_WPSR    (*(__I  uint32_t*)0x4003C0E8U) /**< \brief (AFEC0) AFEC Write Protection Status Register */
  #define REG_AFEC0_VERSION (*(__I  uint32_t*)0x4003C0FCU) /**< \brief (AFEC0) AFEC Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_AFEC0_INSTANCE_ */
