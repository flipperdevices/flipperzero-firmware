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

#ifndef _SAME70_MATRIX_INSTANCE_
#define _SAME70_MATRIX_INSTANCE_

/* ========== Register definition for MATRIX peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_MATRIX_MCFG0                    (0x40088000U) /**< \brief (MATRIX) Master Configuration Register 0 */
  #define REG_MATRIX_MCFG1                    (0x40088004U) /**< \brief (MATRIX) Master Configuration Register 1 */
  #define REG_MATRIX_MCFG2                    (0x40088008U) /**< \brief (MATRIX) Master Configuration Register 2 */
  #define REG_MATRIX_MCFG3                    (0x4008800CU) /**< \brief (MATRIX) Master Configuration Register 3 */
  #define REG_MATRIX_MCFG4                    (0x40088010U) /**< \brief (MATRIX) Master Configuration Register 4 */
  #define REG_MATRIX_MCFG5                    (0x40088014U) /**< \brief (MATRIX) Master Configuration Register 5 */
  #define REG_MATRIX_MCFG6                    (0x40088018U) /**< \brief (MATRIX) Master Configuration Register 6 */
  #define REG_MATRIX_MCFG8                    (0x40088020U) /**< \brief (MATRIX) Master Configuration Register 8 */
  #define REG_MATRIX_MCFG9                    (0x40088024U) /**< \brief (MATRIX) Master Configuration Register 9 */
  #define REG_MATRIX_MCFG10                   (0x40088028U) /**< \brief (MATRIX) Master Configuration Register 10 */
  #define REG_MATRIX_MCFG11                   (0x4008802CU) /**< \brief (MATRIX) Master Configuration Register 11 */
  #define REG_MATRIX_MCFG12                   (0x40088030U) /**< \brief (MATRIX) Master Configuration Register 12 */
  #define REG_MATRIX_SCFG                     (0x40088040U) /**< \brief (MATRIX) Slave Configuration Register */
  #define REG_MATRIX_PRAS0                    (0x40088080U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
  #define REG_MATRIX_PRBS0                    (0x40088084U) /**< \brief (MATRIX) Priority Register B for Slave 0 */
  #define REG_MATRIX_PRAS1                    (0x40088088U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
  #define REG_MATRIX_PRBS1                    (0x4008808CU) /**< \brief (MATRIX) Priority Register B for Slave 1 */
  #define REG_MATRIX_PRAS2                    (0x40088090U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
  #define REG_MATRIX_PRBS2                    (0x40088094U) /**< \brief (MATRIX) Priority Register B for Slave 2 */
  #define REG_MATRIX_PRAS3                    (0x40088098U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
  #define REG_MATRIX_PRBS3                    (0x4008809CU) /**< \brief (MATRIX) Priority Register B for Slave 3 */
  #define REG_MATRIX_PRAS4                    (0x400880A0U) /**< \brief (MATRIX) Priority Register A for Slave 4 */
  #define REG_MATRIX_PRBS4                    (0x400880A4U) /**< \brief (MATRIX) Priority Register B for Slave 4 */
  #define REG_MATRIX_PRAS5                    (0x400880A8U) /**< \brief (MATRIX) Priority Register A for Slave 5 */
  #define REG_MATRIX_PRBS5                    (0x400880ACU) /**< \brief (MATRIX) Priority Register B for Slave 5 */
  #define REG_MATRIX_PRAS6                    (0x400880B0U) /**< \brief (MATRIX) Priority Register A for Slave 6 */
  #define REG_MATRIX_PRBS6                    (0x400880B4U) /**< \brief (MATRIX) Priority Register B for Slave 6 */
  #define REG_MATRIX_PRAS7                    (0x400880B8U) /**< \brief (MATRIX) Priority Register A for Slave 7 */
  #define REG_MATRIX_PRBS7                    (0x400880BCU) /**< \brief (MATRIX) Priority Register B for Slave 7 */
  #define REG_MATRIX_PRAS8                    (0x400880C0U) /**< \brief (MATRIX) Priority Register A for Slave 8 */
  #define REG_MATRIX_PRBS8                    (0x400880C4U) /**< \brief (MATRIX) Priority Register B for Slave 8 */
  #define REG_MATRIX_MRCR                     (0x40088100U) /**< \brief (MATRIX) Master Remap Control Register */
  #define REG_CCFG_CAN0                       (0x40088110U) /**< \brief (MATRIX) CAN0 Configuration Register */
  #define REG_CCFG_SYSIO                      (0x40088114U) /**< \brief (MATRIX) System I/O and CAN1 Configuration Register */
  #define REG_CCFG_PCCR                       (0x40088118U) /**< \brief (MATRIX) Peripheral Clock Configuration Register */
  #define REG_CCFG_DYNCKG                     (0x4008811CU) /**< \brief (MATRIX) Dynamic Clock Gating Register */
  #define REG_CCFG_SMCNFCS                    (0x40088124U) /**< \brief (MATRIX) SMC NAND Flash Chip Select Configuration Register */
  #define REG_MATRIX_WPMR                     (0x400881E4U) /**< \brief (MATRIX) Write Protection Mode Register */
  #define REG_MATRIX_WPSR                     (0x400881E8U) /**< \brief (MATRIX) Write Protection Status Register */
  #define REG_MATRIX_VERSION                  (0x400881FCU) /**< \brief (MATRIX) Version Register */
