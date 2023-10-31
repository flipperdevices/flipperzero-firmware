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

#ifndef _SAME70_SDRAMC_INSTANCE_
#define _SAME70_SDRAMC_INSTANCE_

/* ========== Register definition for SDRAMC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SDRAMC_MR                         (0x40084000U) /**< \brief (SDRAMC) SDRAMC Mode Register */
  #define REG_SDRAMC_TR                         (0x40084004U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
  #define REG_SDRAMC_CR                         (0x40084008U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
  #define REG_SDRAMC_LPR                        (0x40084010U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
  #define REG_SDRAMC_IER                        (0x40084014U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
  #define REG_SDRAMC_IDR                        (0x40084018U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
  #define REG_SDRAMC_IMR                        (0x4008401CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
  #define REG_SDRAMC_ISR                        (0x40084020U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
  #define REG_SDRAMC_MDR                        (0x40084024U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
  #define REG_SDRAMC_CFR1                       (0x40084028U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
  #define REG_SDRAMC_OCMS                       (0x4008402CU) /**< \brief (SDRAMC) SDRAMC OCMS Register */
  #define REG_SDRAMC_OCMS_KEY1                  (0x40084030U) /**< \brief (SDRAMC) SDRAMC OCMS KEY1 Register */
  #define REG_SDRAMC_OCMS_KEY2                  (0x40084034U) /**< \brief (SDRAMC) SDRAMC OCMS KEY2 Register */
  #define REG_SDRAMC_VERSION                    (0x400840FCU) /**< \brief (SDRAMC) SDRAMC Version Register */
#else
  #define REG_SDRAMC_MR        (*(__IO uint32_t*)0x40084000U) /**< \brief (SDRAMC) SDRAMC Mode Register */
  #define REG_SDRAMC_TR        (*(__IO uint32_t*)0x40084004U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
  #define REG_SDRAMC_CR        (*(__IO uint32_t*)0x40084008U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
  #define REG_SDRAMC_LPR       (*(__IO uint32_t*)0x40084010U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
  #define REG_SDRAMC_IER       (*(__O  uint32_t*)0x40084014U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
  #define REG_SDRAMC_IDR       (*(__O  uint32_t*)0x40084018U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
  #define REG_SDRAMC_IMR       (*(__I  uint32_t*)0x4008401CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
  #define REG_SDRAMC_ISR       (*(__I  uint32_t*)0x40084020U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
  #define REG_SDRAMC_MDR       (*(__IO uint32_t*)0x40084024U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
  #define REG_SDRAMC_CFR1      (*(__IO uint32_t*)0x40084028U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
  #define REG_SDRAMC_OCMS      (*(__IO uint32_t*)0x4008402CU) /**< \brief (SDRAMC) SDRAMC OCMS Register */
  #define REG_SDRAMC_OCMS_KEY1 (*(__O  uint32_t*)0x40084030U) /**< \brief (SDRAMC) SDRAMC OCMS KEY1 Register */
  #define REG_SDRAMC_OCMS_KEY2 (*(__O  uint32_t*)0x40084034U) /**< \brief (SDRAMC) SDRAMC OCMS KEY2 Register */
  #define REG_SDRAMC_VERSION   (*(__I  uint32_t*)0x400840FCU) /**< \brief (SDRAMC) SDRAMC Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_SDRAMC_INSTANCE_ */
