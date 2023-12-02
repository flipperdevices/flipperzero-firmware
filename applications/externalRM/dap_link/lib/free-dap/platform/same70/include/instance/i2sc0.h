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

#ifndef _SAME70_I2SC0_INSTANCE_
#define _SAME70_I2SC0_INSTANCE_

/* ========== Register definition for I2SC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_I2SC0_CR                       (0x4008C000U) /**< \brief (I2SC0) Control Register */
  #define REG_I2SC0_MR                       (0x4008C004U) /**< \brief (I2SC0) Mode Register */
  #define REG_I2SC0_SR                       (0x4008C008U) /**< \brief (I2SC0) Status Register */
  #define REG_I2SC0_SCR                      (0x4008C00CU) /**< \brief (I2SC0) Status Clear Register */
  #define REG_I2SC0_SSR                      (0x4008C010U) /**< \brief (I2SC0) Status Set Register */
  #define REG_I2SC0_IER                      (0x4008C014U) /**< \brief (I2SC0) Interrupt Enable Register */
  #define REG_I2SC0_IDR                      (0x4008C018U) /**< \brief (I2SC0) Interrupt Disable Register */
  #define REG_I2SC0_IMR                      (0x4008C01CU) /**< \brief (I2SC0) Interrupt Mask Register */
  #define REG_I2SC0_RHR                      (0x4008C020U) /**< \brief (I2SC0) Receiver Holding Register */
  #define REG_I2SC0_THR                      (0x4008C024U) /**< \brief (I2SC0) Transmitter Holding Register */
  #define REG_I2SC0_VERSION                  (0x4008C028U) /**< \brief (I2SC0) Version Register */
#else
  #define REG_I2SC0_CR      (*(__O  uint32_t*)0x4008C000U) /**< \brief (I2SC0) Control Register */
  #define REG_I2SC0_MR      (*(__IO uint32_t*)0x4008C004U) /**< \brief (I2SC0) Mode Register */
  #define REG_I2SC0_SR      (*(__I  uint32_t*)0x4008C008U) /**< \brief (I2SC0) Status Register */
  #define REG_I2SC0_SCR     (*(__O  uint32_t*)0x4008C00CU) /**< \brief (I2SC0) Status Clear Register */
  #define REG_I2SC0_SSR     (*(__O  uint32_t*)0x4008C010U) /**< \brief (I2SC0) Status Set Register */
  #define REG_I2SC0_IER     (*(__O  uint32_t*)0x4008C014U) /**< \brief (I2SC0) Interrupt Enable Register */
  #define REG_I2SC0_IDR     (*(__O  uint32_t*)0x4008C018U) /**< \brief (I2SC0) Interrupt Disable Register */
  #define REG_I2SC0_IMR     (*(__I  uint32_t*)0x4008C01CU) /**< \brief (I2SC0) Interrupt Mask Register */
  #define REG_I2SC0_RHR     (*(__I  uint32_t*)0x4008C020U) /**< \brief (I2SC0) Receiver Holding Register */
  #define REG_I2SC0_THR     (*(__O  uint32_t*)0x4008C024U) /**< \brief (I2SC0) Transmitter Holding Register */
  #define REG_I2SC0_VERSION (*(__I  uint32_t*)0x4008C028U) /**< \brief (I2SC0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_I2SC0_INSTANCE_ */
