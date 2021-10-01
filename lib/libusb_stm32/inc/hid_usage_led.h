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

#ifndef _USB_HID_USAGE_LED_H_
#define _USB_HID_USAGE_LED_H_

/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_LED HID Usages for LED's
 * \brief Contains USB HID Usages definitions for LED's Control Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_LED                    0x08

#define HID_LED_NUM_LOCK                0x01
#define HID_LED_CAPS_LOCK               0x02
#define HID_LED_SCROLL_LOCK             0x03
#define HID_LED_COMPOSE                 0x04
#define HID_LED_KANA                    0x05
#define HID_LED_POWER                   0x06
#define HID_LED_SHIFT                   0x07
#define HID_LED_DO_NOT_DISTURB          0x08
#define HID_LED_MUTE                    0x09
#define HID_LED_TONE_ENABLE             0x0A
#define HID_LED_HIGH_CUT_FILTER         0x0B
#define HID_LED_LOW_CUT_FILTER          0x0C
#define HID_LED_EQUALIZER_ENABLE        0x0D
#define HID_LED_SOUND_FIELD_ON          0x0E
#define HID_LED_SURROUND_ON             0x0F
#define HID_LED_REPEAT                  0x10
#define HID_LED_STEREO                  0x11
#define HID_LED_SAMPLING_RATE_DETECT    0x12
#define HID_LED_SPINNING                0x13
#define HID_LED_CAV                     0x14
#define HID_LED_CLV                     0x15
#define HID_LED_REC_FORMAT_DETECT       0x16
#define HID_LED_OFF_HOOK                0x17
#define HID_LED_RING                    0x18
#define HID_LED_MESSAGE_WAITING         0x19
#define HID_LED_DATA_MODE               0x1A
#define HID_LED_BATTERY_OPERATION       0x1B
#define HID_LED_BATTERY_OK              0x1C
#define HID_LED_BATTERY_LOW             0x1D
#define HID_LED_SPEAKER                 0x1E
#define HID_LED_HEADSET                 0x1F
#define HID_LED_HOLD                    0x20
#define HID_LED_MICROPHONE              0x21
#define HID_LED_COVERAGE                0x22
#define HID_LED_NIGHT_MODE              0x23
#define HID_LED_SEND_CALLS              0x24
#define HID_LED_CALL_PICKUP             0x25
#define HID_LED_CONFERENCE              0x26
#define HID_LED_STANDBY                 0x27
#define HID_LED_CAMERA_ON               0x28
#define HID_LED_CAMERA_OFF              0x29
#define HID_LED_ONLINE                  0x2A
#define HID_LED_OFFLINE                 0x2B
#define HID_LED_BUSY                    0x2C
#define HID_LED_READY                   0x2D
#define HID_LED_PAPER_OUT               0x2E
#define HID_LED_PAPER_JAM               0x2F
#define HID_LED_REMOTE                  0x30
#define HID_LED_FORWARD                 0x31
#define HID_LED_REVERSE                 0x32
#define HID_LED_STOP                    0x33
#define HID_LED_REWIND                  0x34
#define HID_LED_FAST_FORWARD            0x35
#define HID_LED_PLAY                    0x36
#define HID_LED_PAUSE                   0x37
#define HID_LED_RECORD                  0x38
#define HID_LED_ERROR                   0x39
#define HID_LED_USAGE_SELECTED_IND      0x3A
#define HID_LED_USAGE_INUSE_IND         0x3B
#define HID_LED_USAGE_MULTIMODE_IND     0x3C
#define HID_LED_INDICATOR_ON            0x3D
#define HID_LED_INDICATOR_FLASH         0x3E
#define HID_LED_INDICATOR_SLOW_BLINK    0x3F
#define HID_LED_INDICATOR_FAST_BLINK    0x40
#define HID_LED_INDICATOR_OFF           0x41
#define HID_LED_FLASH_ON_TIME           0x42
#define HID_LED_SLOW_BLINK_ON_TIME      0x43
#define HID_LED_SLOW_BLINK_OFF_TIME     0x44
#define HID_LED_FAST_BLINK_ON_TIME      0x45
#define HID_LED_FAST_BLINK_OFF_TIME     0x46
#define HID_LED_USAGE_INDICATOR_COLOR   0x47
#define HID_LED_INDICATOR_RED           0x48
#define HID_LED_INDICATOR_GREEN         0x49
#define HID_LED_INDICATOR_AMBER         0x4A
#define HID_LED_GENERIC_INDICATOR       0x4B
#define HID_LED_SYSTEM_SUSPEND          0x4C
#define HID_LED_EXT_POWER_CONNECTED     0x4D
/** @} */
#endif

