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

#ifndef _SAME70_TWIHS1_INSTANCE_
#define _SAME70_TWIHS1_INSTANCE_

/* ========== Register definition for TWIHS1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_TWIHS1_CR                     (0x4001C000U) /**< \brief (TWIHS1) Control Register */
  #define REG_TWIHS1_MMR                    (0x4001C004U) /**< \brief (TWIHS1) Master Mode Register */
  #define REG_TWIHS1_SMR                    (0x4001C008U) /**< \brief (TWIHS1) Slave Mode Register */
  #define REG_TWIHS1_IADR                   (0x4001C00CU) /**< \brief (TWIHS1) Internal Address Register */
  #define REG_TWIHS1_CWGR                   (0x4001C010U) /**< \brief (TWIHS1) Clock Waveform Generator Register */
  #define REG_TWIHS1_SR                     (0x4001C020U) /**< \brief (TWIHS1) Status Register */
  #define REG_TWIHS1_IER                    (0x4001C024U) /**< \brief (TWIHS1) Interrupt Enable Register */
  #define REG_TWIHS1_IDR                    (0x4001C028U) /**< \brief (TWIHS1) Interrupt Disable Register */
  #define REG_TWIHS1_IMR                    (0x4001C02CU) /**< \brief (TWIHS1) Interrupt Mask Register */
  #define REG_TWIHS1_RHR                    (0x4001C030U) /**< \brief (TWIHS1) Receive Holding Register */
  #define REG_TWIHS1_THR                    (0x4001C034U) /**< \brief (TWIHS1) Transmit Holding Register */
  #define REG_TWIHS1_SMBTR                  (0x4001C038U) /**< \brief (TWIHS1) SMBus Timing Register */
  #define REG_TWIHS1_FILTR                  (0x4001C044U) /**< \brief (TWIHS1) Filter Register */
  #define REG_TWIHS1_SWMR                   (0x4001C04CU) /**< \brief (TWIHS1) SleepWalking Matching Register */
  #define REG_TWIHS1_DR                     (0x4001C0D0U) /**< \brief (TWIHS1) Debug Register */
  #define REG_TWIHS1_WPMR                   (0x4001C0E4U) /**< \brief (TWIHS1) Write Protection Mode Register */
  #define REG_TWIHS1_WPSR                   (0x4001C0E8U) /**< \brief (TWIHS1) Write Protection Status Register */
  #define REG_TWIHS1_VER                    (0x4001C0FCU) /**< \brief (TWIHS1) Version Register */
#else
  #define REG_TWIHS1_CR    (*(__O  uint32_t*)0x4001C000U) /**< \brief (TWIHS1) Control Register */
  #define REG_TWIHS1_MMR   (*(__IO uint32_t*)0x4001C004U) /**< \brief (TWIHS1) Master Mode Register */
  #define REG_TWIHS1_SMR   (*(__IO uint32_t*)0x4001C008U) /**< \brief (TWIHS1) Slave Mode Register */
  #define REG_TWIHS1_IADR  (*(__IO uint32_t*)0x4001C00CU) /**< \brief (TWIHS1) Internal Address Register */
  #define REG_TWIHS1_CWGR  (*(__IO uint32_t*)0x4001C010U) /**< \brief (TWIHS1) Clock Waveform Generator Register */
  #define REG_TWIHS1_SR    (*(__I  uint32_t*)0x4001C020U) /**< \brief (TWIHS1) Status Register */
  #define REG_TWIHS1_IER   (*(__O  uint32_t*)0x4001C024U) /**< \brief (TWIHS1) Interrupt Enable Register */
  #define REG_TWIHS1_IDR   (*(__O  uint32_t*)0x4001C028U) /**< \brief (TWIHS1) Interrupt Disable Register */
  #define REG_TWIHS1_IMR   (*(__I  uint32_t*)0x4001C02CU) /**< \brief (TWIHS1) Interrupt Mask Register */
  #define REG_TWIHS1_RHR   (*(__I  uint32_t*)0x4001C030U) /**< \brief (TWIHS1) Receive Holding Register */
  #define REG_TWIHS1_THR   (*(__O  uint32_t*)0x4001C034U) /**< \brief (TWIHS1) Transmit Holding Register */
  #define REG_TWIHS1_SMBTR (*(__IO uint32_t*)0x4001C038U) /**< \brief (TWIHS1) SMBus Timing Register */
  #define REG_TWIHS1_FILTR (*(__IO uint32_t*)0x4001C044U) /**< \brief (TWIHS1) Filter Register */
  #define REG_TWIHS1_SWMR  (*(__IO uint32_t*)0x4001C04CU) /**< \brief (TWIHS1) SleepWalking Matching Register */
  #define REG_TWIHS1_DR    (*(__I  uint32_t*)0x4001C0D0U) /**< \brief (TWIHS1) Debug Register */
  #define REG_TWIHS1_WPMR  (*(__IO uint32_t*)0x4001C0E4U) /**< \brief (TWIHS1) Write Protection Mode Register */
  #define REG_TWIHS1_WPSR  (*(__I  uint32_t*)0x4001C0E8U) /**< \brief (TWIHS1) Write Protection Status Register */
  #define REG_TWIHS1_VER   (*(__I  uint32_t*)0x4001C0FCU) /**< \brief (TWIHS1) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_TWIHS1_INSTANCE_ */