#else
  #define REG_MATRIX_MCFG0   (*(__IO uint32_t*)0x40088000U) /**< \brief (MATRIX) Master Configuration Register 0 */
  #define REG_MATRIX_MCFG1   (*(__IO uint32_t*)0x40088004U) /**< \brief (MATRIX) Master Configuration Register 1 */
  #define REG_MATRIX_MCFG2   (*(__IO uint32_t*)0x40088008U) /**< \brief (MATRIX) Master Configuration Register 2 */
  #define REG_MATRIX_MCFG3   (*(__IO uint32_t*)0x4008800CU) /**< \brief (MATRIX) Master Configuration Register 3 */
  #define REG_MATRIX_MCFG4   (*(__IO uint32_t*)0x40088010U) /**< \brief (MATRIX) Master Configuration Register 4 */
  #define REG_MATRIX_MCFG5   (*(__IO uint32_t*)0x40088014U) /**< \brief (MATRIX) Master Configuration Register 5 */
  #define REG_MATRIX_MCFG6   (*(__IO uint32_t*)0x40088018U) /**< \brief (MATRIX) Master Configuration Register 6 */
  #define REG_MATRIX_MCFG8   (*(__IO uint32_t*)0x40088020U) /**< \brief (MATRIX) Master Configuration Register 8 */
  #define REG_MATRIX_MCFG9   (*(__IO uint32_t*)0x40088024U) /**< \brief (MATRIX) Master Configuration Register 9 */
  #define REG_MATRIX_MCFG10  (*(__IO uint32_t*)0x40088028U) /**< \brief (MATRIX) Master Configuration Register 10 */
  #define REG_MATRIX_MCFG11  (*(__IO uint32_t*)0x4008802CU) /**< \brief (MATRIX) Master Configuration Register 11 */
  #define REG_MATRIX_MCFG12  (*(__IO uint32_t*)0x40088030U) /**< \brief (MATRIX) Master Configuration Register 12 */
  #define REG_MATRIX_SCFG    (*(__IO uint32_t*)0x40088040U) /**< \brief (MATRIX) Slave Configuration Register */
  #define REG_MATRIX_PRAS0   (*(__IO uint32_t*)0x40088080U) /**< \brief (MATRIX) Priority Register A for Slave 0 */
  #define REG_MATRIX_PRBS0   (*(__IO uint32_t*)0x40088084U) /**< \brief (MATRIX) Priority Register B for Slave 0 */
  #define REG_MATRIX_PRAS1   (*(__IO uint32_t*)0x40088088U) /**< \brief (MATRIX) Priority Register A for Slave 1 */
  #define REG_MATRIX_PRBS1   (*(__IO uint32_t*)0x4008808CU) /**< \brief (MATRIX) Priority Register B for Slave 1 */
  #define REG_MATRIX_PRAS2   (*(__IO uint32_t*)0x40088090U) /**< \brief (MATRIX) Priority Register A for Slave 2 */
  #define REG_MATRIX_PRBS2   (*(__IO uint32_t*)0x40088094U) /**< \brief (MATRIX) Priority Register B for Slave 2 */
  #define REG_MATRIX_PRAS3   (*(__IO uint32_t*)0x40088098U) /**< \brief (MATRIX) Priority Register A for Slave 3 */
  #define REG_MATRIX_PRBS3   (*(__IO uint32_t*)0x4008809CU) /**< \brief (MATRIX) Priority Register B for Slave 3 */
  #define REG_MATRIX_PRAS4   (*(__IO uint32_t*)0x400880A0U) /**< \brief (MATRIX) Priority Register A for Slave 4 */
  #define REG_MATRIX_PRBS4   (*(__IO uint32_t*)0x400880A4U) /**< \brief (MATRIX) Priority Register B for Slave 4 */
  #define REG_MATRIX_PRAS5   (*(__IO uint32_t*)0x400880A8U) /**< \brief (MATRIX) Priority Register A for Slave 5 */
  #define REG_MATRIX_PRBS5   (*(__IO uint32_t*)0x400880ACU) /**< \brief (MATRIX) Priority Register B for Slave 5 */
  #define REG_MATRIX_PRAS6   (*(__IO uint32_t*)0x400880B0U) /**< \brief (MATRIX) Priority Register A for Slave 6 */
  #define REG_MATRIX_PRBS6   (*(__IO uint32_t*)0x400880B4U) /**< \brief (MATRIX) Priority Register B for Slave 6 */
  #define REG_MATRIX_PRAS7   (*(__IO uint32_t*)0x400880B8U) /**< \brief (MATRIX) Priority Register A for Slave 7 */
  #define REG_MATRIX_PRBS7   (*(__IO uint32_t*)0x400880BCU) /**< \brief (MATRIX) Priority Register B for Slave 7 */
  #define REG_MATRIX_PRAS8   (*(__IO uint32_t*)0x400880C0U) /**< \brief (MATRIX) Priority Register A for Slave 8 */
  #define REG_MATRIX_PRBS8   (*(__IO uint32_t*)0x400880C4U) /**< \brief (MATRIX) Priority Register B for Slave 8 */
  #define REG_MATRIX_MRCR    (*(__IO uint32_t*)0x40088100U) /**< \brief (MATRIX) Master Remap Control Register */
  #define REG_CCFG_CAN0      (*(__IO uint32_t*)0x40088110U) /**< \brief (MATRIX) CAN0 Configuration Register */
  #define REG_CCFG_SYSIO     (*(__IO uint32_t*)0x40088114U) /**< \brief (MATRIX) System I/O and CAN1 Configuration Register */
  #define REG_CCFG_PCCR      (*(__IO uint32_t*)0x40088118U) /**< \brief (MATRIX) Peripheral Clock Configuration Register */
  #define REG_CCFG_DYNCKG    (*(__IO uint32_t*)0x4008811CU) /**< \brief (MATRIX) Dynamic Clock Gating Register */
  #define REG_CCFG_SMCNFCS   (*(__IO uint32_t*)0x40088124U) /**< \brief (MATRIX) SMC NAND Flash Chip Select Configuration Register */
  #define REG_MATRIX_WPMR    (*(__IO uint32_t*)0x400881E4U) /**< \brief (MATRIX) Write Protection Mode Register */
  #define REG_MATRIX_WPSR    (*(__I  uint32_t*)0x400881E8U) /**< \brief (MATRIX) Write Protection Status Register */
  #define REG_MATRIX_VERSION (*(__I  uint32_t*)0x400881FCU) /**< \brief (MATRIX) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_MATRIX_INSTANCE_ */
