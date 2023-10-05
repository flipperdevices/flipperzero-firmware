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

#ifndef _SAME70_RTT_INSTANCE_
#define _SAME70_RTT_INSTANCE_

/* ========== Register definition for RTT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_RTT_MR                  (0x400E1830U) /**< \brief (RTT) Mode Register */
  #define REG_RTT_AR                  (0x400E1834U) /**< \brief (RTT) Alarm Register */
  #define REG_RTT_VR                  (0x400E1838U) /**< \brief (RTT) Value Register */
  #define REG_RTT_SR                  (0x400E183CU) /**< \brief (RTT) Status Register */
#else
  #define REG_RTT_MR (*(__IO uint32_t*)0x400E1830U) /**< \brief (RTT) Mode Register */
  #define REG_RTT_AR (*(__IO uint32_t*)0x400E1834U) /**< \brief (RTT) Alarm Register */
  #define REG_RTT_VR (*(__I  uint32_t*)0x400E1838U) /**< \brief (RTT) Value Register */
  #define REG_RTT_SR (*(__I  uint32_t*)0x400E183CU) /**< \brief (RTT) Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAME70_RTT_INSTANCE_ */
