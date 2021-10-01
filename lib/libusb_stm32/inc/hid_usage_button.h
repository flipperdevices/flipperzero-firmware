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

#ifndef _USB_HID_USAGE_BUTTON_H_
#define _USB_HID_USAGE_BUTTON_H_
#ifdef __cplusplus
    extern "C" {
#endif


/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_BUTTON HID Usage Tables for Button
 * \brief Contains USB HID Usages definitions for Button Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_BUTTON           0x09    /**<\brief HID usage page for Buttons */

#define HID_BUTTON_NO_PRESSED           0x00    /**<\brief No button pressed */
#define HID_BUTTON_1                    0x01    /**<\brief Button 1 pressed */
#define HID_BUTTON_2                    0x02    /**<\brief Button 2 pressed */
#define HID_BUTTON_3                    0x03    /**<\brief Button 3 pressed */
#define HID_BUTTON_4                    0x04    /**<\brief Button 4 pressed */
#define HID_BUTTON_5                    0x05    /**<\brief Button 5 pressed */

/** @}  */

#ifdef __cplusplus
    }
#endif

#endif

