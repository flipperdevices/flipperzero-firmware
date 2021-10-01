/* This file is the part of the LUS32 project
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

#ifndef _USB_HID_USAGE_H_
#define _USB_HID_USAGE_H_

/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_GAME HID Usage Tables for Game
 * \brief Contains USB HID Usages definitions for Game Controls Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_GAME                   0x05

#define HID_GAME_3D_GAME_CONTROLLER     0x01
#define HID_GAME_PINBALL_DEVICE         0x02
#define HID_GAME_GUN_DEVICE             0x03
#define HID_GAME_POINT_OF_VIEW          0x20
#define HID_GAME_TURN_LEFT_RIGHT        0x21
#define HID_GAME_PITCH_FWD_BACK         0x22
#define HID_GAME_ROLL_LEFT_RIGHT        0x23
#define HID_GAME_MOVE_LEFT_RIGHT        0x24
#define HID_GAME_MOVE_FWD_BACK          0x25
#define HID_GAME_MOVE_UP_DOWN           0x26
#define HID_GAME_LEAN_LEFT_RIGHT        0x27
#define HID_GAME_LEAN_FWD_BACK          0x28
#define HID_GAME_HEIGHT_OF_POV          0x29
#define HID_GAME_FLIPPER                0x2A
#define HID_GAME_SECONDARY_FLIPPER      0x2B
#define HID_GAME_BUMP                   0x2C
#define HID_GAME_NEW_GAME               0x2D
#define HID_GAME_SHOOT_BALL             0x2E
#define HID_GAME_PLAYER                 0x2F
#define HID_GAME_GUN_BOLT               0x30
#define HID_GAME_GUN_CLIP               0x31
#define HID_GAME_GUN_SELECTOR           0x32
#define HID_GAME_GUN_SINGLE_SHOT        0x33
#define HID_GAME_GUN_BURST              0x34
#define HID_GAME_GUN_AUTOMATIC          0x35
#define HID_GAME_GUN_SAFETY             0x36
#define HID_GAME_GANEPAD_FIRE_JUMP      0x37
#define HID_GAME_GAMEPAD_TRIGGER        0x38
/** @} */
#endif

