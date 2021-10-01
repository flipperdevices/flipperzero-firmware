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

#ifndef _USB_HID_USAGE_DEVICE_H_
#define _USB_HID_USAGE_DEVICE_H_

/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_DEVICE HID Usages for Device
 * \brief Contains USB HID Usages definitions for Generic Device Control Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */
#define HID_PAGE_DEVICE                0x06    /**<\brief Generic device control usage page.*/
#define HID_DEVICE_BATTERY_STRENGHT    0x20    /**<\brief DV Current battery status.*/
#define HID_DEVICE_WIRELESS_CHANNEL    0x21    /**<\brief DV Logical wireless channel.*/
#define HID_DEVICE_WIRELESS_ID         0x22    /**<\brief DV Unique wireless device ID.*/
#define HID_DEVICE_DISCO_WIRELESS_CTL  0x23    /**<\brief OSC Wirleless discover control.*/
#define HID_DEVICE_SECURITY_CHAR_ENT   0x24    /**<\brief OSC Code character entered.*/
#define HID_DEVICE_SECURITY_CHAR_ERA   0x25    /**<\brief OSC Code character erased.*/
#define HID_DEVICE_SCURITY_CODE_CLR    0x26    /**<\brief OSC Security code cleared.*/
/** @} */
#endif

