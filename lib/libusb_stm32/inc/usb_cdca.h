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

/**\ingroup USB_CDC
 * \addtogroup USB_CDC_ATM USB CDC ATM subclass
 * \brief USB CDC ATM subclass definitions
 * \details This module based on "Universal Serial Bus Communications Class Subclass Specification
 * for Asynchronous Transfer Mode Devices" Revision 1.2
 * @{ */

#ifndef _USB_CDC_ATM_H_
#define _USB_CDC_ATM_H_

#ifdef __cplusplus
    extern "C" {
#endif


/**\name Communications Class Subclass Codes
 * @{ */
#define USB_CDC_SUBCLASS_ATM                0x07 /**<\brief ATM Networking Control Model */
 /* @} */


/**\name CDC ATM subclass specific Functional Descriptors codes
 * @{ */
#define USB_DTYPE_CDC_ATM                   0x10 /**<\brief ATM Networking Functional Descriptor */
/** @} */

/**\name CDC ATM subclass specific requests
 * @{ */
#define USB_CDC_SET_ATM_DATA_FORMAT         0x50 /**<\brief Chooses which ATM data format will be
                                                  * exchanged between the host and the ATM Networking
                                                  * device.*/
#define USB_CDC_GET_ATM_DEVICE_STATISTICS   0x51 /**<\brief Retrieves global statistics from the ATM
                                                  * Networking device.*/
#define USB_CDC_SET_ATM_DEFAULT_VC          0x52 /**<\brief Pre-selects the VPI/VCI value for subsequent
                                                  * GET_ATM_DEVICE_STATISTICS requests.*/
#define USB_CDC_GET_ATM_VC_STATISTICS       0x53 /**<\brief Retrieves statistics from the ATM Networking
                                                  * device for a particular VPI/VCI.*/
/** @} */

/**\name ATM Device Statistics Feature Selector Codes
 * @{ */
#define ATM_STAT_US_CELLS_SENT              0x01 /**<\brief The number of cells that have been sent
                                                  * upstream to the WAN link by the ATM layer. */
#define ATM_STAT_DS_CELLS_RECEIVED          0x02 /**<\brief The number of cells that have been received
                                                  * downstream from the WAN link by the ATM layer */
#define ATM_STAT_DS_CELLS_USB_CONGESTION    0x03 /**<\brief The number of cells that have been received
                                                  * downstream from the WAN link by the ATM layer and
                                                  * discarded due to congestion on the USB link.*/
#define ATM_STAT_DS_CELLS_AAL5_CRC_ERROR    0x04 /**<\brief The number of cells that have been received
                                                  * downstream from the WAN link by the ATM layer and
                                                  * discarded due to AAL5 CRC errors.*/
#define ATM_STAT_DS_CELLS_HEC_ERROR         0x05 /**<\brief The number of cells that have been received
                                                  * downstream from the WAN link and discarded due to
                                                  * HEC errors in the cell header.*/
#define ATM_STAT_DS_CELLS_HEC_ERROR_CORRT   0x06 /**<\brief The number of cells that have been received
                                                  * downstream from the WAN link and have been detected
                                                  * with HEC errors in the cell  header and successfully
                                                  * corrected. */
/** @} */

/**\brief ATM Networking Functional Descriptor */
struct usb_cdc_atm_desc {
    uint8_t     bFunctionLength;        /**<\brief Size of this functional descriptor, in bytes.*/
    uint8_t     bDescriptorType;        /**<\brief CS_INTERFACE descriptor type.*/
    uint8_t     bDescriptorSubType;     /**<\brief ATM Networking Functional Descriptor subtype.*/
    uint8_t     iEndSystemIdentifier;   /**<\brief The string descriptor holds the End System Identifier. */
    uint8_t     bmDataCapabilities;     /**<\brief The ATM data types the device supports.*/
    uint8_t     bmATMDeviceStatistics;  /**<\brief Indicates which optional statistics functions the
                                         * device collects. */
    uint16_t    wType2MaxSegmentSize;   /**<\brief The maximum segment size that the Type 2 device
                                         * is capable of supporting */
    uint16_t    wType3MaxSegmentSize;   /**<\brief The maximum segment size that the Type 3 device
                                         * is capable of supporting */
    uint16_t    wMaxVC;                 /**<\brief The maximum number of simultaneous virtual circuits
                                         * the device is capable of supporting (Type 3 only) */
} __attribute__((packed));

/** @} */

#ifdef __cplusplus
    }
#endif

#endif /* _USB_CDC_ATM_H_ */