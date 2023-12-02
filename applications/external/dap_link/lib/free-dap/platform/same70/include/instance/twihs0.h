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

#ifndef _SAME70_TWIHS0_INSTANCE_
#define _SAME70_TWIHS0_INSTANCE_

/* ========== Register definition for TWIHS0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_TWIHS0_CR                     (0x40018000U) /**< \brief (TWIHS0) Control Register */
  #define REG_TWIHS0_MMR                    (0x40018004U) /**< \brief (TWIHS0) Master Mode Register */
  #define REG_TWIHS0_SMR                    (0x40018008U) /**< \brief (TWIHS0) Slave Mode Register */
  #define REG_TWIHS0_IADR                   (0x4001800CU) /**< \brief (TWIHS0) Internal Address Register */
  #define REG_TWIHS0_CWGR                   (0x40018010U) /**< \brief (TWIHS0) Clock Waveform Generator Register */
  #define REG_TWIHS0_SR                     (0x40018020U) /**< \brief (TWIHS0) Status Register */
  #define REG_TWIHS0_IER                    (0x40018024U) /**< \brief (TWIHS0) Interrupt Enable Register */
  #define REG_TWIHS0_IDR                    (0x40018028U) /**< \brief (TWIHS0) Interrupt Disable Register */
  #define REG_TWIHS0_IMR                    (0x4001802CU) /**< \brief (TWIHS0) Interrupt Mask Register */
  #define REG_TWIHS0_RHR                    (0x40018030U) /**< \brief (TWIHS0) Receive Holding Register */
  #define REG_TWIHS0_THR                    (0x40018034U) /**< \brief (TWIHS0) Transmit Holding Register */
  #define REG_TWIHS0_SMBTR                  (0x40018038U) /**< \brief (TWIHS0) SMBus Timing Register */
  #define REG_TWIHS0_FILTR                  (0x40018044U) /**< \brief (TWIHS0) Filter Register */
  #define REG_TWIHS0_SWMR                   (0x4001804CU) /**< \brief (TWIHS0) SleepWalking Matching Register */
  #define REG_TWIHS0_DR                     (0x400180D0U) /**< \brief (TWIHS0) Debug Register */
  #define REG_TWIHS0_WPMR                   (0x400180E4U) /**< \brief (TWIHS0) Write Protection Mode Register */
  #define REG_TWIHS0_WPSR                   (0x400180E8U) /**< \brief (TWIHS0) Write Protection Status Register */
  #define REG_TWIHS0_VER                    (0x400180FCU) /**< \brief (TWIHS0) Version Register */
#else
  #define REG_TWIHS0_CR    (*(__O  uint32_t*)0x40018000U) /**< \brief (TWIHS0) Control Register */
  #define REG_TWIHS0_MMR   (*(__IO uint32_t*)0x40018004U) /**< \brief (TWIHS0) Master Mode Register */
  #define REG_TWIHS0_SMR   (*(__IO uint32_t*)0x40018008U) /**< \brief (TWIHS0) Slave Mode Register */
  #define REG_TWIHS0_IADR  (*(__IO uint32_t*)0x4001800CU) /**< \brief (TWIHS0) Internal Address Register */
  #define REG_TWIHS0_CWGR  (*(__IO uint32_t*)0x40018010U) /**< \brief (TWIHS0) Clock Waveform Generator Register */
  #define REG_TWIHS0_SR    (*(__I  uint32_t*)0x40018020U) /**< \brief (TWIHS0) Status Register */
  #define REG_TWIHS0_IER   (*(__O  uint32_t*)0x40018024U) /**< \brief (TWIHS0) Interrupt Enable Register */
  #define REG_TWIHS0_IDR   (*(__O  uint32_t*)0x40018028U) /**< \brief (TWIHS0) Interrupt Disable Register */
  #define REG_TWIHS0_IMR   (*(__I  uint32_t*)0x4001802CU) /**< \brief (TWIHS0) Interrupt Mask Register */
  #define REG_TWIHS0_RHR   (*(__I  uint32_t*)0x40018030U) /**< \brief (TWIHS0) Receive Holding Register */
  #define REG_TWIHS0_THR   (*(__O  uint32_t*)0x40018034U) /**< \brief (TWIHS0) Transmit Holding Register */
  #define REG_TWIHS0_SMBTR (*(__IO uint32_t*)0x40018038U) /**< \brief (TWIHS0) SMBus Timing Register */
  #define REG_TWIHS0_FILTR (*(__IO uint32_t*)0x40018044U) /**< \brief (TWIHS0) Filter Register */
  #define REG_TWIHS0_SWMR  (*(__IO uint32_t*)0x4001804CU) /**< \brief (TWIHS0) SleepWalking Matching Register */
  #define REG_TWIHS0_DR    (*(__I  uint32_t*)0x400180D0U) /**< \brief (TWIHS0) Debug Register */
  #define REG_TWIHS0_WPMR  (*(__IO uint32_t*)0x400180E4U) /**< \brief (TWIHS0) Write Protection Mode Register */
  #define REG_TWIHS0_WPSR  (*(__I  uint32_t*)0x400180E8U) /**< \brief (TWIHS0) Write Protection Status Register */
  #define REG_TWIHS0_VER   (*(__I  uint32_t*)0x400180FCU) /**< \brief (TWIHS0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_TWIHS0_INSTANCE_ */
