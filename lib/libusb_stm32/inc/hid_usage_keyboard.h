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

#ifndef _USB_HID_USAGE_KEYBOARD_H_
#define _USB_HID_USAGE_KEYBOARD_H_

/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_KEYBOARD HID Usages for Keyboard
 * \brief Contains USB HID Usages definitions for Keyboard/Keypad Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_KEYBOARD               0x07

#define HID_KEYBOARD_ERR_ROLL_OVER      0x01
#define HID_KEYBOARD_POST_FAIL          0x02
#define HID_KEYBOARD_ERR_UNDEFINED      0x03
#define HID_KEYBOARD_A                  0x04
#define HID_KEYBOARD_B                  0x05
#define HID_KEYBOARD_C                  0x06
#define HID_KEYBOARD_D                  0x07
#define HID_KEYBOARD_E                  0x08
#define HID_KEYBOARD_F                  0x09
#define HID_KEYBOARD_G                  0x0A
#define HID_KEYBOARD_H                  0x0B
#define HID_KEYBOARD_I                  0x0C
#define HID_KEYBOARD_J                  0x0D
#define HID_KEYBOARD_K                  0x0E
#define HID_KEYBOARD_L                  0x0F
#define HID_KEYBOARD_M                  0x10
#define HID_KEYBOARD_N                  0x11
#define HID_KEYBOARD_O                  0x12
#define HID_KEYBOARD_P                  0x13
#define HID_KEYBOARD_Q                  0x14
#define HID_KEYBOARD_R                  0x15
#define HID_KEYBOARD_S                  0x16
#define HID_KEYBOARD_T                  0x17
#define HID_KEYBOARD_U                  0x18
#define HID_KEYBOARD_V                  0x19
#define HID_KEYBOARD_W                  0x1A
#define HID_KEYBOARD_X                  0x1B
#define HID_KEYBOARD_Y                  0x1C
#define HID_KEYBOARD_Z                  0x1D
#define HID_KEYBOARD_1                  0x1E
#define HID_KEYBOARD_2                  0x1F
#define HID_KEYBOARD_3                  0x20
#define HID_KEYBOARD_4                  0x21
#define HID_KEYBOARD_5                  0x22
#define HID_KEYBOARD_6                  0x23
#define HID_KEYBOARD_7                  0x24
#define HID_KEYBOARD_8                  0x25
#define HID_KEYBOARD_9                  0x26
#define HID_KEYBOARD_0                  0x27
#define HID_KEYBOARD_RETURN             0x28
#define HID_KEYBOARD_ESCAPE             0x29
#define HID_KEYBOARD_DELETE             0x2A
#define HID_KEYBOARD_TAB                0x2B
#define HID_KEYBOARD_SPACEBAR           0x2C
#define HID_KEYBOARD_MINUS              0x2D
#define HID_KEYBOARD_EQUAL_SIGN         0x2E
#define HID_KEYBOARD_OPEN_BRACKET       0x2F
#define HID_KEYBOARD_CLOSE_BRACKET      0x30
#define HID_KEYBOARD_BACKSLASH          0x31
#define HID_KEYBOARD_NONUS_HASH         0x32
#define HID_KEYBOARD_SEMICOLON          0x33
#define HID_KEYBOARD_APOSTROPHE         0x34
#define HID_KEYBOARD_GRAVE_ACCENT       0x35
#define HID_KEYBOARD_COLON              0x36
#define HID_KEYBOARD_DOT                0x37
#define HID_KEYBOARD_SLASH              0x38
#define HID_KEYBOARD_CAPS_LOCK          0x39
#define HID_KEYBOARD_F1                 0x3A
#define HID_KEYBOARD_F2                 0x3B
#define HID_KEYBOARD_F3                 0x3C
#define HID_KEYBOARD_F4                 0x3D
#define HID_KEYBOARD_F5                 0x3E
#define HID_KEYBOARD_F6                 0x3F
#define HID_KEYBOARD_F7                 0x40
#define HID_KEYBOARD_F8                 0x41
#define HID_KEYBOARD_F9                 0x42
#define HID_KEYBOARD_F10                0x43
#define HID_KEYBOARD_F11                0x44
#define HID_KEYBOARD_F12                0x45
#define HID_KEYBOARD_PRINT_SCREEN       0x46
#define HID_KEYBOARD_SCROLL_LOCK        0x47
#define HID_KEYBOARD_PAUSE              0x48
#define HID_KEYBOARD_INSERT             0x49
#define HID_KEYBOARD_HOME               0x4A
#define HID_KEYBOARD_PAGE_UP            0x4B
#define HID_KEYBOARD_DELETE_FORWARD     0x4C
#define HID_KEYBOARD_END                0x4D
#define HID_KEYBOARD_PAGE_DOWN          0x4E
#define HID_KEYBOARD_RIGHT_ARROW        0x4F
#define HID_KEYBOARD_LEFT_ARROW         0x50
#define HID_KEYBOARD_DOWN_ARROW         0x51
#define HID_KEYBOARD_UP_ARROW           0x52
#define HID_KEYPAD_NUMLOCK              0x53
#define HID_KEYPAD_SLASH                0x54
#define HID_KEYPAD_ASTERISK             0x55
#define HID_KEYPAD_MINUS                0x56
#define HID_KEYPAD_PLUS                 0x57
#define HID_KEYPAD_ENTER                0x58
#define HID_KEYPAD_1                    0x59
#define HID_KEYPAD_2                    0x5A
#define HID_KEYPAD_3                    0x5B
#define HID_KEYPAD_4                    0x5C
#define HID_KEYPAD_5                    0x5D
#define HID_KEYPAD_6                    0x5E
#define HID_KEYPAD_7                    0x5F
#define HID_KEYPAD_8                    0x60
#define HID_KEYPAD_9                    0x61
#define HID_KEYPAD_0                    0x62
#define HID_KEYPAD_DOT                  0x63
#define HID_KEYBOARD_NONUS_BACKSLASH    0x64
#define HID_KEYBOARD_APPLICATION        0x65
#define HID_KEYBOARD_POWER              0x66
#define HID_KEYPAD_EQUAL                0x67
#define HID_KEYBOARD_F13                0x68
#define HID_KEYBOARD_F14                0x69
#define HID_KEYBOARD_F15                0x6A
#define HID_KEYBOARD_F16                0x6B
#define HID_KEYBOARD_F17                0x6C
#define HID_KEYBOARD_F18                0x6D
#define HID_KEYBOARD_F19                0x6E
#define HID_KEYBOARD_F20                0x6F
#define HID_KEYBOARD_F21                0x70
#define HID_KEYBOARD_F22                0x71
#define HID_KEYBOARD_F23                0x72
#define HID_KEYBOARD_F24                0x73
#define HID_KEYBOARD_EXECUTE            0x74
#define HID_KEYBOARD_HELP               0x75
#define HID_KEYBOARD_MENU               0x76
#define HID_KEYBOARD_SELECT             0x77
#define HID_KEYBOARD_STOP               0x78
#define HID_KEYBOARD_AGAIN              0x79
#define HID_KEYBOARD_UNDO               0x7A
#define HID_KEYBOARD_CUT                0x7B
#define HID_KEYBOARD_COPY               0x7C
#define HID_KEYBOARD_PASTE              0x7D
#define HID_KEYBOARD_FIND               0x7E
#define HID_KEYBOARD_MUTE               0x7F
#define HID_KEYBOARD_VOLUME_UP          0x80
#define HID_KEYBOARD_VOLUME_DOWN        0x81
#define HID_KEYBOARD_LOCK_CAPS_LOCK     0x82
#define HID_KEYBOARD_LOCK_NUM_LOCK      0x83
#define HID_KEYBOARD_LOCK_SCROLL_LOCK   0x84
#define HID_KEYPAD_COMMA                0x85
#define HID_KEYPAD_EQUAL_SIGN           0x86
#define HID_KEYBOARD_INTERNATIONAL_1    0x87
#define HID_KEYBOARD_INTERNATIONAL_2    0x88
#define HID_KEYBOARD_INTERNATIONAL_3    0x89
#define HID_KEYBOARD_INTERNATIONAL_4    0x8A
#define HID_KEYBOARD_INTERNATIONAL_5    0x8B
#define HID_KEYBOARD_INTERNATIONAL_6    0x8C
#define HID_KEYBOARD_INTERNATIONAL_7    0x8D
#define HID_KEYBOARD_INTERNATIONAL_8    0x8E
#define HID_KEYBOARD_INTERNATIONAL_9    0x8F
#define HID_KEYBOARD_LANG_1             0x90
#define HID_KEYBOARD_LANG_2             0x91
#define HID_KEYBOARD_LANG_3             0x92
#define HID_KEYBOARD_LANG_4             0x93
#define HID_KEYBOARD_LANG_5             0x94
#define HID_KEYBOARD_LANG_6             0x95
#define HID_KEYBOARD_LANG_7             0x96
#define HID_KEYBOARD_LANG_8             0x97
#define HID_KEYBOARD_LANG_9             0x98
#define HID_KEYBOARD_ALTERNATE_ERASE    0x99
#define HID_KEYBOARD_SYSREQ             0x9A
#define HID_KEYBOARD_CANCEL             0x9B
#define HID_KEYBOARD_CLEAR              0x9C
#define HID_KEYBOARD_PRIOR              0x9D
#define HID_KEYBOARD_RETURN_1           0x9E
#define HID_KEYBOARD_SEPARATOR          0x9F
#define HID_KEYBOARD_OUT                0xA0
#define HID_KEYBOARD_OPER               0xA1
#define HID_KEYBOARD_CLEAR_AGAIN        0xA2
#define HID_KEYBOARD_CRSEL_PROPS        0xA3
#define HID_KEYBOARD_EXSEL              0xA4
#define HID_KEYPAD_00                   0xB0
#define HID_KEYPAD_000                  0xB1
#define HID_KEYPAD_OPEN_PARENTHESIS     0xB6
#define HID_KEYPAD_CLOSE_PARENTHESIS    0xB7
#define HID_KEYPAD_OPEN_BRACE           0xB8
#define HID_KEYPAD_CLOSE_BRACE          0xB9
#define HID_KEYPAD_TAB                  0xBA
#define HID_KEYPAD_BACKSPACE            0xBB
#define HID_KEYPAD_A                    0xBC
#define HID_KEYPAD_B                    0xBD
#define HID_KEYPAD_C                    0xBE
#define HID_KEYPAD_D                    0xBF
#define HID_KEYPAD_E                    0xC0
#define HID_KEYPAD_F                    0xC1
#define HID_KEYPAD_XOR                  0xC2
#define HID_KEYPAD_CARET                0xC3
#define HID_KEYPAD_PERCENT              0xC4
#define HID_KEYPAD_LESS_THEN            0xC5
#define HID_KEYPAD_GREATER_THEN         0xC6
#define HID_KEYPAD_AMPERSAND            0xC7
#define HID_KEYPAD_DOUBLE_AMPERSAND     0xC8
#define HID_KEYPAD_PIPE                 0xC9
#define HID_KEYPAD_DOUBLE_PIPE          0xCA
#define HID_KEYPAD_COLON                0xCB
#define HID_KEYPAD_HASH                 0xCC
#define HID_KEYPAD_SPACE                0xCD
#define HID_KEYPAD_AT                   0xCE
#define HID_KEYPAD_BANG                 0xCF
#define HID_KEYPAD_MEM_STORE            0xD0
#define HID_KEYPAD_MEM_RECALL           0xD1
#define HID_KEYPAD_MEM_CLEAR            0xD2
#define HID_KEYPAD_MEM_ADD              0xD3
#define HID_KEYPAD_MEM_SUBTRACT         0xD4
#define HID_KEYPAD_MEM_MULTIPLY         0xD5
#define HID_KEYPAD_MEM_DIVIDE           0xD6
#define HID_KEYPAD_PLUS_MINUS           0xD7
#define HID_KEYPAD_CLEAR                0xD8
#define HID_KEYPAD_CLEAR_ENTRY          0xD9
#define HID_KEYPAD_BINARY               0xDA
#define HID_KEYPAD_OCTAL                0xDB
#define HID_KEYPAD_DECIMAL              0xDC
#define HID_KEYPAD_HEXADECIMAL          0xDD
#define HID_KEYBOARD_L_CTRL             0xE0
#define HID_KEYBOARD_L_SHIFT            0xE1
#define HID_KEYBOARD_L_ALT              0xE2
#define HID_KEYBOARD_L_GUI              0xE3
#define HID_KEYBOARD_R_CTRL             0xE4
#define HID_KEYBOARD_R_SHIFT            0xE5
#define HID_KEYBOARD_R_ALT              0xE6
#define HID_KEYBOARD_R_GUI              0xE7

/** @}  */

#endif

