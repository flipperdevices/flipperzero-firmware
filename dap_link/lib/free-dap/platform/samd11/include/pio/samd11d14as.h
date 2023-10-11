/**
 * \file
 *
 * \brief Peripheral I/O description for SAMD11D14AS
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

#ifndef _SAMD11D14AS_PIO_
#define _SAMD11D14AS_PIO_

#define PIN_PA02                            2  /**< \brief Pin Number for PA02 */
#define PORT_PA02              (_UL_(1) <<  2) /**< \brief PORT Mask  for PA02 */
#define PIN_PA03                            3  /**< \brief Pin Number for PA03 */
#define PORT_PA03              (_UL_(1) <<  3) /**< \brief PORT Mask  for PA03 */
#define PIN_PA04                            4  /**< \brief Pin Number for PA04 */
#define PORT_PA04              (_UL_(1) <<  4) /**< \brief PORT Mask  for PA04 */
#define PIN_PA05                            5  /**< \brief Pin Number for PA05 */
#define PORT_PA05              (_UL_(1) <<  5) /**< \brief PORT Mask  for PA05 */
#define PIN_PA06                            6  /**< \brief Pin Number for PA06 */
#define PORT_PA06              (_UL_(1) <<  6) /**< \brief PORT Mask  for PA06 */
#define PIN_PA07                            7  /**< \brief Pin Number for PA07 */
#define PORT_PA07              (_UL_(1) <<  7) /**< \brief PORT Mask  for PA07 */
#define PIN_PA08                            8  /**< \brief Pin Number for PA08 */
#define PORT_PA08              (_UL_(1) <<  8) /**< \brief PORT Mask  for PA08 */
#define PIN_PA09                            9  /**< \brief Pin Number for PA09 */
#define PORT_PA09              (_UL_(1) <<  9) /**< \brief PORT Mask  for PA09 */
#define PIN_PA14                           14  /**< \brief Pin Number for PA14 */
#define PORT_PA14              (_UL_(1) << 14) /**< \brief PORT Mask  for PA14 */
#define PIN_PA15                           15  /**< \brief Pin Number for PA15 */
#define PORT_PA15              (_UL_(1) << 15) /**< \brief PORT Mask  for PA15 */
#define PIN_PA16                           16  /**< \brief Pin Number for PA16 */
#define PORT_PA16              (_UL_(1) << 16) /**< \brief PORT Mask  for PA16 */
#define PIN_PA22                           22  /**< \brief Pin Number for PA22 */
#define PORT_PA22              (_UL_(1) << 22) /**< \brief PORT Mask  for PA22 */
#define PIN_PA23                           23  /**< \brief Pin Number for PA23 */
#define PORT_PA23              (_UL_(1) << 23) /**< \brief PORT Mask  for PA23 */
#define PIN_PA24                           24  /**< \brief Pin Number for PA24 */
#define PORT_PA24              (_UL_(1) << 24) /**< \brief PORT Mask  for PA24 */
#define PIN_PA25                           25  /**< \brief Pin Number for PA25 */
#define PORT_PA25              (_UL_(1) << 25) /**< \brief PORT Mask  for PA25 */
#define PIN_PA28                           28  /**< \brief Pin Number for PA28 */
#define PORT_PA28              (_UL_(1) << 28) /**< \brief PORT Mask  for PA28 */
#define PIN_PA30                           30  /**< \brief Pin Number for PA30 */
#define PORT_PA30              (_UL_(1) << 30) /**< \brief PORT Mask  for PA30 */
#define PIN_PA31                           31  /**< \brief Pin Number for PA31 */
#define PORT_PA31              (_UL_(1) << 31) /**< \brief PORT Mask  for PA31 */
/* ========== PORT definition for CORE peripheral ========== */
#define PIN_PA30G_CORE_SWCLK           _L_(30) /**< \brief CORE signal: SWCLK on PA30 mux G */
#define MUX_PA30G_CORE_SWCLK            _L_(6)
#define PINMUX_PA30G_CORE_SWCLK    ((PIN_PA30G_CORE_SWCLK << 16) | MUX_PA30G_CORE_SWCLK)
#define PORT_PA30G_CORE_SWCLK  (_UL_(1) << 30)
/* ========== PORT definition for GCLK peripheral ========== */
#define PIN_PA08H_GCLK_IO0              _L_(8) /**< \brief GCLK signal: IO0 on PA08 mux H */
#define MUX_PA08H_GCLK_IO0              _L_(7)
#define PINMUX_PA08H_GCLK_IO0      ((PIN_PA08H_GCLK_IO0 << 16) | MUX_PA08H_GCLK_IO0)
#define PORT_PA08H_GCLK_IO0    (_UL_(1) <<  8)
#define PIN_PA24H_GCLK_IO0             _L_(24) /**< \brief GCLK signal: IO0 on PA24 mux H */
#define MUX_PA24H_GCLK_IO0              _L_(7)
#define PINMUX_PA24H_GCLK_IO0      ((PIN_PA24H_GCLK_IO0 << 16) | MUX_PA24H_GCLK_IO0)
#define PORT_PA24H_GCLK_IO0    (_UL_(1) << 24)
#define PIN_PA25H_GCLK_IO0             _L_(25) /**< \brief GCLK signal: IO0 on PA25 mux H */
#define MUX_PA25H_GCLK_IO0              _L_(7)
#define PINMUX_PA25H_GCLK_IO0      ((PIN_PA25H_GCLK_IO0 << 16) | MUX_PA25H_GCLK_IO0)
#define PORT_PA25H_GCLK_IO0    (_UL_(1) << 25)
#define PIN_PA30H_GCLK_IO0             _L_(30) /**< \brief GCLK signal: IO0 on PA30 mux H */
#define MUX_PA30H_GCLK_IO0              _L_(7)
#define PINMUX_PA30H_GCLK_IO0      ((PIN_PA30H_GCLK_IO0 << 16) | MUX_PA30H_GCLK_IO0)
#define PORT_PA30H_GCLK_IO0    (_UL_(1) << 30)
#define PIN_PA31H_GCLK_IO0             _L_(31) /**< \brief GCLK signal: IO0 on PA31 mux H */
#define MUX_PA31H_GCLK_IO0              _L_(7)
#define PINMUX_PA31H_GCLK_IO0      ((PIN_PA31H_GCLK_IO0 << 16) | MUX_PA31H_GCLK_IO0)
#define PORT_PA31H_GCLK_IO0    (_UL_(1) << 31)
#define PIN_PA09H_GCLK_IO1              _L_(9) /**< \brief GCLK signal: IO1 on PA09 mux H */
#define MUX_PA09H_GCLK_IO1              _L_(7)
#define PINMUX_PA09H_GCLK_IO1      ((PIN_PA09H_GCLK_IO1 << 16) | MUX_PA09H_GCLK_IO1)
#define PORT_PA09H_GCLK_IO1    (_UL_(1) <<  9)
#define PIN_PA22H_GCLK_IO1             _L_(22) /**< \brief GCLK signal: IO1 on PA22 mux H */
#define MUX_PA22H_GCLK_IO1              _L_(7)
#define PINMUX_PA22H_GCLK_IO1      ((PIN_PA22H_GCLK_IO1 << 16) | MUX_PA22H_GCLK_IO1)
#define PORT_PA22H_GCLK_IO1    (_UL_(1) << 22)
#define PIN_PA16H_GCLK_IO2             _L_(16) /**< \brief GCLK signal: IO2 on PA16 mux H */
#define MUX_PA16H_GCLK_IO2              _L_(7)
#define PINMUX_PA16H_GCLK_IO2      ((PIN_PA16H_GCLK_IO2 << 16) | MUX_PA16H_GCLK_IO2)
#define PORT_PA16H_GCLK_IO2    (_UL_(1) << 16)
#define PIN_PA23H_GCLK_IO2             _L_(23) /**< \brief GCLK signal: IO2 on PA23 mux H */
#define MUX_PA23H_GCLK_IO2              _L_(7)
#define PINMUX_PA23H_GCLK_IO2      ((PIN_PA23H_GCLK_IO2 << 16) | MUX_PA23H_GCLK_IO2)
#define PORT_PA23H_GCLK_IO2    (_UL_(1) << 23)
#define PIN_PA14H_GCLK_IO4             _L_(14) /**< \brief GCLK signal: IO4 on PA14 mux H */
#define MUX_PA14H_GCLK_IO4              _L_(7)
#define PINMUX_PA14H_GCLK_IO4      ((PIN_PA14H_GCLK_IO4 << 16) | MUX_PA14H_GCLK_IO4)
#define PORT_PA14H_GCLK_IO4    (_UL_(1) << 14)
#define PIN_PA15H_GCLK_IO5             _L_(15) /**< \brief GCLK signal: IO5 on PA15 mux H */
#define MUX_PA15H_GCLK_IO5              _L_(7)
#define PINMUX_PA15H_GCLK_IO5      ((PIN_PA15H_GCLK_IO5 << 16) | MUX_PA15H_GCLK_IO5)
#define PORT_PA15H_GCLK_IO5    (_UL_(1) << 15)
/* ========== PORT definition for EIC peripheral ========== */
#define PIN_PA16A_EIC_EXTINT0          _L_(16) /**< \brief EIC signal: EXTINT0 on PA16 mux A */
#define MUX_PA16A_EIC_EXTINT0           _L_(0)
#define PINMUX_PA16A_EIC_EXTINT0   ((PIN_PA16A_EIC_EXTINT0 << 16) | MUX_PA16A_EIC_EXTINT0)
#define PORT_PA16A_EIC_EXTINT0  (_UL_(1) << 16)
#define PIN_PA16A_EIC_EXTINT_NUM        _L_(0) /**< \brief EIC signal: PIN_PA16 External Interrupt Line */
#define PIN_PA15A_EIC_EXTINT1          _L_(15) /**< \brief EIC signal: EXTINT1 on PA15 mux A */
#define MUX_PA15A_EIC_EXTINT1           _L_(0)
#define PINMUX_PA15A_EIC_EXTINT1   ((PIN_PA15A_EIC_EXTINT1 << 16) | MUX_PA15A_EIC_EXTINT1)
#define PORT_PA15A_EIC_EXTINT1  (_UL_(1) << 15)
#define PIN_PA15A_EIC_EXTINT_NUM        _L_(1) /**< \brief EIC signal: PIN_PA15 External Interrupt Line */
#define PIN_PA02A_EIC_EXTINT2           _L_(2) /**< \brief EIC signal: EXTINT2 on PA02 mux A */
#define MUX_PA02A_EIC_EXTINT2           _L_(0)
#define PINMUX_PA02A_EIC_EXTINT2   ((PIN_PA02A_EIC_EXTINT2 << 16) | MUX_PA02A_EIC_EXTINT2)
#define PORT_PA02A_EIC_EXTINT2  (_UL_(1) <<  2)
#define PIN_PA02A_EIC_EXTINT_NUM        _L_(2) /**< \brief EIC signal: PIN_PA02 External Interrupt Line */
#define PIN_PA30A_EIC_EXTINT2          _L_(30) /**< \brief EIC signal: EXTINT2 on PA30 mux A */
#define MUX_PA30A_EIC_EXTINT2           _L_(0)
#define PINMUX_PA30A_EIC_EXTINT2   ((PIN_PA30A_EIC_EXTINT2 << 16) | MUX_PA30A_EIC_EXTINT2)
#define PORT_PA30A_EIC_EXTINT2  (_UL_(1) << 30)
#define PIN_PA30A_EIC_EXTINT_NUM        _L_(2) /**< \brief EIC signal: PIN_PA30 External Interrupt Line */
#define PIN_PA03A_EIC_EXTINT3           _L_(3) /**< \brief EIC signal: EXTINT3 on PA03 mux A */
#define MUX_PA03A_EIC_EXTINT3           _L_(0)
#define PINMUX_PA03A_EIC_EXTINT3   ((PIN_PA03A_EIC_EXTINT3 << 16) | MUX_PA03A_EIC_EXTINT3)
#define PORT_PA03A_EIC_EXTINT3  (_UL_(1) <<  3)
#define PIN_PA03A_EIC_EXTINT_NUM        _L_(3) /**< \brief EIC signal: PIN_PA03 External Interrupt Line */
#define PIN_PA31A_EIC_EXTINT3          _L_(31) /**< \brief EIC signal: EXTINT3 on PA31 mux A */
#define MUX_PA31A_EIC_EXTINT3           _L_(0)
#define PINMUX_PA31A_EIC_EXTINT3   ((PIN_PA31A_EIC_EXTINT3 << 16) | MUX_PA31A_EIC_EXTINT3)
#define PORT_PA31A_EIC_EXTINT3  (_UL_(1) << 31)
#define PIN_PA31A_EIC_EXTINT_NUM        _L_(3) /**< \brief EIC signal: PIN_PA31 External Interrupt Line */
#define PIN_PA04A_EIC_EXTINT4           _L_(4) /**< \brief EIC signal: EXTINT4 on PA04 mux A */
#define MUX_PA04A_EIC_EXTINT4           _L_(0)
#define PINMUX_PA04A_EIC_EXTINT4   ((PIN_PA04A_EIC_EXTINT4 << 16) | MUX_PA04A_EIC_EXTINT4)
#define PORT_PA04A_EIC_EXTINT4  (_UL_(1) <<  4)
#define PIN_PA04A_EIC_EXTINT_NUM        _L_(4) /**< \brief EIC signal: PIN_PA04 External Interrupt Line */
#define PIN_PA24A_EIC_EXTINT4          _L_(24) /**< \brief EIC signal: EXTINT4 on PA24 mux A */
#define MUX_PA24A_EIC_EXTINT4           _L_(0)
#define PINMUX_PA24A_EIC_EXTINT4   ((PIN_PA24A_EIC_EXTINT4 << 16) | MUX_PA24A_EIC_EXTINT4)
#define PORT_PA24A_EIC_EXTINT4  (_UL_(1) << 24)
#define PIN_PA24A_EIC_EXTINT_NUM        _L_(4) /**< \brief EIC signal: PIN_PA24 External Interrupt Line */
#define PIN_PA05A_EIC_EXTINT5           _L_(5) /**< \brief EIC signal: EXTINT5 on PA05 mux A */
#define MUX_PA05A_EIC_EXTINT5           _L_(0)
#define PINMUX_PA05A_EIC_EXTINT5   ((PIN_PA05A_EIC_EXTINT5 << 16) | MUX_PA05A_EIC_EXTINT5)
#define PORT_PA05A_EIC_EXTINT5  (_UL_(1) <<  5)
#define PIN_PA05A_EIC_EXTINT_NUM        _L_(5) /**< \brief EIC signal: PIN_PA05 External Interrupt Line */
#define PIN_PA25A_EIC_EXTINT5          _L_(25) /**< \brief EIC signal: EXTINT5 on PA25 mux A */
#define MUX_PA25A_EIC_EXTINT5           _L_(0)
#define PINMUX_PA25A_EIC_EXTINT5   ((PIN_PA25A_EIC_EXTINT5 << 16) | MUX_PA25A_EIC_EXTINT5)
#define PORT_PA25A_EIC_EXTINT5  (_UL_(1) << 25)
#define PIN_PA25A_EIC_EXTINT_NUM        _L_(5) /**< \brief EIC signal: PIN_PA25 External Interrupt Line */
#define PIN_PA06A_EIC_EXTINT6           _L_(6) /**< \brief EIC signal: EXTINT6 on PA06 mux A */
#define MUX_PA06A_EIC_EXTINT6           _L_(0)
#define PINMUX_PA06A_EIC_EXTINT6   ((PIN_PA06A_EIC_EXTINT6 << 16) | MUX_PA06A_EIC_EXTINT6)
#define PORT_PA06A_EIC_EXTINT6  (_UL_(1) <<  6)
#define PIN_PA06A_EIC_EXTINT_NUM        _L_(6) /**< \brief EIC signal: PIN_PA06 External Interrupt Line */
#define PIN_PA08A_EIC_EXTINT6           _L_(8) /**< \brief EIC signal: EXTINT6 on PA08 mux A */
#define MUX_PA08A_EIC_EXTINT6           _L_(0)
#define PINMUX_PA08A_EIC_EXTINT6   ((PIN_PA08A_EIC_EXTINT6 << 16) | MUX_PA08A_EIC_EXTINT6)
#define PORT_PA08A_EIC_EXTINT6  (_UL_(1) <<  8)
#define PIN_PA08A_EIC_EXTINT_NUM        _L_(6) /**< \brief EIC signal: PIN_PA08 External Interrupt Line */
#define PIN_PA22A_EIC_EXTINT6          _L_(22) /**< \brief EIC signal: EXTINT6 on PA22 mux A */
#define MUX_PA22A_EIC_EXTINT6           _L_(0)
#define PINMUX_PA22A_EIC_EXTINT6   ((PIN_PA22A_EIC_EXTINT6 << 16) | MUX_PA22A_EIC_EXTINT6)
#define PORT_PA22A_EIC_EXTINT6  (_UL_(1) << 22)
#define PIN_PA22A_EIC_EXTINT_NUM        _L_(6) /**< \brief EIC signal: PIN_PA22 External Interrupt Line */
#define PIN_PA07A_EIC_EXTINT7           _L_(7) /**< \brief EIC signal: EXTINT7 on PA07 mux A */
#define MUX_PA07A_EIC_EXTINT7           _L_(0)
#define PINMUX_PA07A_EIC_EXTINT7   ((PIN_PA07A_EIC_EXTINT7 << 16) | MUX_PA07A_EIC_EXTINT7)
#define PORT_PA07A_EIC_EXTINT7  (_UL_(1) <<  7)
#define PIN_PA07A_EIC_EXTINT_NUM        _L_(7) /**< \brief EIC signal: PIN_PA07 External Interrupt Line */
#define PIN_PA09A_EIC_EXTINT7           _L_(9) /**< \brief EIC signal: EXTINT7 on PA09 mux A */
#define MUX_PA09A_EIC_EXTINT7           _L_(0)
#define PINMUX_PA09A_EIC_EXTINT7   ((PIN_PA09A_EIC_EXTINT7 << 16) | MUX_PA09A_EIC_EXTINT7)
#define PORT_PA09A_EIC_EXTINT7  (_UL_(1) <<  9)
#define PIN_PA09A_EIC_EXTINT_NUM        _L_(7) /**< \brief EIC signal: PIN_PA09 External Interrupt Line */
#define PIN_PA23A_EIC_EXTINT7          _L_(23) /**< \brief EIC signal: EXTINT7 on PA23 mux A */
#define MUX_PA23A_EIC_EXTINT7           _L_(0)
#define PINMUX_PA23A_EIC_EXTINT7   ((PIN_PA23A_EIC_EXTINT7 << 16) | MUX_PA23A_EIC_EXTINT7)
#define PORT_PA23A_EIC_EXTINT7  (_UL_(1) << 23)
#define PIN_PA23A_EIC_EXTINT_NUM        _L_(7) /**< \brief EIC signal: PIN_PA23 External Interrupt Line */
#define PIN_PA14A_EIC_NMI              _L_(14) /**< \brief EIC signal: NMI on PA14 mux A */
#define MUX_PA14A_EIC_NMI               _L_(0)
#define PINMUX_PA14A_EIC_NMI       ((PIN_PA14A_EIC_NMI << 16) | MUX_PA14A_EIC_NMI)
#define PORT_PA14A_EIC_NMI     (_UL_(1) << 14)
/* ========== PORT definition for USB peripheral ========== */
#define PIN_PA24G_USB_DM               _L_(24) /**< \brief USB signal: DM on PA24 mux G */
#define MUX_PA24G_USB_DM                _L_(6)
#define PINMUX_PA24G_USB_DM        ((PIN_PA24G_USB_DM << 16) | MUX_PA24G_USB_DM)
#define PORT_PA24G_USB_DM      (_UL_(1) << 24)
#define PIN_PA25G_USB_DP               _L_(25) /**< \brief USB signal: DP on PA25 mux G */
#define MUX_PA25G_USB_DP                _L_(6)
#define PINMUX_PA25G_USB_DP        ((PIN_PA25G_USB_DP << 16) | MUX_PA25G_USB_DP)
#define PORT_PA25G_USB_DP      (_UL_(1) << 25)
#define PIN_PA23G_USB_SOF_1KHZ         _L_(23) /**< \brief USB signal: SOF_1KHZ on PA23 mux G */
#define MUX_PA23G_USB_SOF_1KHZ          _L_(6)
#define PINMUX_PA23G_USB_SOF_1KHZ  ((PIN_PA23G_USB_SOF_1KHZ << 16) | MUX_PA23G_USB_SOF_1KHZ)
#define PORT_PA23G_USB_SOF_1KHZ  (_UL_(1) << 23)
/* ========== PORT definition for SERCOM0 peripheral ========== */
#define PIN_PA04D_SERCOM0_PAD0          _L_(4) /**< \brief SERCOM0 signal: PAD0 on PA04 mux D */
#define MUX_PA04D_SERCOM0_PAD0          _L_(3)
#define PINMUX_PA04D_SERCOM0_PAD0  ((PIN_PA04D_SERCOM0_PAD0 << 16) | MUX_PA04D_SERCOM0_PAD0)
#define PORT_PA04D_SERCOM0_PAD0  (_UL_(1) <<  4)
#define PIN_PA14C_SERCOM0_PAD0         _L_(14) /**< \brief SERCOM0 signal: PAD0 on PA14 mux C */
#define MUX_PA14C_SERCOM0_PAD0          _L_(2)
#define PINMUX_PA14C_SERCOM0_PAD0  ((PIN_PA14C_SERCOM0_PAD0 << 16) | MUX_PA14C_SERCOM0_PAD0)
#define PORT_PA14C_SERCOM0_PAD0  (_UL_(1) << 14)
#define PIN_PA06C_SERCOM0_PAD0          _L_(6) /**< \brief SERCOM0 signal: PAD0 on PA06 mux C */
#define MUX_PA06C_SERCOM0_PAD0          _L_(2)
#define PINMUX_PA06C_SERCOM0_PAD0  ((PIN_PA06C_SERCOM0_PAD0 << 16) | MUX_PA06C_SERCOM0_PAD0)
#define PORT_PA06C_SERCOM0_PAD0  (_UL_(1) <<  6)
#define PIN_PA05D_SERCOM0_PAD1          _L_(5) /**< \brief SERCOM0 signal: PAD1 on PA05 mux D */
#define MUX_PA05D_SERCOM0_PAD1          _L_(3)
#define PINMUX_PA05D_SERCOM0_PAD1  ((PIN_PA05D_SERCOM0_PAD1 << 16) | MUX_PA05D_SERCOM0_PAD1)
#define PORT_PA05D_SERCOM0_PAD1  (_UL_(1) <<  5)
#define PIN_PA15C_SERCOM0_PAD1         _L_(15) /**< \brief SERCOM0 signal: PAD1 on PA15 mux C */
#define MUX_PA15C_SERCOM0_PAD1          _L_(2)
#define PINMUX_PA15C_SERCOM0_PAD1  ((PIN_PA15C_SERCOM0_PAD1 << 16) | MUX_PA15C_SERCOM0_PAD1)
#define PORT_PA15C_SERCOM0_PAD1  (_UL_(1) << 15)
#define PIN_PA07C_SERCOM0_PAD1          _L_(7) /**< \brief SERCOM0 signal: PAD1 on PA07 mux C */
#define MUX_PA07C_SERCOM0_PAD1          _L_(2)
#define PINMUX_PA07C_SERCOM0_PAD1  ((PIN_PA07C_SERCOM0_PAD1 << 16) | MUX_PA07C_SERCOM0_PAD1)
#define PORT_PA07C_SERCOM0_PAD1  (_UL_(1) <<  7)
#define PIN_PA06D_SERCOM0_PAD2          _L_(6) /**< \brief SERCOM0 signal: PAD2 on PA06 mux D */
#define MUX_PA06D_SERCOM0_PAD2          _L_(3)
#define PINMUX_PA06D_SERCOM0_PAD2  ((PIN_PA06D_SERCOM0_PAD2 << 16) | MUX_PA06D_SERCOM0_PAD2)
#define PORT_PA06D_SERCOM0_PAD2  (_UL_(1) <<  6)
#define PIN_PA08D_SERCOM0_PAD2          _L_(8) /**< \brief SERCOM0 signal: PAD2 on PA08 mux D */
#define MUX_PA08D_SERCOM0_PAD2          _L_(3)
#define PINMUX_PA08D_SERCOM0_PAD2  ((PIN_PA08D_SERCOM0_PAD2 << 16) | MUX_PA08D_SERCOM0_PAD2)
#define PORT_PA08D_SERCOM0_PAD2  (_UL_(1) <<  8)
#define PIN_PA04C_SERCOM0_PAD2          _L_(4) /**< \brief SERCOM0 signal: PAD2 on PA04 mux C */
#define MUX_PA04C_SERCOM0_PAD2          _L_(2)
#define PINMUX_PA04C_SERCOM0_PAD2  ((PIN_PA04C_SERCOM0_PAD2 << 16) | MUX_PA04C_SERCOM0_PAD2)
#define PORT_PA04C_SERCOM0_PAD2  (_UL_(1) <<  4)
#define PIN_PA07D_SERCOM0_PAD3          _L_(7) /**< \brief SERCOM0 signal: PAD3 on PA07 mux D */
#define MUX_PA07D_SERCOM0_PAD3          _L_(3)
#define PINMUX_PA07D_SERCOM0_PAD3  ((PIN_PA07D_SERCOM0_PAD3 << 16) | MUX_PA07D_SERCOM0_PAD3)
#define PORT_PA07D_SERCOM0_PAD3  (_UL_(1) <<  7)
#define PIN_PA09D_SERCOM0_PAD3          _L_(9) /**< \brief SERCOM0 signal: PAD3 on PA09 mux D */
#define MUX_PA09D_SERCOM0_PAD3          _L_(3)
#define PINMUX_PA09D_SERCOM0_PAD3  ((PIN_PA09D_SERCOM0_PAD3 << 16) | MUX_PA09D_SERCOM0_PAD3)
#define PORT_PA09D_SERCOM0_PAD3  (_UL_(1) <<  9)
#define PIN_PA05C_SERCOM0_PAD3          _L_(5) /**< \brief SERCOM0 signal: PAD3 on PA05 mux C */
#define MUX_PA05C_SERCOM0_PAD3          _L_(2)
#define PINMUX_PA05C_SERCOM0_PAD3  ((PIN_PA05C_SERCOM0_PAD3 << 16) | MUX_PA05C_SERCOM0_PAD3)
#define PORT_PA05C_SERCOM0_PAD3  (_UL_(1) <<  5)
/* ========== PORT definition for SERCOM1 peripheral ========== */
#define PIN_PA22C_SERCOM1_PAD0         _L_(22) /**< \brief SERCOM1 signal: PAD0 on PA22 mux C */
#define MUX_PA22C_SERCOM1_PAD0          _L_(2)
#define PINMUX_PA22C_SERCOM1_PAD0  ((PIN_PA22C_SERCOM1_PAD0 << 16) | MUX_PA22C_SERCOM1_PAD0)
#define PORT_PA22C_SERCOM1_PAD0  (_UL_(1) << 22)
#define PIN_PA30C_SERCOM1_PAD0         _L_(30) /**< \brief SERCOM1 signal: PAD0 on PA30 mux C */
#define MUX_PA30C_SERCOM1_PAD0          _L_(2)
#define PINMUX_PA30C_SERCOM1_PAD0  ((PIN_PA30C_SERCOM1_PAD0 << 16) | MUX_PA30C_SERCOM1_PAD0)
#define PORT_PA30C_SERCOM1_PAD0  (_UL_(1) << 30)
#define PIN_PA23C_SERCOM1_PAD1         _L_(23) /**< \brief SERCOM1 signal: PAD1 on PA23 mux C */
#define MUX_PA23C_SERCOM1_PAD1          _L_(2)
#define PINMUX_PA23C_SERCOM1_PAD1  ((PIN_PA23C_SERCOM1_PAD1 << 16) | MUX_PA23C_SERCOM1_PAD1)
#define PORT_PA23C_SERCOM1_PAD1  (_UL_(1) << 23)
#define PIN_PA31C_SERCOM1_PAD1         _L_(31) /**< \brief SERCOM1 signal: PAD1 on PA31 mux C */
#define MUX_PA31C_SERCOM1_PAD1          _L_(2)
#define PINMUX_PA31C_SERCOM1_PAD1  ((PIN_PA31C_SERCOM1_PAD1 << 16) | MUX_PA31C_SERCOM1_PAD1)
#define PORT_PA31C_SERCOM1_PAD1  (_UL_(1) << 31)
#define PIN_PA30D_SERCOM1_PAD2         _L_(30) /**< \brief SERCOM1 signal: PAD2 on PA30 mux D */
#define MUX_PA30D_SERCOM1_PAD2          _L_(3)
#define PINMUX_PA30D_SERCOM1_PAD2  ((PIN_PA30D_SERCOM1_PAD2 << 16) | MUX_PA30D_SERCOM1_PAD2)
#define PORT_PA30D_SERCOM1_PAD2  (_UL_(1) << 30)
#define PIN_PA16C_SERCOM1_PAD2         _L_(16) /**< \brief SERCOM1 signal: PAD2 on PA16 mux C */
#define MUX_PA16C_SERCOM1_PAD2          _L_(2)
#define PINMUX_PA16C_SERCOM1_PAD2  ((PIN_PA16C_SERCOM1_PAD2 << 16) | MUX_PA16C_SERCOM1_PAD2)
#define PORT_PA16C_SERCOM1_PAD2  (_UL_(1) << 16)
#define PIN_PA24C_SERCOM1_PAD2         _L_(24) /**< \brief SERCOM1 signal: PAD2 on PA24 mux C */
#define MUX_PA24C_SERCOM1_PAD2          _L_(2)
#define PINMUX_PA24C_SERCOM1_PAD2  ((PIN_PA24C_SERCOM1_PAD2 << 16) | MUX_PA24C_SERCOM1_PAD2)
#define PORT_PA24C_SERCOM1_PAD2  (_UL_(1) << 24)
#define PIN_PA08C_SERCOM1_PAD2          _L_(8) /**< \brief SERCOM1 signal: PAD2 on PA08 mux C */
#define MUX_PA08C_SERCOM1_PAD2          _L_(2)
#define PINMUX_PA08C_SERCOM1_PAD2  ((PIN_PA08C_SERCOM1_PAD2 << 16) | MUX_PA08C_SERCOM1_PAD2)
#define PORT_PA08C_SERCOM1_PAD2  (_UL_(1) <<  8)
#define PIN_PA31D_SERCOM1_PAD3         _L_(31) /**< \brief SERCOM1 signal: PAD3 on PA31 mux D */
#define MUX_PA31D_SERCOM1_PAD3          _L_(3)
#define PINMUX_PA31D_SERCOM1_PAD3  ((PIN_PA31D_SERCOM1_PAD3 << 16) | MUX_PA31D_SERCOM1_PAD3)
#define PORT_PA31D_SERCOM1_PAD3  (_UL_(1) << 31)
#define PIN_PA25C_SERCOM1_PAD3         _L_(25) /**< \brief SERCOM1 signal: PAD3 on PA25 mux C */
#define MUX_PA25C_SERCOM1_PAD3          _L_(2)
#define PINMUX_PA25C_SERCOM1_PAD3  ((PIN_PA25C_SERCOM1_PAD3 << 16) | MUX_PA25C_SERCOM1_PAD3)
#define PORT_PA25C_SERCOM1_PAD3  (_UL_(1) << 25)
#define PIN_PA09C_SERCOM1_PAD3          _L_(9) /**< \brief SERCOM1 signal: PAD3 on PA09 mux C */
#define MUX_PA09C_SERCOM1_PAD3          _L_(2)
#define PINMUX_PA09C_SERCOM1_PAD3  ((PIN_PA09C_SERCOM1_PAD3 << 16) | MUX_PA09C_SERCOM1_PAD3)
#define PORT_PA09C_SERCOM1_PAD3  (_UL_(1) <<  9)
/* ========== PORT definition for SERCOM2 peripheral ========== */
#define PIN_PA14D_SERCOM2_PAD0         _L_(14) /**< \brief SERCOM2 signal: PAD0 on PA14 mux D */
#define MUX_PA14D_SERCOM2_PAD0          _L_(3)
#define PINMUX_PA14D_SERCOM2_PAD0  ((PIN_PA14D_SERCOM2_PAD0 << 16) | MUX_PA14D_SERCOM2_PAD0)
#define PORT_PA14D_SERCOM2_PAD0  (_UL_(1) << 14)
#define PIN_PA22D_SERCOM2_PAD0         _L_(22) /**< \brief SERCOM2 signal: PAD0 on PA22 mux D */
#define MUX_PA22D_SERCOM2_PAD0          _L_(3)
#define PINMUX_PA22D_SERCOM2_PAD0  ((PIN_PA22D_SERCOM2_PAD0 << 16) | MUX_PA22D_SERCOM2_PAD0)
#define PORT_PA22D_SERCOM2_PAD0  (_UL_(1) << 22)
#define PIN_PA15D_SERCOM2_PAD1         _L_(15) /**< \brief SERCOM2 signal: PAD1 on PA15 mux D */
#define MUX_PA15D_SERCOM2_PAD1          _L_(3)
#define PINMUX_PA15D_SERCOM2_PAD1  ((PIN_PA15D_SERCOM2_PAD1 << 16) | MUX_PA15D_SERCOM2_PAD1)
#define PORT_PA15D_SERCOM2_PAD1  (_UL_(1) << 15)
#define PIN_PA23D_SERCOM2_PAD1         _L_(23) /**< \brief SERCOM2 signal: PAD1 on PA23 mux D */
#define MUX_PA23D_SERCOM2_PAD1          _L_(3)
#define PINMUX_PA23D_SERCOM2_PAD1  ((PIN_PA23D_SERCOM2_PAD1 << 16) | MUX_PA23D_SERCOM2_PAD1)
#define PORT_PA23D_SERCOM2_PAD1  (_UL_(1) << 23)
#define PIN_PA16D_SERCOM2_PAD2         _L_(16) /**< \brief SERCOM2 signal: PAD2 on PA16 mux D */
#define MUX_PA16D_SERCOM2_PAD2          _L_(3)
#define PINMUX_PA16D_SERCOM2_PAD2  ((PIN_PA16D_SERCOM2_PAD2 << 16) | MUX_PA16D_SERCOM2_PAD2)
#define PORT_PA16D_SERCOM2_PAD2  (_UL_(1) << 16)
#define PIN_PA24D_SERCOM2_PAD2         _L_(24) /**< \brief SERCOM2 signal: PAD2 on PA24 mux D */
#define MUX_PA24D_SERCOM2_PAD2          _L_(3)
#define PINMUX_PA24D_SERCOM2_PAD2  ((PIN_PA24D_SERCOM2_PAD2 << 16) | MUX_PA24D_SERCOM2_PAD2)
#define PORT_PA24D_SERCOM2_PAD2  (_UL_(1) << 24)
#define PIN_PA25D_SERCOM2_PAD3         _L_(25) /**< \brief SERCOM2 signal: PAD3 on PA25 mux D */
#define MUX_PA25D_SERCOM2_PAD3          _L_(3)
#define PINMUX_PA25D_SERCOM2_PAD3  ((PIN_PA25D_SERCOM2_PAD3 << 16) | MUX_PA25D_SERCOM2_PAD3)
#define PORT_PA25D_SERCOM2_PAD3  (_UL_(1) << 25)
/* ========== PORT definition for TCC0 peripheral ========== */
#define PIN_PA04F_TCC0_WO0              _L_(4) /**< \brief TCC0 signal: WO0 on PA04 mux F */
#define MUX_PA04F_TCC0_WO0              _L_(5)
#define PINMUX_PA04F_TCC0_WO0      ((PIN_PA04F_TCC0_WO0 << 16) | MUX_PA04F_TCC0_WO0)
#define PORT_PA04F_TCC0_WO0    (_UL_(1) <<  4)
#define PIN_PA14F_TCC0_WO0             _L_(14) /**< \brief TCC0 signal: WO0 on PA14 mux F */
#define MUX_PA14F_TCC0_WO0              _L_(5)
#define PINMUX_PA14F_TCC0_WO0      ((PIN_PA14F_TCC0_WO0 << 16) | MUX_PA14F_TCC0_WO0)
#define PORT_PA14F_TCC0_WO0    (_UL_(1) << 14)
#define PIN_PA05F_TCC0_WO1              _L_(5) /**< \brief TCC0 signal: WO1 on PA05 mux F */
#define MUX_PA05F_TCC0_WO1              _L_(5)
#define PINMUX_PA05F_TCC0_WO1      ((PIN_PA05F_TCC0_WO1 << 16) | MUX_PA05F_TCC0_WO1)
#define PORT_PA05F_TCC0_WO1    (_UL_(1) <<  5)
#define PIN_PA15F_TCC0_WO1             _L_(15) /**< \brief TCC0 signal: WO1 on PA15 mux F */
#define MUX_PA15F_TCC0_WO1              _L_(5)
#define PINMUX_PA15F_TCC0_WO1      ((PIN_PA15F_TCC0_WO1 << 16) | MUX_PA15F_TCC0_WO1)
#define PORT_PA15F_TCC0_WO1    (_UL_(1) << 15)
#define PIN_PA06F_TCC0_WO2              _L_(6) /**< \brief TCC0 signal: WO2 on PA06 mux F */
#define MUX_PA06F_TCC0_WO2              _L_(5)
#define PINMUX_PA06F_TCC0_WO2      ((PIN_PA06F_TCC0_WO2 << 16) | MUX_PA06F_TCC0_WO2)
#define PORT_PA06F_TCC0_WO2    (_UL_(1) <<  6)
#define PIN_PA30F_TCC0_WO2             _L_(30) /**< \brief TCC0 signal: WO2 on PA30 mux F */
#define MUX_PA30F_TCC0_WO2              _L_(5)
#define PINMUX_PA30F_TCC0_WO2      ((PIN_PA30F_TCC0_WO2 << 16) | MUX_PA30F_TCC0_WO2)
#define PORT_PA30F_TCC0_WO2    (_UL_(1) << 30)
#define PIN_PA08E_TCC0_WO2              _L_(8) /**< \brief TCC0 signal: WO2 on PA08 mux E */
#define MUX_PA08E_TCC0_WO2              _L_(4)
#define PINMUX_PA08E_TCC0_WO2      ((PIN_PA08E_TCC0_WO2 << 16) | MUX_PA08E_TCC0_WO2)
#define PORT_PA08E_TCC0_WO2    (_UL_(1) <<  8)
#define PIN_PA24E_TCC0_WO2             _L_(24) /**< \brief TCC0 signal: WO2 on PA24 mux E */
#define MUX_PA24E_TCC0_WO2              _L_(4)
#define PINMUX_PA24E_TCC0_WO2      ((PIN_PA24E_TCC0_WO2 << 16) | MUX_PA24E_TCC0_WO2)
#define PORT_PA24E_TCC0_WO2    (_UL_(1) << 24)
#define PIN_PA07F_TCC0_WO3              _L_(7) /**< \brief TCC0 signal: WO3 on PA07 mux F */
#define MUX_PA07F_TCC0_WO3              _L_(5)
#define PINMUX_PA07F_TCC0_WO3      ((PIN_PA07F_TCC0_WO3 << 16) | MUX_PA07F_TCC0_WO3)
#define PORT_PA07F_TCC0_WO3    (_UL_(1) <<  7)
#define PIN_PA31F_TCC0_WO3             _L_(31) /**< \brief TCC0 signal: WO3 on PA31 mux F */
#define MUX_PA31F_TCC0_WO3              _L_(5)
#define PINMUX_PA31F_TCC0_WO3      ((PIN_PA31F_TCC0_WO3 << 16) | MUX_PA31F_TCC0_WO3)
#define PORT_PA31F_TCC0_WO3    (_UL_(1) << 31)
#define PIN_PA09E_TCC0_WO3              _L_(9) /**< \brief TCC0 signal: WO3 on PA09 mux E */
#define MUX_PA09E_TCC0_WO3              _L_(4)
#define PINMUX_PA09E_TCC0_WO3      ((PIN_PA09E_TCC0_WO3 << 16) | MUX_PA09E_TCC0_WO3)
#define PORT_PA09E_TCC0_WO3    (_UL_(1) <<  9)
#define PIN_PA25E_TCC0_WO3             _L_(25) /**< \brief TCC0 signal: WO3 on PA25 mux E */
#define MUX_PA25E_TCC0_WO3              _L_(4)
#define PINMUX_PA25E_TCC0_WO3      ((PIN_PA25E_TCC0_WO3 << 16) | MUX_PA25E_TCC0_WO3)
#define PORT_PA25E_TCC0_WO3    (_UL_(1) << 25)
#define PIN_PA22F_TCC0_WO4             _L_(22) /**< \brief TCC0 signal: WO4 on PA22 mux F */
#define MUX_PA22F_TCC0_WO4              _L_(5)
#define PINMUX_PA22F_TCC0_WO4      ((PIN_PA22F_TCC0_WO4 << 16) | MUX_PA22F_TCC0_WO4)
#define PORT_PA22F_TCC0_WO4    (_UL_(1) << 22)
#define PIN_PA24F_TCC0_WO4             _L_(24) /**< \brief TCC0 signal: WO4 on PA24 mux F */
#define MUX_PA24F_TCC0_WO4              _L_(5)
#define PINMUX_PA24F_TCC0_WO4      ((PIN_PA24F_TCC0_WO4 << 16) | MUX_PA24F_TCC0_WO4)
#define PORT_PA24F_TCC0_WO4    (_UL_(1) << 24)
#define PIN_PA08F_TCC0_WO4              _L_(8) /**< \brief TCC0 signal: WO4 on PA08 mux F */
#define MUX_PA08F_TCC0_WO4              _L_(5)
#define PINMUX_PA08F_TCC0_WO4      ((PIN_PA08F_TCC0_WO4 << 16) | MUX_PA08F_TCC0_WO4)
#define PORT_PA08F_TCC0_WO4    (_UL_(1) <<  8)
#define PIN_PA23F_TCC0_WO5             _L_(23) /**< \brief TCC0 signal: WO5 on PA23 mux F */
#define MUX_PA23F_TCC0_WO5              _L_(5)
#define PINMUX_PA23F_TCC0_WO5      ((PIN_PA23F_TCC0_WO5 << 16) | MUX_PA23F_TCC0_WO5)
#define PORT_PA23F_TCC0_WO5    (_UL_(1) << 23)
#define PIN_PA25F_TCC0_WO5             _L_(25) /**< \brief TCC0 signal: WO5 on PA25 mux F */
#define MUX_PA25F_TCC0_WO5              _L_(5)
#define PINMUX_PA25F_TCC0_WO5      ((PIN_PA25F_TCC0_WO5 << 16) | MUX_PA25F_TCC0_WO5)
#define PORT_PA25F_TCC0_WO5    (_UL_(1) << 25)
#define PIN_PA09F_TCC0_WO5              _L_(9) /**< \brief TCC0 signal: WO5 on PA09 mux F */
#define MUX_PA09F_TCC0_WO5              _L_(5)
#define PINMUX_PA09F_TCC0_WO5      ((PIN_PA09F_TCC0_WO5 << 16) | MUX_PA09F_TCC0_WO5)
#define PORT_PA09F_TCC0_WO5    (_UL_(1) <<  9)
#define PIN_PA16F_TCC0_WO6             _L_(16) /**< \brief TCC0 signal: WO6 on PA16 mux F */
#define MUX_PA16F_TCC0_WO6              _L_(5)
#define PINMUX_PA16F_TCC0_WO6      ((PIN_PA16F_TCC0_WO6 << 16) | MUX_PA16F_TCC0_WO6)
#define PORT_PA16F_TCC0_WO6    (_UL_(1) << 16)
/* ========== PORT definition for TC1 peripheral ========== */
#define PIN_PA04E_TC1_WO0               _L_(4) /**< \brief TC1 signal: WO0 on PA04 mux E */
#define MUX_PA04E_TC1_WO0               _L_(4)
#define PINMUX_PA04E_TC1_WO0       ((PIN_PA04E_TC1_WO0 << 16) | MUX_PA04E_TC1_WO0)
#define PORT_PA04E_TC1_WO0     (_UL_(1) <<  4)
#define PIN_PA14E_TC1_WO0              _L_(14) /**< \brief TC1 signal: WO0 on PA14 mux E */
#define MUX_PA14E_TC1_WO0               _L_(4)
#define PINMUX_PA14E_TC1_WO0       ((PIN_PA14E_TC1_WO0 << 16) | MUX_PA14E_TC1_WO0)
#define PORT_PA14E_TC1_WO0     (_UL_(1) << 14)
#define PIN_PA16E_TC1_WO0              _L_(16) /**< \brief TC1 signal: WO0 on PA16 mux E */
#define MUX_PA16E_TC1_WO0               _L_(4)
#define PINMUX_PA16E_TC1_WO0       ((PIN_PA16E_TC1_WO0 << 16) | MUX_PA16E_TC1_WO0)
#define PORT_PA16E_TC1_WO0     (_UL_(1) << 16)
#define PIN_PA22E_TC1_WO0              _L_(22) /**< \brief TC1 signal: WO0 on PA22 mux E */
#define MUX_PA22E_TC1_WO0               _L_(4)
#define PINMUX_PA22E_TC1_WO0       ((PIN_PA22E_TC1_WO0 << 16) | MUX_PA22E_TC1_WO0)
#define PORT_PA22E_TC1_WO0     (_UL_(1) << 22)
#define PIN_PA05E_TC1_WO1               _L_(5) /**< \brief TC1 signal: WO1 on PA05 mux E */
#define MUX_PA05E_TC1_WO1               _L_(4)
#define PINMUX_PA05E_TC1_WO1       ((PIN_PA05E_TC1_WO1 << 16) | MUX_PA05E_TC1_WO1)
#define PORT_PA05E_TC1_WO1     (_UL_(1) <<  5)
#define PIN_PA15E_TC1_WO1              _L_(15) /**< \brief TC1 signal: WO1 on PA15 mux E */
#define MUX_PA15E_TC1_WO1               _L_(4)
#define PINMUX_PA15E_TC1_WO1       ((PIN_PA15E_TC1_WO1 << 16) | MUX_PA15E_TC1_WO1)
#define PORT_PA15E_TC1_WO1     (_UL_(1) << 15)
#define PIN_PA23E_TC1_WO1              _L_(23) /**< \brief TC1 signal: WO1 on PA23 mux E */
#define MUX_PA23E_TC1_WO1               _L_(4)
#define PINMUX_PA23E_TC1_WO1       ((PIN_PA23E_TC1_WO1 << 16) | MUX_PA23E_TC1_WO1)
#define PORT_PA23E_TC1_WO1     (_UL_(1) << 23)
/* ========== PORT definition for TC2 peripheral ========== */
#define PIN_PA06E_TC2_WO0               _L_(6) /**< \brief TC2 signal: WO0 on PA06 mux E */
#define MUX_PA06E_TC2_WO0               _L_(4)
#define PINMUX_PA06E_TC2_WO0       ((PIN_PA06E_TC2_WO0 << 16) | MUX_PA06E_TC2_WO0)
#define PORT_PA06E_TC2_WO0     (_UL_(1) <<  6)
#define PIN_PA30E_TC2_WO0              _L_(30) /**< \brief TC2 signal: WO0 on PA30 mux E */
#define MUX_PA30E_TC2_WO0               _L_(4)
#define PINMUX_PA30E_TC2_WO0       ((PIN_PA30E_TC2_WO0 << 16) | MUX_PA30E_TC2_WO0)
#define PORT_PA30E_TC2_WO0     (_UL_(1) << 30)
#define PIN_PA07E_TC2_WO1               _L_(7) /**< \brief TC2 signal: WO1 on PA07 mux E */
#define MUX_PA07E_TC2_WO1               _L_(4)
#define PINMUX_PA07E_TC2_WO1       ((PIN_PA07E_TC2_WO1 << 16) | MUX_PA07E_TC2_WO1)
#define PORT_PA07E_TC2_WO1     (_UL_(1) <<  7)
#define PIN_PA31E_TC2_WO1              _L_(31) /**< \brief TC2 signal: WO1 on PA31 mux E */
#define MUX_PA31E_TC2_WO1               _L_(4)
#define PINMUX_PA31E_TC2_WO1       ((PIN_PA31E_TC2_WO1 << 16) | MUX_PA31E_TC2_WO1)
#define PORT_PA31E_TC2_WO1     (_UL_(1) << 31)
/* ========== PORT definition for ADC peripheral ========== */
#define PIN_PA02B_ADC_AIN0              _L_(2) /**< \brief ADC signal: AIN0 on PA02 mux B */
#define MUX_PA02B_ADC_AIN0              _L_(1)
#define PINMUX_PA02B_ADC_AIN0      ((PIN_PA02B_ADC_AIN0 << 16) | MUX_PA02B_ADC_AIN0)
#define PORT_PA02B_ADC_AIN0    (_UL_(1) <<  2)
#define PIN_PA03B_ADC_AIN1              _L_(3) /**< \brief ADC signal: AIN1 on PA03 mux B */
#define MUX_PA03B_ADC_AIN1              _L_(1)
#define PINMUX_PA03B_ADC_AIN1      ((PIN_PA03B_ADC_AIN1 << 16) | MUX_PA03B_ADC_AIN1)
#define PORT_PA03B_ADC_AIN1    (_UL_(1) <<  3)
#define PIN_PA04B_ADC_AIN2              _L_(4) /**< \brief ADC signal: AIN2 on PA04 mux B */
#define MUX_PA04B_ADC_AIN2              _L_(1)
#define PINMUX_PA04B_ADC_AIN2      ((PIN_PA04B_ADC_AIN2 << 16) | MUX_PA04B_ADC_AIN2)
#define PORT_PA04B_ADC_AIN2    (_UL_(1) <<  4)
#define PIN_PA05B_ADC_AIN3              _L_(5) /**< \brief ADC signal: AIN3 on PA05 mux B */
#define MUX_PA05B_ADC_AIN3              _L_(1)
#define PINMUX_PA05B_ADC_AIN3      ((PIN_PA05B_ADC_AIN3 << 16) | MUX_PA05B_ADC_AIN3)
#define PORT_PA05B_ADC_AIN3    (_UL_(1) <<  5)
#define PIN_PA06B_ADC_AIN4              _L_(6) /**< \brief ADC signal: AIN4 on PA06 mux B */
#define MUX_PA06B_ADC_AIN4              _L_(1)
#define PINMUX_PA06B_ADC_AIN4      ((PIN_PA06B_ADC_AIN4 << 16) | MUX_PA06B_ADC_AIN4)
#define PORT_PA06B_ADC_AIN4    (_UL_(1) <<  6)
#define PIN_PA07B_ADC_AIN5              _L_(7) /**< \brief ADC signal: AIN5 on PA07 mux B */
#define MUX_PA07B_ADC_AIN5              _L_(1)
#define PINMUX_PA07B_ADC_AIN5      ((PIN_PA07B_ADC_AIN5 << 16) | MUX_PA07B_ADC_AIN5)
#define PORT_PA07B_ADC_AIN5    (_UL_(1) <<  7)
#define PIN_PA14B_ADC_AIN6             _L_(14) /**< \brief ADC signal: AIN6 on PA14 mux B */
#define MUX_PA14B_ADC_AIN6              _L_(1)
#define PINMUX_PA14B_ADC_AIN6      ((PIN_PA14B_ADC_AIN6 << 16) | MUX_PA14B_ADC_AIN6)
#define PORT_PA14B_ADC_AIN6    (_UL_(1) << 14)
#define PIN_PA15B_ADC_AIN7             _L_(15) /**< \brief ADC signal: AIN7 on PA15 mux B */
#define MUX_PA15B_ADC_AIN7              _L_(1)
#define PINMUX_PA15B_ADC_AIN7      ((PIN_PA15B_ADC_AIN7 << 16) | MUX_PA15B_ADC_AIN7)
#define PORT_PA15B_ADC_AIN7    (_UL_(1) << 15)
#define PIN_PA04B_ADC_VREFP             _L_(4) /**< \brief ADC signal: VREFP on PA04 mux B */
#define MUX_PA04B_ADC_VREFP             _L_(1)
#define PINMUX_PA04B_ADC_VREFP     ((PIN_PA04B_ADC_VREFP << 16) | MUX_PA04B_ADC_VREFP)
#define PORT_PA04B_ADC_VREFP   (_UL_(1) <<  4)
/* ========== PORT definition for AC peripheral ========== */
#define PIN_PA04B_AC_AIN0               _L_(4) /**< \brief AC signal: AIN0 on PA04 mux B */
#define MUX_PA04B_AC_AIN0               _L_(1)
#define PINMUX_PA04B_AC_AIN0       ((PIN_PA04B_AC_AIN0 << 16) | MUX_PA04B_AC_AIN0)
#define PORT_PA04B_AC_AIN0     (_UL_(1) <<  4)
#define PIN_PA05B_AC_AIN1               _L_(5) /**< \brief AC signal: AIN1 on PA05 mux B */
#define MUX_PA05B_AC_AIN1               _L_(1)
#define PINMUX_PA05B_AC_AIN1       ((PIN_PA05B_AC_AIN1 << 16) | MUX_PA05B_AC_AIN1)
#define PORT_PA05B_AC_AIN1     (_UL_(1) <<  5)
#define PIN_PA06B_AC_AIN2               _L_(6) /**< \brief AC signal: AIN2 on PA06 mux B */
#define MUX_PA06B_AC_AIN2               _L_(1)
#define PINMUX_PA06B_AC_AIN2       ((PIN_PA06B_AC_AIN2 << 16) | MUX_PA06B_AC_AIN2)
#define PORT_PA06B_AC_AIN2     (_UL_(1) <<  6)
#define PIN_PA07B_AC_AIN3               _L_(7) /**< \brief AC signal: AIN3 on PA07 mux B */
#define MUX_PA07B_AC_AIN3               _L_(1)
#define PINMUX_PA07B_AC_AIN3       ((PIN_PA07B_AC_AIN3 << 16) | MUX_PA07B_AC_AIN3)
#define PORT_PA07B_AC_AIN3     (_UL_(1) <<  7)
#define PIN_PA14G_AC_CMP0              _L_(14) /**< \brief AC signal: CMP0 on PA14 mux G */
#define MUX_PA14G_AC_CMP0               _L_(6)
#define PINMUX_PA14G_AC_CMP0       ((PIN_PA14G_AC_CMP0 << 16) | MUX_PA14G_AC_CMP0)
#define PORT_PA14G_AC_CMP0     (_UL_(1) << 14)
#define PIN_PA15G_AC_CMP1              _L_(15) /**< \brief AC signal: CMP1 on PA15 mux G */
#define MUX_PA15G_AC_CMP1               _L_(6)
#define PINMUX_PA15G_AC_CMP1       ((PIN_PA15G_AC_CMP1 << 16) | MUX_PA15G_AC_CMP1)
#define PORT_PA15G_AC_CMP1     (_UL_(1) << 15)
/* ========== PORT definition for DAC peripheral ========== */
#define PIN_PA02B_DAC_VOUT              _L_(2) /**< \brief DAC signal: VOUT on PA02 mux B */
#define MUX_PA02B_DAC_VOUT              _L_(1)
#define PINMUX_PA02B_DAC_VOUT      ((PIN_PA02B_DAC_VOUT << 16) | MUX_PA02B_DAC_VOUT)
#define PORT_PA02B_DAC_VOUT    (_UL_(1) <<  2)
#define PIN_PA03B_DAC_VREFP             _L_(3) /**< \brief DAC signal: VREFP on PA03 mux B */
#define MUX_PA03B_DAC_VREFP             _L_(1)
#define PINMUX_PA03B_DAC_VREFP     ((PIN_PA03B_DAC_VREFP << 16) | MUX_PA03B_DAC_VREFP)
#define PORT_PA03B_DAC_VREFP   (_UL_(1) <<  3)

#endif /* _SAMD11D14AS_PIO_ */
