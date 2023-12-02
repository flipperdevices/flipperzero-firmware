/**
 * \file
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _SAME70_I2SC1_INSTANCE_
#define _SAME70_I2SC1_INSTANCE_

/* ========== Register definition for I2SC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_I2SC1_CR                       (0x40090000U) /**< \brief (I2SC1) Control Register */
  #define REG_I2SC1_MR                       (0x40090004U) /**< \brief (I2SC1) Mode Register */
  #define REG_I2SC1_SR                       (0x40090008U) /**< \brief (I2SC1) Status Register */
  #define REG_I2SC1_SCR                      (0x4009000CU) /**< \brief (I2SC1) Status Clear Register */
  #define REG_I2SC1_SSR                      (0x40090010U) /**< \brief (I2SC1) Status Set Register */
  #define REG_I2SC1_IER                      (0x40090014U) /**< \brief (I2SC1) Interrupt Enable Register */
  #define REG_I2SC1_IDR                      (0x40090018U) /**< \brief (I2SC1) Interrupt Disable Register */
  #define REG_I2SC1_IMR                      (0x4009001CU) /**< \brief (I2SC1) Interrupt Mask Register */
  #define REG_I2SC1_RHR                      (0x40090020U) /**< \brief (I2SC1) Receiver Holding Register */
  #define REG_I2SC1_THR                      (0x40090024U) /**< \brief (I2SC1) Transmitter Holding Register */
  #define REG_I2SC1_VERSION                  (0x40090028U) /**< \brief (I2SC1) Version Register */
#else
  #define REG_I2SC1_CR      (*(__O  uint32_t*)0x40090000U) /**< \brief (I2SC1) Control Register */
  #define REG_I2SC1_MR      (*(__IO uint32_t*)0x40090004U) /**< \brief (I2SC1) Mode Register */
  #define REG_I2SC1_SR      (*(__I  uint32_t*)0x40090008U) /**< \brief (I2SC1) Status Register */
  #define REG_I2SC1_SCR     (*(__O  uint32_t*)0x4009000CU) /**< \brief (I2SC1) Status Clear Register */
  #define REG_I2SC1_SSR     (*(__O  uint32_t*)0x40090010U) /**< \brief (I2SC1) Status Set Register */
  #define REG_I2SC1_IER     (*(__O  uint32_t*)0x40090014U) /**< \brief (I2SC1) Interrupt Enable Register */
  #define REG_I2SC1_IDR     (*(__O  uint32_t*)0x40090018U) /**< \brief (I2SC1) Interrupt Disable Register */
  #define REG_I2SC1_IMR     (*(__I  uint32_t*)0x4009001CU) /**< \brief (I2SC1) Interrupt Mask Register */
  #define REG_I2SC1_RHR     (*(__I  uint32_t*)0x40090020U) /**< \brief (I2SC1) Receiver Holding Register */
  #define REG_I2SC1_THR     (*(__O  uint32_t*)0x40090024U) /**< \brief (I2SC1) Transmitter Holding Register */
  #define REG_I2SC1_VERSION (*(__I  uint32_t*)0x40090028U) /**< \brief (I2SC1) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_I2SC1_INSTANCE_ */
