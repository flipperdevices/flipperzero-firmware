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

#ifndef _SAME70_TC1_INSTANCE_
#define _SAME70_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_TC1_CCR0                   (0x40010000U) /**< \brief (TC1) Channel Control Register (channel = 0) */
  #define REG_TC1_CMR0                   (0x40010004U) /**< \brief (TC1) Channel Mode Register (channel = 0) */
  #define REG_TC1_SMMR0                  (0x40010008U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 0) */
  #define REG_TC1_RAB0                   (0x4001000CU) /**< \brief (TC1) Register AB (channel = 0) */
  #define REG_TC1_CV0                    (0x40010010U) /**< \brief (TC1) Counter Value (channel = 0) */
  #define REG_TC1_RA0                    (0x40010014U) /**< \brief (TC1) Register A (channel = 0) */
  #define REG_TC1_RB0                    (0x40010018U) /**< \brief (TC1) Register B (channel = 0) */
  #define REG_TC1_RC0                    (0x4001001CU) /**< \brief (TC1) Register C (channel = 0) */
  #define REG_TC1_SR0                    (0x40010020U) /**< \brief (TC1) Status Register (channel = 0) */
  #define REG_TC1_IER0                   (0x40010024U) /**< \brief (TC1) Interrupt Enable Register (channel = 0) */
  #define REG_TC1_IDR0                   (0x40010028U) /**< \brief (TC1) Interrupt Disable Register (channel = 0) */
  #define REG_TC1_IMR0                   (0x4001002CU) /**< \brief (TC1) Interrupt Mask Register (channel = 0) */
  #define REG_TC1_EMR0                   (0x40010030U) /**< \brief (TC1) Extended Mode Register (channel = 0) */
  #define REG_TC1_CCR1                   (0x40010040U) /**< \brief (TC1) Channel Control Register (channel = 1) */
  #define REG_TC1_CMR1                   (0x40010044U) /**< \brief (TC1) Channel Mode Register (channel = 1) */
  #define REG_TC1_SMMR1                  (0x40010048U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 1) */
  #define REG_TC1_RAB1                   (0x4001004CU) /**< \brief (TC1) Register AB (channel = 1) */
  #define REG_TC1_CV1                    (0x40010050U) /**< \brief (TC1) Counter Value (channel = 1) */
  #define REG_TC1_RA1                    (0x40010054U) /**< \brief (TC1) Register A (channel = 1) */
  #define REG_TC1_RB1                    (0x40010058U) /**< \brief (TC1) Register B (channel = 1) */
  #define REG_TC1_RC1                    (0x4001005CU) /**< \brief (TC1) Register C (channel = 1) */
  #define REG_TC1_SR1                    (0x40010060U) /**< \brief (TC1) Status Register (channel = 1) */
  #define REG_TC1_IER1                   (0x40010064U) /**< \brief (TC1) Interrupt Enable Register (channel = 1) */
  #define REG_TC1_IDR1                   (0x40010068U) /**< \brief (TC1) Interrupt Disable Register (channel = 1) */
  #define REG_TC1_IMR1                   (0x4001006CU) /**< \brief (TC1) Interrupt Mask Register (channel = 1) */
  #define REG_TC1_EMR1                   (0x40010070U) /**< \brief (TC1) Extended Mode Register (channel = 1) */
  #define REG_TC1_CCR2                   (0x40010080U) /**< \brief (TC1) Channel Control Register (channel = 2) */
  #define REG_TC1_CMR2                   (0x40010084U) /**< \brief (TC1) Channel Mode Register (channel = 2) */
  #define REG_TC1_SMMR2                  (0x40010088U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 2) */
  #define REG_TC1_RAB2                   (0x4001008CU) /**< \brief (TC1) Register AB (channel = 2) */
  #define REG_TC1_CV2                    (0x40010090U) /**< \brief (TC1) Counter Value (channel = 2) */
  #define REG_TC1_RA2                    (0x40010094U) /**< \brief (TC1) Register A (channel = 2) */
  #define REG_TC1_RB2                    (0x40010098U) /**< \brief (TC1) Register B (channel = 2) */
  #define REG_TC1_RC2                    (0x4001009CU) /**< \brief (TC1) Register C (channel = 2) */
  #define REG_TC1_SR2                    (0x400100A0U) /**< \brief (TC1) Status Register (channel = 2) */
  #define REG_TC1_IER2                   (0x400100A4U) /**< \brief (TC1) Interrupt Enable Register (channel = 2) */
  #define REG_TC1_IDR2                   (0x400100A8U) /**< \brief (TC1) Interrupt Disable Register (channel = 2) */
  #define REG_TC1_IMR2                   (0x400100ACU) /**< \brief (TC1) Interrupt Mask Register (channel = 2) */
  #define REG_TC1_EMR2                   (0x400100B0U) /**< \brief (TC1) Extended Mode Register (channel = 2) */
  #define REG_TC1_BCR                    (0x400100C0U) /**< \brief (TC1) Block Control Register */
  #define REG_TC1_BMR                    (0x400100C4U) /**< \brief (TC1) Block Mode Register */
  #define REG_TC1_QIER                   (0x400100C8U) /**< \brief (TC1) QDEC Interrupt Enable Register */
  #define REG_TC1_QIDR                   (0x400100CCU) /**< \brief (TC1) QDEC Interrupt Disable Register */
  #define REG_TC1_QIMR                   (0x400100D0U) /**< \brief (TC1) QDEC Interrupt Mask Register */
  #define REG_TC1_QISR                   (0x400100D4U) /**< \brief (TC1) QDEC Interrupt Status Register */
  #define REG_TC1_FMR                    (0x400100D8U) /**< \brief (TC1) Fault Mode Register */
  #define REG_TC1_WPMR                   (0x400100E4U) /**< \brief (TC1) Write Protection Mode Register */
  #define REG_TC1_VER                    (0x400100FCU) /**< \brief (TC1) Version Register */
