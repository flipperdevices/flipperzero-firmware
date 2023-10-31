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

#ifndef _SAME70_HSMCI_INSTANCE_
#define _SAME70_HSMCI_INSTANCE_

/* ========== Register definition for HSMCI peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_HSMCI_CR                       (0x40000000U) /**< \brief (HSMCI) Control Register */
  #define REG_HSMCI_MR                       (0x40000004U) /**< \brief (HSMCI) Mode Register */
  #define REG_HSMCI_DTOR                     (0x40000008U) /**< \brief (HSMCI) Data Timeout Register */
  #define REG_HSMCI_SDCR                     (0x4000000CU) /**< \brief (HSMCI) SD/SDIO Card Register */
  #define REG_HSMCI_ARGR                     (0x40000010U) /**< \brief (HSMCI) Argument Register */
  #define REG_HSMCI_CMDR                     (0x40000014U) /**< \brief (HSMCI) Command Register */
  #define REG_HSMCI_BLKR                     (0x40000018U) /**< \brief (HSMCI) Block Register */
  #define REG_HSMCI_CSTOR                    (0x4000001CU) /**< \brief (HSMCI) Completion Signal Timeout Register */
  #define REG_HSMCI_RSPR                     (0x40000020U) /**< \brief (HSMCI) Response Register */
  #define REG_HSMCI_RDR                      (0x40000030U) /**< \brief (HSMCI) Receive Data Register */
  #define REG_HSMCI_TDR                      (0x40000034U) /**< \brief (HSMCI) Transmit Data Register */
  #define REG_HSMCI_SR                       (0x40000040U) /**< \brief (HSMCI) Status Register */
  #define REG_HSMCI_IER                      (0x40000044U) /**< \brief (HSMCI) Interrupt Enable Register */
  #define REG_HSMCI_IDR                      (0x40000048U) /**< \brief (HSMCI) Interrupt Disable Register */
  #define REG_HSMCI_IMR                      (0x4000004CU) /**< \brief (HSMCI) Interrupt Mask Register */
  #define REG_HSMCI_DMA                      (0x40000050U) /**< \brief (HSMCI) DMA Configuration Register */
  #define REG_HSMCI_CFG                      (0x40000054U) /**< \brief (HSMCI) Configuration Register */
  #define REG_HSMCI_WPMR                     (0x400000E4U) /**< \brief (HSMCI) Write Protection Mode Register */
  #define REG_HSMCI_WPSR                     (0x400000E8U) /**< \brief (HSMCI) Write Protection Status Register */
  #define REG_HSMCI_VERSION                  (0x400000FCU) /**< \brief (HSMCI) Version Register */
  #define REG_HSMCI_FIFO                     (0x40000200U) /**< \brief (HSMCI) FIFO Memory Aperture0 */
#else
  #define REG_HSMCI_CR      (*(__O  uint32_t*)0x40000000U) /**< \brief (HSMCI) Control Register */
  #define REG_HSMCI_MR      (*(__IO uint32_t*)0x40000004U) /**< \brief (HSMCI) Mode Register */
  #define REG_HSMCI_DTOR    (*(__IO uint32_t*)0x40000008U) /**< \brief (HSMCI) Data Timeout Register */
  #define REG_HSMCI_SDCR    (*(__IO uint32_t*)0x4000000CU) /**< \brief (HSMCI) SD/SDIO Card Register */
  #define REG_HSMCI_ARGR    (*(__IO uint32_t*)0x40000010U) /**< \brief (HSMCI) Argument Register */
  #define REG_HSMCI_CMDR    (*(__O  uint32_t*)0x40000014U) /**< \brief (HSMCI) Command Register */
  #define REG_HSMCI_BLKR    (*(__IO uint32_t*)0x40000018U) /**< \brief (HSMCI) Block Register */
  #define REG_HSMCI_CSTOR   (*(__IO uint32_t*)0x4000001CU) /**< \brief (HSMCI) Completion Signal Timeout Register */
  #define REG_HSMCI_RSPR    (*(__I  uint32_t*)0x40000020U) /**< \brief (HSMCI) Response Register */
  #define REG_HSMCI_RDR     (*(__I  uint32_t*)0x40000030U) /**< \brief (HSMCI) Receive Data Register */
  #define REG_HSMCI_TDR     (*(__O  uint32_t*)0x40000034U) /**< \brief (HSMCI) Transmit Data Register */
  #define REG_HSMCI_SR      (*(__I  uint32_t*)0x40000040U) /**< \brief (HSMCI) Status Register */
  #define REG_HSMCI_IER     (*(__O  uint32_t*)0x40000044U) /**< \brief (HSMCI) Interrupt Enable Register */
  #define REG_HSMCI_IDR     (*(__O  uint32_t*)0x40000048U) /**< \brief (HSMCI) Interrupt Disable Register */
  #define REG_HSMCI_IMR     (*(__I  uint32_t*)0x4000004CU) /**< \brief (HSMCI) Interrupt Mask Register */
  #define REG_HSMCI_DMA     (*(__IO uint32_t*)0x40000050U) /**< \brief (HSMCI) DMA Configuration Register */
  #define REG_HSMCI_CFG     (*(__IO uint32_t*)0x40000054U) /**< \brief (HSMCI) Configuration Register */
  #define REG_HSMCI_WPMR    (*(__IO uint32_t*)0x400000E4U) /**< \brief (HSMCI) Write Protection Mode Register */
  #define REG_HSMCI_WPSR    (*(__I  uint32_t*)0x400000E8U) /**< \brief (HSMCI) Write Protection Status Register */
  #define REG_HSMCI_VERSION (*(__I  uint32_t*)0x400000FCU) /**< \brief (HSMCI) Version Register */
  #define REG_HSMCI_FIFO    (*(__IO uint32_t*)0x40000200U) /**< \brief (HSMCI) FIFO Memory Aperture0 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_HSMCI_INSTANCE_ */
