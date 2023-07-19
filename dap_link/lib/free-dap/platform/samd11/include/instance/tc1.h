/**
 * \file
 *
 * \brief Instance description for TC1
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD11_TC1_INSTANCE_
#define _SAMD11_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC1_CTRLA              (0x42001800) /**< \brief (TC1) Control A */
#define REG_TC1_READREQ            (0x42001802) /**< \brief (TC1) Read Request */
#define REG_TC1_CTRLBCLR           (0x42001804) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (0x42001805) /**< \brief (TC1) Control B Set */
#define REG_TC1_CTRLC              (0x42001806) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (0x42001808) /**< \brief (TC1) Debug Control */
#define REG_TC1_EVCTRL             (0x4200180A) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (0x4200180C) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (0x4200180D) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (0x4200180E) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (0x4200180F) /**< \brief (TC1) Status */
#define REG_TC1_COUNT16_COUNT      (0x42001810) /**< \brief (TC1) COUNT16 Counter Value */
#define REG_TC1_COUNT16_CC0        (0x42001818) /**< \brief (TC1) COUNT16 Compare/Capture 0 */
#define REG_TC1_COUNT16_CC1        (0x4200181A) /**< \brief (TC1) COUNT16 Compare/Capture 1 */
#define REG_TC1_COUNT32_COUNT      (0x42001810) /**< \brief (TC1) COUNT32 Counter Value */
#define REG_TC1_COUNT32_CC0        (0x42001818) /**< \brief (TC1) COUNT32 Compare/Capture 0 */
#define REG_TC1_COUNT32_CC1        (0x4200181C) /**< \brief (TC1) COUNT32 Compare/Capture 1 */
#define REG_TC1_COUNT8_COUNT       (0x42001810) /**< \brief (TC1) COUNT8 Counter Value */
#define REG_TC1_COUNT8_PER         (0x42001814) /**< \brief (TC1) COUNT8 Period Value */
#define REG_TC1_COUNT8_CC0         (0x42001818) /**< \brief (TC1) COUNT8 Compare/Capture 0 */
#define REG_TC1_COUNT8_CC1         (0x42001819) /**< \brief (TC1) COUNT8 Compare/Capture 1 */
#else
#define REG_TC1_CTRLA              (*(RwReg16*)0x42001800UL) /**< \brief (TC1) Control A */
#define REG_TC1_READREQ            (*(RwReg16*)0x42001802UL) /**< \brief (TC1) Read Request */
#define REG_TC1_CTRLBCLR           (*(RwReg8 *)0x42001804UL) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (*(RwReg8 *)0x42001805UL) /**< \brief (TC1) Control B Set */
#define REG_TC1_CTRLC              (*(RwReg8 *)0x42001806UL) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (*(RwReg8 *)0x42001808UL) /**< \brief (TC1) Debug Control */
#define REG_TC1_EVCTRL             (*(RwReg16*)0x4200180AUL) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (*(RwReg8 *)0x4200180CUL) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (*(RwReg8 *)0x4200180DUL) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (*(RwReg8 *)0x4200180EUL) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (*(RoReg8 *)0x4200180FUL) /**< \brief (TC1) Status */
#define REG_TC1_COUNT16_COUNT      (*(RwReg16*)0x42001810UL) /**< \brief (TC1) COUNT16 Counter Value */
#define REG_TC1_COUNT16_CC0        (*(RwReg16*)0x42001818UL) /**< \brief (TC1) COUNT16 Compare/Capture 0 */
#define REG_TC1_COUNT16_CC1        (*(RwReg16*)0x4200181AUL) /**< \brief (TC1) COUNT16 Compare/Capture 1 */
#define REG_TC1_COUNT32_COUNT      (*(RwReg  *)0x42001810UL) /**< \brief (TC1) COUNT32 Counter Value */
#define REG_TC1_COUNT32_CC0        (*(RwReg  *)0x42001818UL) /**< \brief (TC1) COUNT32 Compare/Capture 0 */
#define REG_TC1_COUNT32_CC1        (*(RwReg  *)0x4200181CUL) /**< \brief (TC1) COUNT32 Compare/Capture 1 */
#define REG_TC1_COUNT8_COUNT       (*(RwReg8 *)0x42001810UL) /**< \brief (TC1) COUNT8 Counter Value */
#define REG_TC1_COUNT8_PER         (*(RwReg8 *)0x42001814UL) /**< \brief (TC1) COUNT8 Period Value */
#define REG_TC1_COUNT8_CC0         (*(RwReg8 *)0x42001818UL) /**< \brief (TC1) COUNT8 Compare/Capture 0 */
#define REG_TC1_COUNT8_CC1         (*(RwReg8 *)0x42001819UL) /**< \brief (TC1) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC1 peripheral ========== */
#define TC1_CC8_NUM                 2        // Number of 8-bit Counters
#define TC1_CC16_NUM                2        // Number of 16-bit Counters
#define TC1_CC32_NUM                2        // Number of 32-bit Counters
#define TC1_DITHERING_EXT           0        // Dithering feature implemented
#define TC1_DMAC_ID_MC_0            13
#define TC1_DMAC_ID_MC_1            14
#define TC1_DMAC_ID_MC_LSB          13
#define TC1_DMAC_ID_MC_MSB          14
#define TC1_DMAC_ID_MC_SIZE         2
#define TC1_DMAC_ID_OVF             12       // Indexes of DMA Overflow trigger
#define TC1_GCLK_ID                 18       // Index of Generic Clock
#define TC1_MASTER                  1       
#define TC1_OW_NUM                  2        // Number of Output Waveforms
#define TC1_PERIOD_EXT              0        // Period feature implemented
#define TC1_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD11_TC1_INSTANCE_ */
