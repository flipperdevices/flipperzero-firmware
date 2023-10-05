/**
 * \file
 *
 * \brief Instance description for TC2
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

#ifndef _SAMD11_TC2_INSTANCE_
#define _SAMD11_TC2_INSTANCE_

/* ========== Register definition for TC2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC2_CTRLA              (0x42001C00) /**< \brief (TC2) Control A */
#define REG_TC2_READREQ            (0x42001C02) /**< \brief (TC2) Read Request */
#define REG_TC2_CTRLBCLR           (0x42001C04) /**< \brief (TC2) Control B Clear */
#define REG_TC2_CTRLBSET           (0x42001C05) /**< \brief (TC2) Control B Set */
#define REG_TC2_CTRLC              (0x42001C06) /**< \brief (TC2) Control C */
#define REG_TC2_DBGCTRL            (0x42001C08) /**< \brief (TC2) Debug Control */
#define REG_TC2_EVCTRL             (0x42001C0A) /**< \brief (TC2) Event Control */
#define REG_TC2_INTENCLR           (0x42001C0C) /**< \brief (TC2) Interrupt Enable Clear */
#define REG_TC2_INTENSET           (0x42001C0D) /**< \brief (TC2) Interrupt Enable Set */
#define REG_TC2_INTFLAG            (0x42001C0E) /**< \brief (TC2) Interrupt Flag Status and Clear */
#define REG_TC2_STATUS             (0x42001C0F) /**< \brief (TC2) Status */
#define REG_TC2_COUNT16_COUNT      (0x42001C10) /**< \brief (TC2) COUNT16 Counter Value */
#define REG_TC2_COUNT16_CC0        (0x42001C18) /**< \brief (TC2) COUNT16 Compare/Capture 0 */
#define REG_TC2_COUNT16_CC1        (0x42001C1A) /**< \brief (TC2) COUNT16 Compare/Capture 1 */
#define REG_TC2_COUNT32_COUNT      (0x42001C10) /**< \brief (TC2) COUNT32 Counter Value */
#define REG_TC2_COUNT32_CC0        (0x42001C18) /**< \brief (TC2) COUNT32 Compare/Capture 0 */
#define REG_TC2_COUNT32_CC1        (0x42001C1C) /**< \brief (TC2) COUNT32 Compare/Capture 1 */
#define REG_TC2_COUNT8_COUNT       (0x42001C10) /**< \brief (TC2) COUNT8 Counter Value */
#define REG_TC2_COUNT8_PER         (0x42001C14) /**< \brief (TC2) COUNT8 Period Value */
#define REG_TC2_COUNT8_CC0         (0x42001C18) /**< \brief (TC2) COUNT8 Compare/Capture 0 */
#define REG_TC2_COUNT8_CC1         (0x42001C19) /**< \brief (TC2) COUNT8 Compare/Capture 1 */
#else
#define REG_TC2_CTRLA              (*(RwReg16*)0x42001C00UL) /**< \brief (TC2) Control A */
#define REG_TC2_READREQ            (*(RwReg16*)0x42001C02UL) /**< \brief (TC2) Read Request */
#define REG_TC2_CTRLBCLR           (*(RwReg8 *)0x42001C04UL) /**< \brief (TC2) Control B Clear */
#define REG_TC2_CTRLBSET           (*(RwReg8 *)0x42001C05UL) /**< \brief (TC2) Control B Set */
#define REG_TC2_CTRLC              (*(RwReg8 *)0x42001C06UL) /**< \brief (TC2) Control C */
#define REG_TC2_DBGCTRL            (*(RwReg8 *)0x42001C08UL) /**< \brief (TC2) Debug Control */
#define REG_TC2_EVCTRL             (*(RwReg16*)0x42001C0AUL) /**< \brief (TC2) Event Control */
#define REG_TC2_INTENCLR           (*(RwReg8 *)0x42001C0CUL) /**< \brief (TC2) Interrupt Enable Clear */
#define REG_TC2_INTENSET           (*(RwReg8 *)0x42001C0DUL) /**< \brief (TC2) Interrupt Enable Set */
#define REG_TC2_INTFLAG            (*(RwReg8 *)0x42001C0EUL) /**< \brief (TC2) Interrupt Flag Status and Clear */
#define REG_TC2_STATUS             (*(RoReg8 *)0x42001C0FUL) /**< \brief (TC2) Status */
#define REG_TC2_COUNT16_COUNT      (*(RwReg16*)0x42001C10UL) /**< \brief (TC2) COUNT16 Counter Value */
#define REG_TC2_COUNT16_CC0        (*(RwReg16*)0x42001C18UL) /**< \brief (TC2) COUNT16 Compare/Capture 0 */
#define REG_TC2_COUNT16_CC1        (*(RwReg16*)0x42001C1AUL) /**< \brief (TC2) COUNT16 Compare/Capture 1 */
#define REG_TC2_COUNT32_COUNT      (*(RwReg  *)0x42001C10UL) /**< \brief (TC2) COUNT32 Counter Value */
#define REG_TC2_COUNT32_CC0        (*(RwReg  *)0x42001C18UL) /**< \brief (TC2) COUNT32 Compare/Capture 0 */
#define REG_TC2_COUNT32_CC1        (*(RwReg  *)0x42001C1CUL) /**< \brief (TC2) COUNT32 Compare/Capture 1 */
#define REG_TC2_COUNT8_COUNT       (*(RwReg8 *)0x42001C10UL) /**< \brief (TC2) COUNT8 Counter Value */
#define REG_TC2_COUNT8_PER         (*(RwReg8 *)0x42001C14UL) /**< \brief (TC2) COUNT8 Period Value */
#define REG_TC2_COUNT8_CC0         (*(RwReg8 *)0x42001C18UL) /**< \brief (TC2) COUNT8 Compare/Capture 0 */
#define REG_TC2_COUNT8_CC1         (*(RwReg8 *)0x42001C19UL) /**< \brief (TC2) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC2 peripheral ========== */
#define TC2_CC8_NUM                 2        // Number of 8-bit Counters
#define TC2_CC16_NUM                2        // Number of 16-bit Counters
#define TC2_CC32_NUM                2        // Number of 32-bit Counters
#define TC2_DITHERING_EXT           0        // Dithering feature implemented
#define TC2_DMAC_ID_MC_0            16
#define TC2_DMAC_ID_MC_1            17
#define TC2_DMAC_ID_MC_LSB          16
#define TC2_DMAC_ID_MC_MSB          17
#define TC2_DMAC_ID_MC_SIZE         2
#define TC2_DMAC_ID_OVF             15       // Indexes of DMA Overflow trigger
#define TC2_GCLK_ID                 18       // Index of Generic Clock
#define TC2_MASTER                  0       
#define TC2_OW_NUM                  2        // Number of Output Waveforms
#define TC2_PERIOD_EXT              0        // Period feature implemented
#define TC2_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD11_TC2_INSTANCE_ */
