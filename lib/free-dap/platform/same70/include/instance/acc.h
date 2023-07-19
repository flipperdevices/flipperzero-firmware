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

#ifndef _SAME70_ACC_INSTANCE_
#define _SAME70_ACC_INSTANCE_

/* ========== Register definition for ACC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_ACC_CR                    (0x40044000U) /**< \brief (ACC) Control Register */
  #define REG_ACC_MR                    (0x40044004U) /**< \brief (ACC) Mode Register */
  #define REG_ACC_IER                   (0x40044024U) /**< \brief (ACC) Interrupt Enable Register */
  #define REG_ACC_IDR                   (0x40044028U) /**< \brief (ACC) Interrupt Disable Register */
  #define REG_ACC_IMR                   (0x4004402CU) /**< \brief (ACC) Interrupt Mask Register */
  #define REG_ACC_ISR                   (0x40044030U) /**< \brief (ACC) Interrupt Status Register */
  #define REG_ACC_ACR                   (0x40044094U) /**< \brief (ACC) Analog Control Register */
  #define REG_ACC_WPMR                  (0x400440E4U) /**< \brief (ACC) Write Protection Mode Register */
  #define REG_ACC_WPSR                  (0x400440E8U) /**< \brief (ACC) Write Protection Status Register */
  #define REG_ACC_VER                   (0x400440FCU) /**< \brief (ACC) Version Register */
#else
  #define REG_ACC_CR   (*(__O  uint32_t*)0x40044000U) /**< \brief (ACC) Control Register */
  #define REG_ACC_MR   (*(__IO uint32_t*)0x40044004U) /**< \brief (ACC) Mode Register */
  #define REG_ACC_IER  (*(__O  uint32_t*)0x40044024U) /**< \brief (ACC) Interrupt Enable Register */
  #define REG_ACC_IDR  (*(__O  uint32_t*)0x40044028U) /**< \brief (ACC) Interrupt Disable Register */
  #define REG_ACC_IMR  (*(__I  uint32_t*)0x4004402CU) /**< \brief (ACC) Interrupt Mask Register */
  #define REG_ACC_ISR  (*(__I  uint32_t*)0x40044030U) /**< \brief (ACC) Interrupt Status Register */
  #define REG_ACC_ACR  (*(__IO uint32_t*)0x40044094U) /**< \brief (ACC) Analog Control Register */
  #define REG_ACC_WPMR (*(__IO uint32_t*)0x400440E4U) /**< \brief (ACC) Write Protection Mode Register */
  #define REG_ACC_WPSR (*(__I  uint32_t*)0x400440E8U) /**< \brief (ACC) Write Protection Status Register */
  #define REG_ACC_VER  (*(__I  uint32_t*)0x400440FCU) /**< \brief (ACC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_ACC_INSTANCE_ */
