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

#ifndef _SAME70_ICM_INSTANCE_
#define _SAME70_ICM_INSTANCE_

/* ========== Register definition for ICM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_ICM_CFG                       (0x40048000U) /**< \brief (ICM) Configuration Register */
  #define REG_ICM_CTRL                      (0x40048004U) /**< \brief (ICM) Control Register */
  #define REG_ICM_SR                        (0x40048008U) /**< \brief (ICM) Status Register */
  #define REG_ICM_IER                       (0x40048010U) /**< \brief (ICM) Interrupt Enable Register */
  #define REG_ICM_IDR                       (0x40048014U) /**< \brief (ICM) Interrupt Disable Register */
  #define REG_ICM_IMR                       (0x40048018U) /**< \brief (ICM) Interrupt Mask Register */
  #define REG_ICM_ISR                       (0x4004801CU) /**< \brief (ICM) Interrupt Status Register */
  #define REG_ICM_UASR                      (0x40048020U) /**< \brief (ICM) Undefined Access Status Register */
  #define REG_ICM_DSCR                      (0x40048030U) /**< \brief (ICM) Region Descriptor Area Start Address Register */
  #define REG_ICM_HASH                      (0x40048034U) /**< \brief (ICM) Region Hash Area Start Address Register */
  #define REG_ICM_UIHVAL                    (0x40048038U) /**< \brief (ICM) User Initial Hash Value 0 Register */
  #define REG_ICM_ADDRSIZE                  (0x400480ECU) /**< \brief (ICM) Address Size Register */
  #define REG_ICM_IPNAME                    (0x400480F0U) /**< \brief (ICM) IP Name 1 Register */
  #define REG_ICM_FEATURES                  (0x400480F8U) /**< \brief (ICM) Feature Register */
  #define REG_ICM_VERSION                   (0x400480FCU) /**< \brief (ICM) Version Register */
#else
  #define REG_ICM_CFG      (*(__IO uint32_t*)0x40048000U) /**< \brief (ICM) Configuration Register */
  #define REG_ICM_CTRL     (*(__O  uint32_t*)0x40048004U) /**< \brief (ICM) Control Register */
  #define REG_ICM_SR       (*(__I  uint32_t*)0x40048008U) /**< \brief (ICM) Status Register */
  #define REG_ICM_IER      (*(__O  uint32_t*)0x40048010U) /**< \brief (ICM) Interrupt Enable Register */
  #define REG_ICM_IDR      (*(__O  uint32_t*)0x40048014U) /**< \brief (ICM) Interrupt Disable Register */
  #define REG_ICM_IMR      (*(__I  uint32_t*)0x40048018U) /**< \brief (ICM) Interrupt Mask Register */
  #define REG_ICM_ISR      (*(__I  uint32_t*)0x4004801CU) /**< \brief (ICM) Interrupt Status Register */
  #define REG_ICM_UASR     (*(__I  uint32_t*)0x40048020U) /**< \brief (ICM) Undefined Access Status Register */
  #define REG_ICM_DSCR     (*(__IO uint32_t*)0x40048030U) /**< \brief (ICM) Region Descriptor Area Start Address Register */
  #define REG_ICM_HASH     (*(__IO uint32_t*)0x40048034U) /**< \brief (ICM) Region Hash Area Start Address Register */
  #define REG_ICM_UIHVAL   (*(__O  uint32_t*)0x40048038U) /**< \brief (ICM) User Initial Hash Value 0 Register */
  #define REG_ICM_ADDRSIZE (*(__I  uint32_t*)0x400480ECU) /**< \brief (ICM) Address Size Register */
  #define REG_ICM_IPNAME   (*(__I  uint32_t*)0x400480F0U) /**< \brief (ICM) IP Name 1 Register */
  #define REG_ICM_FEATURES (*(__I  uint32_t*)0x400480F8U) /**< \brief (ICM) Feature Register */
  #define REG_ICM_VERSION  (*(__I  uint32_t*)0x400480FCU) /**< \brief (ICM) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_ICM_INSTANCE_ */
