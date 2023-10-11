/**
 * \file
 *
 * \brief Instance description for TCC0
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

#ifndef _SAMD11_TCC0_INSTANCE_
#define _SAMD11_TCC0_INSTANCE_

/* ========== Register definition for TCC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC0_CTRLA             (0x42001400) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (0x42001404) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (0x42001405) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (0x42001408) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (0x4200140C) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (0x42001410) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (0x42001414) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (0x42001418) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (0x4200141E) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (0x42001420) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (0x42001424) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (0x42001428) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (0x4200142C) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (0x42001430) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (0x42001434) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (0x42001438) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (0x4200143C) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (0x42001440) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (0x42001444) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (0x42001448) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (0x4200144C) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (0x42001450) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_PATTB             (0x42001464) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_WAVEB             (0x42001468) /**< \brief (TCC0) Waveform Control Buffer */
#define REG_TCC0_PERB              (0x4200146C) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCB0              (0x42001470) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCB1              (0x42001474) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCB2              (0x42001478) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCB3              (0x4200147C) /**< \brief (TCC0) Compare and Capture Buffer 3 */
#else
#define REG_TCC0_CTRLA             (*(RwReg  *)0x42001400UL) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (*(RwReg8 *)0x42001404UL) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (*(RwReg8 *)0x42001405UL) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (*(RoReg  *)0x42001408UL) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (*(RwReg  *)0x4200140CUL) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (*(RwReg  *)0x42001410UL) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (*(RwReg  *)0x42001414UL) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (*(RwReg  *)0x42001418UL) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (*(RwReg8 *)0x4200141EUL) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (*(RwReg  *)0x42001420UL) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (*(RwReg  *)0x42001424UL) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (*(RwReg  *)0x42001428UL) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (*(RwReg  *)0x4200142CUL) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (*(RwReg  *)0x42001430UL) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (*(RwReg  *)0x42001434UL) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (*(RwReg16*)0x42001438UL) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (*(RwReg  *)0x4200143CUL) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (*(RwReg  *)0x42001440UL) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (*(RwReg  *)0x42001444UL) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (*(RwReg  *)0x42001448UL) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (*(RwReg  *)0x4200144CUL) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (*(RwReg  *)0x42001450UL) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_PATTB             (*(RwReg16*)0x42001464UL) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_WAVEB             (*(RwReg  *)0x42001468UL) /**< \brief (TCC0) Waveform Control Buffer */
#define REG_TCC0_PERB              (*(RwReg  *)0x4200146CUL) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCB0              (*(RwReg  *)0x42001470UL) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCB1              (*(RwReg  *)0x42001474UL) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCB2              (*(RwReg  *)0x42001478UL) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCB3              (*(RwReg  *)0x4200147CUL) /**< \brief (TCC0) Compare and Capture Buffer 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC0 peripheral ========== */
#define TCC0_CC_NUM                 4        // Number of Compare/Capture units
#define TCC0_DITHERING              1        // Dithering feature implemented
#define TCC0_DMAC_ID_MC_0           8
#define TCC0_DMAC_ID_MC_1           9
#define TCC0_DMAC_ID_MC_2           10
#define TCC0_DMAC_ID_MC_3           11
#define TCC0_DMAC_ID_MC_LSB         8
#define TCC0_DMAC_ID_MC_MSB         11
#define TCC0_DMAC_ID_MC_SIZE        4
#define TCC0_DMAC_ID_OVF            7        // DMA overflow/underflow/retrigger trigger
#define TCC0_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC0_EXT                    31       // (@_DITHERING*16+@_PG*8+@_SWAP*4+@_DTI*2+@_OTMX*1)
#define TCC0_GCLK_ID                17       // Index of Generic Clock
#define TCC0_MASTER                 0       
#define TCC0_OTMX                   1        // Output Matrix feature implemented
#define TCC0_OW_NUM                 8        // Number of Output Waveforms
#define TCC0_PG                     1        // Pattern Generation feature implemented
#define TCC0_SIZE                   24      
#define TCC0_SWAP                   1        // DTI outputs swap feature implemented

#endif /* _SAMD11_TCC0_INSTANCE_ */
