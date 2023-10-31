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

#ifndef _SAME70_TWIHS2_INSTANCE_
#define _SAME70_TWIHS2_INSTANCE_

/* ========== Register definition for TWIHS2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_TWIHS2_CR                     (0x40060000U) /**< \brief (TWIHS2) Control Register */
  #define REG_TWIHS2_MMR                    (0x40060004U) /**< \brief (TWIHS2) Master Mode Register */
  #define REG_TWIHS2_SMR                    (0x40060008U) /**< \brief (TWIHS2) Slave Mode Register */
  #define REG_TWIHS2_IADR                   (0x4006000CU) /**< \brief (TWIHS2) Internal Address Register */
  #define REG_TWIHS2_CWGR                   (0x40060010U) /**< \brief (TWIHS2) Clock Waveform Generator Register */
  #define REG_TWIHS2_SR                     (0x40060020U) /**< \brief (TWIHS2) Status Register */
  #define REG_TWIHS2_IER                    (0x40060024U) /**< \brief (TWIHS2) Interrupt Enable Register */
  #define REG_TWIHS2_IDR                    (0x40060028U) /**< \brief (TWIHS2) Interrupt Disable Register */
  #define REG_TWIHS2_IMR                    (0x4006002CU) /**< \brief (TWIHS2) Interrupt Mask Register */
  #define REG_TWIHS2_RHR                    (0x40060030U) /**< \brief (TWIHS2) Receive Holding Register */
  #define REG_TWIHS2_THR                    (0x40060034U) /**< \brief (TWIHS2) Transmit Holding Register */
  #define REG_TWIHS2_SMBTR                  (0x40060038U) /**< \brief (TWIHS2) SMBus Timing Register */
  #define REG_TWIHS2_FILTR                  (0x40060044U) /**< \brief (TWIHS2) Filter Register */
  #define REG_TWIHS2_SWMR                   (0x4006004CU) /**< \brief (TWIHS2) SleepWalking Matching Register */
  #define REG_TWIHS2_DR                     (0x400600D0U) /**< \brief (TWIHS2) Debug Register */
  #define REG_TWIHS2_WPMR                   (0x400600E4U) /**< \brief (TWIHS2) Write Protection Mode Register */
  #define REG_TWIHS2_WPSR                   (0x400600E8U) /**< \brief (TWIHS2) Write Protection Status Register */
  #define REG_TWIHS2_VER                    (0x400600FCU) /**< \brief (TWIHS2) Version Register */
#else
  #define REG_TWIHS2_CR    (*(__O  uint32_t*)0x40060000U) /**< \brief (TWIHS2) Control Register */
  #define REG_TWIHS2_MMR   (*(__IO uint32_t*)0x40060004U) /**< \brief (TWIHS2) Master Mode Register */
  #define REG_TWIHS2_SMR   (*(__IO uint32_t*)0x40060008U) /**< \brief (TWIHS2) Slave Mode Register */
  #define REG_TWIHS2_IADR  (*(__IO uint32_t*)0x4006000CU) /**< \brief (TWIHS2) Internal Address Register */
  #define REG_TWIHS2_CWGR  (*(__IO uint32_t*)0x40060010U) /**< \brief (TWIHS2) Clock Waveform Generator Register */
  #define REG_TWIHS2_SR    (*(__I  uint32_t*)0x40060020U) /**< \brief (TWIHS2) Status Register */
  #define REG_TWIHS2_IER   (*(__O  uint32_t*)0x40060024U) /**< \brief (TWIHS2) Interrupt Enable Register */
  #define REG_TWIHS2_IDR   (*(__O  uint32_t*)0x40060028U) /**< \brief (TWIHS2) Interrupt Disable Register */
  #define REG_TWIHS2_IMR   (*(__I  uint32_t*)0x4006002CU) /**< \brief (TWIHS2) Interrupt Mask Register */
  #define REG_TWIHS2_RHR   (*(__I  uint32_t*)0x40060030U) /**< \brief (TWIHS2) Receive Holding Register */
  #define REG_TWIHS2_THR   (*(__O  uint32_t*)0x40060034U) /**< \brief (TWIHS2) Transmit Holding Register */
  #define REG_TWIHS2_SMBTR (*(__IO uint32_t*)0x40060038U) /**< \brief (TWIHS2) SMBus Timing Register */
  #define REG_TWIHS2_FILTR (*(__IO uint32_t*)0x40060044U) /**< \brief (TWIHS2) Filter Register */
  #define REG_TWIHS2_SWMR  (*(__IO uint32_t*)0x4006004CU) /**< \brief (TWIHS2) SleepWalking Matching Register */
  #define REG_TWIHS2_DR    (*(__I  uint32_t*)0x400600D0U) /**< \brief (TWIHS2) Debug Register */
  #define REG_TWIHS2_WPMR  (*(__IO uint32_t*)0x400600E4U) /**< \brief (TWIHS2) Write Protection Mode Register */
  #define REG_TWIHS2_WPSR  (*(__I  uint32_t*)0x400600E8U) /**< \brief (TWIHS2) Write Protection Status Register */
  #define REG_TWIHS2_VER   (*(__I  uint32_t*)0x400600FCU) /**< \brief (TWIHS2) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_TWIHS2_INSTANCE_ */
