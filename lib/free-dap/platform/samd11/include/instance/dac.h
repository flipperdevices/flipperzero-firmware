/**
 * \file
 *
 * \brief Instance description for DAC
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

#ifndef _SAMD11_DAC_INSTANCE_
#define _SAMD11_DAC_INSTANCE_

/* ========== Register definition for DAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DAC_CTRLA              (0x42002800) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (0x42002801) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (0x42002802) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (0x42002804) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (0x42002805) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (0x42002806) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (0x42002807) /**< \brief (DAC) Status */
#define REG_DAC_DATA               (0x42002808) /**< \brief (DAC) Data */
#define REG_DAC_DATABUF            (0x4200280C) /**< \brief (DAC) Data Buffer */
#else
#define REG_DAC_CTRLA              (*(RwReg8 *)0x42002800UL) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (*(RwReg8 *)0x42002801UL) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (*(RwReg8 *)0x42002802UL) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (*(RwReg8 *)0x42002804UL) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (*(RwReg8 *)0x42002805UL) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (*(RwReg8 *)0x42002806UL) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (*(RoReg8 *)0x42002807UL) /**< \brief (DAC) Status */
#define REG_DAC_DATA               (*(RwReg16*)0x42002808UL) /**< \brief (DAC) Data */
#define REG_DAC_DATABUF            (*(RwReg16*)0x4200280CUL) /**< \brief (DAC) Data Buffer */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DAC peripheral ========== */
#define DAC_DMAC_ID_EMPTY           19       // Index of DMAC EMPTY trigger
#define DAC_GCLK_ID                 22       // Index of Generic Clock

#endif /* _SAMD11_DAC_INSTANCE_ */
