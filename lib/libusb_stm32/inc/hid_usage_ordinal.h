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

#ifndef _USB_HID_USAGE_ORDINAL_H_
#define _USB_HID_USAGE_ORDINAL_H_
#ifdef __cplusplus
    extern "C" {
#endif


/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_ORDINAL HID Usage Tables for Ordinal
 * \brief Contains USB HID Usages definitions for Ordinal Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_ORDINAL                0x0A

#define HID_ORDINAL_INSTANCE_1          0x01
#define HID_ORDINAL_INSTANCE_2          0x02
#define HID_ORDINAL_INSTANCE_3          0x03
#define HID_ORDINAL_INSTANCE_4          0x04
#define HID_ORDINAL_INSTANCE_5          0x05

/** @}  */

#ifdef __cplusplus
    }
#endif

#endif