#else
  #define REG_TC1_CCR0  (*(__O  uint32_t*)0x40010000U) /**< \brief (TC1) Channel Control Register (channel = 0) */
  #define REG_TC1_CMR0  (*(__IO uint32_t*)0x40010004U) /**< \brief (TC1) Channel Mode Register (channel = 0) */
  #define REG_TC1_SMMR0 (*(__IO uint32_t*)0x40010008U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 0) */
  #define REG_TC1_RAB0  (*(__I  uint32_t*)0x4001000CU) /**< \brief (TC1) Register AB (channel = 0) */
  #define REG_TC1_CV0   (*(__I  uint32_t*)0x40010010U) /**< \brief (TC1) Counter Value (channel = 0) */
  #define REG_TC1_RA0   (*(__IO uint32_t*)0x40010014U) /**< \brief (TC1) Register A (channel = 0) */
  #define REG_TC1_RB0   (*(__IO uint32_t*)0x40010018U) /**< \brief (TC1) Register B (channel = 0) */
  #define REG_TC1_RC0   (*(__IO uint32_t*)0x4001001CU) /**< \brief (TC1) Register C (channel = 0) */
  #define REG_TC1_SR0   (*(__I  uint32_t*)0x40010020U) /**< \brief (TC1) Status Register (channel = 0) */
  #define REG_TC1_IER0  (*(__O  uint32_t*)0x40010024U) /**< \brief (TC1) Interrupt Enable Register (channel = 0) */
  #define REG_TC1_IDR0  (*(__O  uint32_t*)0x40010028U) /**< \brief (TC1) Interrupt Disable Register (channel = 0) */
  #define REG_TC1_IMR0  (*(__I  uint32_t*)0x4001002CU) /**< \brief (TC1) Interrupt Mask Register (channel = 0) */
  #define REG_TC1_EMR0  (*(__IO uint32_t*)0x40010030U) /**< \brief (TC1) Extended Mode Register (channel = 0) */
  #define REG_TC1_CCR1  (*(__O  uint32_t*)0x40010040U) /**< \brief (TC1) Channel Control Register (channel = 1) */
  #define REG_TC1_CMR1  (*(__IO uint32_t*)0x40010044U) /**< \brief (TC1) Channel Mode Register (channel = 1) */
  #define REG_TC1_SMMR1 (*(__IO uint32_t*)0x40010048U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 1) */
  #define REG_TC1_RAB1  (*(__I  uint32_t*)0x4001004CU) /**< \brief (TC1) Register AB (channel = 1) */
  #define REG_TC1_CV1   (*(__I  uint32_t*)0x40010050U) /**< \brief (TC1) Counter Value (channel = 1) */
  #define REG_TC1_RA1   (*(__IO uint32_t*)0x40010054U) /**< \brief (TC1) Register A (channel = 1) */
  #define REG_TC1_RB1   (*(__IO uint32_t*)0x40010058U) /**< \brief (TC1) Register B (channel = 1) */
  #define REG_TC1_RC1   (*(__IO uint32_t*)0x4001005CU) /**< \brief (TC1) Register C (channel = 1) */
  #define REG_TC1_SR1   (*(__I  uint32_t*)0x40010060U) /**< \brief (TC1) Status Register (channel = 1) */
  #define REG_TC1_IER1  (*(__O  uint32_t*)0x40010064U) /**< \brief (TC1) Interrupt Enable Register (channel = 1) */
  #define REG_TC1_IDR1  (*(__O  uint32_t*)0x40010068U) /**< \brief (TC1) Interrupt Disable Register (channel = 1) */
  #define REG_TC1_IMR1  (*(__I  uint32_t*)0x4001006CU) /**< \brief (TC1) Interrupt Mask Register (channel = 1) */
  #define REG_TC1_EMR1  (*(__IO uint32_t*)0x40010070U) /**< \brief (TC1) Extended Mode Register (channel = 1) */
  #define REG_TC1_CCR2  (*(__O  uint32_t*)0x40010080U) /**< \brief (TC1) Channel Control Register (channel = 2) */
  #define REG_TC1_CMR2  (*(__IO uint32_t*)0x40010084U) /**< \brief (TC1) Channel Mode Register (channel = 2) */
  #define REG_TC1_SMMR2 (*(__IO uint32_t*)0x40010088U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 2) */
  #define REG_TC1_RAB2  (*(__I  uint32_t*)0x4001008CU) /**< \brief (TC1) Register AB (channel = 2) */
  #define REG_TC1_CV2   (*(__I  uint32_t*)0x40010090U) /**< \brief (TC1) Counter Value (channel = 2) */
  #define REG_TC1_RA2   (*(__IO uint32_t*)0x40010094U) /**< \brief (TC1) Register A (channel = 2) */
  #define REG_TC1_RB2   (*(__IO uint32_t*)0x40010098U) /**< \brief (TC1) Register B (channel = 2) */
  #define REG_TC1_RC2   (*(__IO uint32_t*)0x4001009CU) /**< \brief (TC1) Register C (channel = 2) */
  #define REG_TC1_SR2   (*(__I  uint32_t*)0x400100A0U) /**< \brief (TC1) Status Register (channel = 2) */
  #define REG_TC1_IER2  (*(__O  uint32_t*)0x400100A4U) /**< \brief (TC1) Interrupt Enable Register (channel = 2) */
  #define REG_TC1_IDR2  (*(__O  uint32_t*)0x400100A8U) /**< \brief (TC1) Interrupt Disable Register (channel = 2) */
  #define REG_TC1_IMR2  (*(__I  uint32_t*)0x400100ACU) /**< \brief (TC1) Interrupt Mask Register (channel = 2) */
  #define REG_TC1_EMR2  (*(__IO uint32_t*)0x400100B0U) /**< \brief (TC1) Extended Mode Register (channel = 2) */
  #define REG_TC1_BCR   (*(__O  uint32_t*)0x400100C0U) /**< \brief (TC1) Block Control Register */
  #define REG_TC1_BMR   (*(__IO uint32_t*)0x400100C4U) /**< \brief (TC1) Block Mode Register */
  #define REG_TC1_QIER  (*(__O  uint32_t*)0x400100C8U) /**< \brief (TC1) QDEC Interrupt Enable Register */
  #define REG_TC1_QIDR  (*(__O  uint32_t*)0x400100CCU) /**< \brief (TC1) QDEC Interrupt Disable Register */
  #define REG_TC1_QIMR  (*(__I  uint32_t*)0x400100D0U) /**< \brief (TC1) QDEC Interrupt Mask Register */
  #define REG_TC1_QISR  (*(__I  uint32_t*)0x400100D4U) /**< \brief (TC1) QDEC Interrupt Status Register */
  #define REG_TC1_FMR   (*(__IO uint32_t*)0x400100D8U) /**< \brief (TC1) Fault Mode Register */
  #define REG_TC1_WPMR  (*(__IO uint32_t*)0x400100E4U) /**< \brief (TC1) Write Protection Mode Register */
  #define REG_TC1_VER   (*(__I  uint32_t*)0x400100FCU) /**< \brief (TC1) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_TC1_INSTANCE_ */
