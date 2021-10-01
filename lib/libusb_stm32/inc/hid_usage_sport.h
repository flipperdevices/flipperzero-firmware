/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _USB_HID_USAGE_SPORT_H_
#define _USB_HID_USAGE_SPORT_H_
#ifdef __cplusplus
    extern "C" {
#endif


/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_SPORT HID Usage Tables for Sport
 * \brief Contains USB HID Usages definitions for Sport Controls Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_SPORT                  0x04

#define HID_SPORT_BASEBALL_BAT          0x01
#define HID_SPORT_GOLF_CLUB             0x02
#define HID_SPORT_ROWING_MACHINE        0x03
#define HID_SPORT_TREADMILL             0x04
#define HID_SPORT_OAR                   0x30
#define HID_SPORT_SLOPE                 0x31
#define HID_SPORT_RATE                  0x32
#define HID_SPORT_STICK_SPEED           0x33
#define HID_SPORT_STICK_FACE_ANGLE      0x34
#define HID_SPORT_STICK_HEEL_TOE        0x35
#define HID_SPORT_STICK_FOLLOW_THROUGH  0x36
#define HID_SPORT_STICK_TEMPO           0x37
#define HID_SPORT_STICK_TYPE            0x38
#define HID_SPORT_STICK_HEIGHT          0x39
#define HID_SPORT_PUTTER                0x50
#define HID_SPORT_1_IRON                0x51
#define HID_SPORT_2_IRON                0x52
#define HID_SPORT_3_IRON                0x53
#define HID_SPORT_4_IRON                0x54
#define HID_SPORT_5_IRON                0x55
#define HID_SPORT_6_IRON                0x56
#define HID_SPORT_7_IRON                0x57
#define HID_SPORT_8_IRON                0x58
#define HID_SPORT_9_IRON                0x59
#define HID_SPORT_10_IRON               0x5A
#define HID_SPORT_11_IRON               0x5B
#define HID_SPORT_SAND_WEDGE            0x5C
#define HID_SPORT_LOFT_WEDGE            0x5D
#define HID_SPORT_POWER_WEDGE           0x5E
#define HID_SPORT_1_WOOD                0x5F
#define HID_SPORT_3_WOOD                0x60
#define HID_SPORT_5_WOOD                0x61
#define HID_SPORT_7_WOOD                0x62
#define HID_SPORT_9_WOOD                0x63

/** @}  */

#ifdef __cplusplus
    }
#endif

#endif

