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

#ifndef _USB_HID_USAGE_TELEPHONY_H_
#define _USB_HID_USAGE_TELEPHONY_H_
#ifdef __cplusplus
    extern "C" {
#endif


/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_TELEPHONY HID Usage Tables for Telephony
 * \brief Contains USB HID Usages definitions for Telephony Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_TELEPHONY              0x0B

#define HID_PHONE_PHONE                 0x01
#define HID_PHONE_ANSWERING_MACHINE     0x02
#define HID_PHONE_MESSAGE_CONTROLS      0x03
#define HID_PHONE_HANDSET               0x04
#define HID_PHONE_HEADSET               0x05
#define HID_PHONE_TELEPHONY_KEYPAD      0x06
#define HID_PHONE_PROGRAMMABLE_BUTTON   0x07
#define HID_PHONE_HOOK_SWITCH           0x20
#define HID_PHONE_FLASH                 0x21
#define HID_PHONE_FEATURE               0x22
#define HID_PHONE_HOLD                  0x23
#define HID_PHONE_REDIAL                0x24
#define HID_PHONE_TRANSFER              0x25
#define HID_PHONE_DROP                  0x26
#define HID_PHONE_PARK                  0x27
#define HID_PHONE_FORWARD_CALLS         0x28
#define HID_PHONE_ALTERNATE_FUNCTION    0x29
#define HID_PHONE_LINE                  0x2A
#define HID_PHONE_SPEAKERPHONE          0x2B
#define HID_PHONE_CONFERENCE            0x2C
#define HID_PHONE_RING_ENABLE           0x2D
#define HID_PHONE_RING_SELECT           0x2E
#define HID_PHONE_PHONE_MUTE            0x2F
#define HID_PHONE_CALLER_ID             0x30
#define HID_PHONE_SEND                  0x31
#define HID_PHONE_SPEED_DIAL            0x50
#define HID_PHONE_STORE_NUMBER          0x51
#define HID_PHONE_RECALL_NUMBER         0x52
#define HID_PHONE_PHONE_DIRECTORY       0x53
#define HID_PHONE_VOICE_MAIL            0x70
#define HID_PHONE_SCREEN_CALLS          0x71
#define HID_PHONE_DO_NOT_DISTURB        0x72
#define HID_PHONE_MESSAGE               0x73
#define HID_PHONE_ANSWER_ON_OFF         0x74
#define HID_PHONE_INSIDE_DIAL_TONE      0x90
#define HID_PHONE_OUTSIDE_DIAL_TONE     0x91
#define HID_PHONE_INSIDE_RING_TONE      0x92
#define HID_PHONE_OUTSIDE_RING_TONE     0x93
#define HID_PHONE_PRIORITY_RING_TONE    0x94
#define HID_PHONE_INSIDE_RINGBACK       0x95
#define HID_PHONE_PRIORITY_RINGBACK     0x96
#define HID_PHONE_LINE_BUSY_TONE        0x97
#define HID_PHONE_REORDER_TONE          0x98
#define HID_PHONE_CALL_WAITING_TONE     0x99
#define HID_PHONE_CONFIRMATION_TONE_1   0x9A
#define HID_PHONE_CONFIRMATION_TONE_2   0x9B
#define HID_PHONE_TONES_OFF             0x9C
#define HID_PHONE_OUTSIDE_RINGBACK      0x9D
#define HID_PHONE_RINGER                0x9E
#define HID_PHONE_KEY_0                 0xB0
#define HID_PHONE_KEY_1                 0xB1
#define HID_PHONE_KEY_2                 0xB2
#define HID_PHONE_KEY_3                 0xB3
#define HID_PHONE_KEY_4                 0xB4
#define HID_PHONE_KEY_5                 0xB5
#define HID_PHONE_KEY_6                 0xB6
#define HID_PHONE_KEY_7                 0xB7
#define HID_PHONE_KEY_8                 0xB8
#define HID_PHONE_KEY_9                 0xB9
#define HID_PHONE_KEY_STAR              0xBA
#define HID_PHONE_KEY_POUND             0xBB
#define HID_PHONE_KEY_A                 0xBC
#define HID_PHONE_KEY_B                 0xBD
#define HID_PHONE_KEY_C                 0xBE
#define HID_PHONE_KEY_D                 0xBF

/** @}  */

#ifdef __cplusplus
    }
#endif

#endif

