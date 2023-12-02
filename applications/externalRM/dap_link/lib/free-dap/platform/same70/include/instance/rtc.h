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

#ifndef _SAME70_RTC_INSTANCE_
#define _SAME70_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_RTC_CR                       (0x400E1860U) /**< \brief (RTC) Control Register */
  #define REG_RTC_MR                       (0x400E1864U) /**< \brief (RTC) Mode Register */
  #define REG_RTC_TIMR                     (0x400E1868U) /**< \brief (RTC) Time Register */
  #define REG_RTC_CALR                     (0x400E186CU) /**< \brief (RTC) Calendar Register */
  #define REG_RTC_TIMALR                   (0x400E1870U) /**< \brief (RTC) Time Alarm Register */
  #define REG_RTC_CALALR                   (0x400E1874U) /**< \brief (RTC) Calendar Alarm Register */
  #define REG_RTC_SR                       (0x400E1878U) /**< \brief (RTC) Status Register */
  #define REG_RTC_SCCR                     (0x400E187CU) /**< \brief (RTC) Status Clear Command Register */
  #define REG_RTC_IER                      (0x400E1880U) /**< \brief (RTC) Interrupt Enable Register */
  #define REG_RTC_IDR                      (0x400E1884U) /**< \brief (RTC) Interrupt Disable Register */
  #define REG_RTC_IMR                      (0x400E1888U) /**< \brief (RTC) Interrupt Mask Register */
  #define REG_RTC_VER                      (0x400E188CU) /**< \brief (RTC) Valid Entry Register */
  #define REG_RTC_WPMR                     (0x400E1944U) /**< \brief (RTC) Write Protection Mode Register */
  #define REG_RTC_VERSION                  (0x400E195CU) /**< \brief (RTC) Version Register */
#else
  #define REG_RTC_CR      (*(__IO uint32_t*)0x400E1860U) /**< \brief (RTC) Control Register */
  #define REG_RTC_MR      (*(__IO uint32_t*)0x400E1864U) /**< \brief (RTC) Mode Register */
  #define REG_RTC_TIMR    (*(__IO uint32_t*)0x400E1868U) /**< \brief (RTC) Time Register */
  #define REG_RTC_CALR    (*(__IO uint32_t*)0x400E186CU) /**< \brief (RTC) Calendar Register */
  #define REG_RTC_TIMALR  (*(__IO uint32_t*)0x400E1870U) /**< \brief (RTC) Time Alarm Register */
  #define REG_RTC_CALALR  (*(__IO uint32_t*)0x400E1874U) /**< \brief (RTC) Calendar Alarm Register */
  #define REG_RTC_SR      (*(__I  uint32_t*)0x400E1878U) /**< \brief (RTC) Status Register */
  #define REG_RTC_SCCR    (*(__O  uint32_t*)0x400E187CU) /**< \brief (RTC) Status Clear Command Register */
  #define REG_RTC_IER     (*(__O  uint32_t*)0x400E1880U) /**< \brief (RTC) Interrupt Enable Register */
  #define REG_RTC_IDR     (*(__O  uint32_t*)0x400E1884U) /**< \brief (RTC) Interrupt Disable Register */
  #define REG_RTC_IMR     (*(__I  uint32_t*)0x400E1888U) /**< \brief (RTC) Interrupt Mask Register */
  #define REG_RTC_VER     (*(__I  uint32_t*)0x400E188CU) /**< \brief (RTC) Valid Entry Register */
  #define REG_RTC_WPMR    (*(__IO uint32_t*)0x400E1944U) /**< \brief (RTC) Write Protection Mode Register */
  #define REG_RTC_VERSION (*(__I  uint32_t*)0x400E195CU) /**< \brief (RTC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_RTC_INSTANCE_ */
