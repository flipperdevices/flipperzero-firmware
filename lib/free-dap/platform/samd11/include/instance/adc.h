/**
 * \file
 *
 * \brief Instance description for ADC
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

#ifndef _SAMD11_ADC_INSTANCE_
#define _SAMD11_ADC_INSTANCE_

/* ========== Register definition for ADC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ADC_CTRLA              (0x42002000) /**< \brief (ADC) Control A */
#define REG_ADC_REFCTRL            (0x42002001) /**< \brief (ADC) Reference Control */
#define REG_ADC_AVGCTRL            (0x42002002) /**< \brief (ADC) Average Control */
#define REG_ADC_SAMPCTRL           (0x42002003) /**< \brief (ADC) Sampling Time Control */
#define REG_ADC_CTRLB              (0x42002004) /**< \brief (ADC) Control B */
#define REG_ADC_WINCTRL            (0x42002008) /**< \brief (ADC) Window Monitor Control */
#define REG_ADC_SWTRIG             (0x4200200C) /**< \brief (ADC) Software Trigger */
#define REG_ADC_INPUTCTRL          (0x42002010) /**< \brief (ADC) Input Control */
#define REG_ADC_EVCTRL             (0x42002014) /**< \brief (ADC) Event Control */
#define REG_ADC_INTENCLR           (0x42002016) /**< \brief (ADC) Interrupt Enable Clear */
#define REG_ADC_INTENSET           (0x42002017) /**< \brief (ADC) Interrupt Enable Set */
#define REG_ADC_INTFLAG            (0x42002018) /**< \brief (ADC) Interrupt Flag Status and Clear */
#define REG_ADC_STATUS             (0x42002019) /**< \brief (ADC) Status */
#define REG_ADC_RESULT             (0x4200201A) /**< \brief (ADC) Result */
#define REG_ADC_WINLT              (0x4200201C) /**< \brief (ADC) Window Monitor Lower Threshold */
#define REG_ADC_WINUT              (0x42002020) /**< \brief (ADC) Window Monitor Upper Threshold */
#define REG_ADC_GAINCORR           (0x42002024) /**< \brief (ADC) Gain Correction */
#define REG_ADC_OFFSETCORR         (0x42002026) /**< \brief (ADC) Offset Correction */
#define REG_ADC_CALIB              (0x42002028) /**< \brief (ADC) Calibration */
#define REG_ADC_DBGCTRL            (0x4200202A) /**< \brief (ADC) Debug Control */
#else
#define REG_ADC_CTRLA              (*(RwReg8 *)0x42002000UL) /**< \brief (ADC) Control A */
#define REG_ADC_REFCTRL            (*(RwReg8 *)0x42002001UL) /**< \brief (ADC) Reference Control */
#define REG_ADC_AVGCTRL            (*(RwReg8 *)0x42002002UL) /**< \brief (ADC) Average Control */
#define REG_ADC_SAMPCTRL           (*(RwReg8 *)0x42002003UL) /**< \brief (ADC) Sampling Time Control */
#define REG_ADC_CTRLB              (*(RwReg16*)0x42002004UL) /**< \brief (ADC) Control B */
#define REG_ADC_WINCTRL            (*(RwReg8 *)0x42002008UL) /**< \brief (ADC) Window Monitor Control */
#define REG_ADC_SWTRIG             (*(RwReg8 *)0x4200200CUL) /**< \brief (ADC) Software Trigger */
#define REG_ADC_INPUTCTRL          (*(RwReg  *)0x42002010UL) /**< \brief (ADC) Input Control */
#define REG_ADC_EVCTRL             (*(RwReg8 *)0x42002014UL) /**< \brief (ADC) Event Control */
#define REG_ADC_INTENCLR           (*(RwReg8 *)0x42002016UL) /**< \brief (ADC) Interrupt Enable Clear */
#define REG_ADC_INTENSET           (*(RwReg8 *)0x42002017UL) /**< \brief (ADC) Interrupt Enable Set */
#define REG_ADC_INTFLAG            (*(RwReg8 *)0x42002018UL) /**< \brief (ADC) Interrupt Flag Status and Clear */
#define REG_ADC_STATUS             (*(RoReg8 *)0x42002019UL) /**< \brief (ADC) Status */
#define REG_ADC_RESULT             (*(RoReg16*)0x4200201AUL) /**< \brief (ADC) Result */
#define REG_ADC_WINLT              (*(RwReg16*)0x4200201CUL) /**< \brief (ADC) Window Monitor Lower Threshold */
#define REG_ADC_WINUT              (*(RwReg16*)0x42002020UL) /**< \brief (ADC) Window Monitor Upper Threshold */
#define REG_ADC_GAINCORR           (*(RwReg16*)0x42002024UL) /**< \brief (ADC) Gain Correction */
#define REG_ADC_OFFSETCORR         (*(RwReg16*)0x42002026UL) /**< \brief (ADC) Offset Correction */
#define REG_ADC_CALIB              (*(RwReg16*)0x42002028UL) /**< \brief (ADC) Calibration */
#define REG_ADC_DBGCTRL            (*(RwReg8 *)0x4200202AUL) /**< \brief (ADC) Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for ADC peripheral ========== */
#define ADC_DMAC_ID_RESRDY          18       // Index of DMA RESRDY trigger
#define ADC_EXTCHANNEL_MSB          9        // Number of external channels
#define ADC_GCLK_ID                 19       // Index of Generic Clock
#define ADC_RESULT_BITS             16       // Size of RESULT.RESULT bitfield
#define ADC_RESULT_MSB              15       // Size of Result

#endif /* _SAMD11_ADC_INSTANCE_ */
