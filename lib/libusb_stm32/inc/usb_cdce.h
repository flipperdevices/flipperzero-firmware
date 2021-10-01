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
 * \addtogroup USB_CDC_ECM USB CDC ECM subclass
 * \brief USB CDC ECM subclass definitions
 * \details This module based on "Universal Serial Bus Communications Class Subclass Specification for
 * Ethernet Control Model Devices Revision 1.2"
 * @{ */

#ifndef _USB_CDC_ECM_H_
#define _USB_CDC_ECM_H_

#ifdef __cplusplus
    extern "C" {
#endif

/**\name Communications Class Subclass Codes
 * @{ */
#define USB_CDC_SUBCLASS_ETH                0x06 /**<\brief Ethernet Networking Control Model */
 /* @} */

/**\name CDC ECM subclass specific Functional Descriptors codes
 * @{ */
#define USB_DTYPE_CDC_ETHERNET              0x0F /**<\brief Ethernet Networking Functional Descriptor*/
/** @} */

/**\name CDC ECM subclass specific requests
 * @{ */
#define USB_CDC_SET_ETH_MULTICAST_FILTERS   0x40 /**<\brief  */
#define USB_CDC_SET_ETH_PM_PATTERN_FILTER   0x41 /**<\brief  */
#define USB_CDC_GET_ETH_PM_PATTERN_FILTER   0x42 /**<\brief  */
#define USB_CDC_SET_ETH_PACKET_FILTER       0x43 /**<\brief Sets device filter for running a network
                                                  * analyzer application on the host machine.*/
#define USB_CDC_GET_ETH_STATISTIC           0x44 /**<\brief Retrieves Ethernet device statistics such
                                                  * as frames transmitted, frames received, and bad
                                                  * frames received.*/
/** @} */

/**\name Ethernet Statistics Capabilities
 * @{ */
#define USB_ETH_XMIT_OK                     (1<<0)  /**<\brief Frames transmitted without errors.*/
#define USB_ETH_RCV_OK                      (1<<1)  /**<\brief Frames received without errors.*/
#define USB_ETH_XMIT_ERROR                  (1<<2)  /**<\brief Frames not transmitted, or transmitted
                                                     * with errors.*/
#define USB_ETH_RCV_ERROR                   (1<<3)  /**<\brief Frames received with errors that are
                                                     * not delivered to the USB host. */
#define USB_ETH_RCV_NO_BUFFER               (1<<4)  /**<\brief Frame missed, no buffers */
#define USB_ETH_DIRECTED_BYTES_XMIT         (1<<5)  /**<\brief Directed bytes transmitted without errors */
#define USB_ETH_DIRECTED_FRAMES_XMIT        (1<<6)  /**<\brief Directed frames transmitted without errors */
#define USB_ETH_MULTICAST_BYTES_XMIT        (1<<7)  /**<\brief Multicast bytes transmitted without errors */
#define USB_ETH_MULTICAST_FRAMES_XMIT       (1<<8)  /**<\brief Multicast frames transmitted without errors */
#define USB_ETH_BROADCAST_BYTES_XMIT        (1<<9)  /**<\brief Broadcast bytes transmitted without errors */
#define USB_ETH_BROADCAST_FRAMES_XMIT       (1<<10) /**<\brief Broadcast frames transmitted without errors */
#define USB_ETH_DIRECTED_BYTES_RCV          (1<<11) /**<\brief Directed bytes received without errors */
#define USB_ETH_DIRECTED_FRAMES_RCV         (1<<12) /**<\brief Directed frames received without errors */
#define USB_ETH_MULTICAST_BYTES_RCV         (1<<13) /**<\brief Multicast bytes received without errors */
#define USB_ETH_MULTICAST_FRAMES_RCV        (1<<14) /**<\brief Multicast frames received without errors */
#define USB_ETH_BROADCAST_BYTES_RCV         (1<<15) /**<\brief Broadcast bytes received without errors */
#define USB_ETH_BROADCAST_FRAMES_RCV        (1<<16) /**<\brief Broadcast frames received without errors */
#define USB_ETH_RCV_CRC_ERROR               (1<<17) /**<\brief Frames received with circular redundancy check
                                                     * (CRC) or frame check sequence (FCS) error */
#define USB_ETH_TRANSMIT_QUEUE_LENGTH       (1<<18) /**<\brief Length of transmit queue */
#define USB_ETH_RCV_ERROR_ALIGNMENT         (1<<19) /**<\brief Frames received with alignment error */
#define USB_ETH_XMIT_ONE_COLLISION          (1<<20) /**<\brief Frames transmitted with one collision */
#define USB_ETH_XMIT_MORE_COLLISIONS        (1<<21) /**<\brief Frames transmitted with more than one collision */
#define USB_ETH_XMIT_DEFERRED               (1<<22) /**<\brief Frames transmitted after deferral */
#define USB_ETH_XMIT_MAX_COLLISIONS         (1<<23) /**<\brief Frames not transmitted due to collisions */
#define USB_ETH_RCV_OVERRUN                 (1<<24) /**<\brief Frames not received due to overrun */
#define USB_ETH_XMIT_UNDERRUN               (1<<25) /**<\brief Frames not transmitted due to underrun */
#define USB_ETH_XMIT_HEARTBEAT_FAILURE      (1<<26) /**<\brief Frames transmitted with heartbeat failure */
#define USB_ETH_XMIT_TIMES_CRS_LOST         (1<<27) /**<\brief Times carrier sense signal lost during transmission */
#define USB_ETH_XMIT_LATE_COLLISIONS        (1<<28) /**<\brief Late collisions detected */
/** @} */


/**\brief Ethernet Networking Functional Descriptor
 * \details describes the operational modes supported by the
 * Communications Class interface, as defined in Section 3.4, with the SubClass code of Ethernet
 * Networking Control. It can only occur within the class-specific portion of an Interface descriptor.
 */
struct usb_cdc_ether_desc {
    uint8_t     bFunctionLength;        /**<\brief Size of this functional descriptor, in bytes.*/
    uint8_t     bDescriptorType;        /**<\brief CS_INTERFACE descriptor type.*/
    uint8_t     bDescriptorSubType;     /**<\brief Ethernet Networking Functional Descriptor.*/
    uint8_t     iMACAddress;            /**<\brief Index of string descriptor that holds the
                                         * 48bit Ethernet MAC.*/
    uint32_t    bmEthernetStatistics;   /**<\brief Indicates which Ethernet statistics functions
                                         * the device collects. */
    uint16_t    wMaxSegmentSize;        /**<\brief The maximum segment size that the Ethernet
                                         * device is capable of supporting. */
    uint16_t    wNumberMCFilters;       /**<\brief Contains the number of multicast filters that
                                         * can be configured by the host. */
    uint8_t     bNumberPowerFilters;    /**<\brief Contains the number of pattern filters that
                                         * are available for causing wake-up of the host. */
} __attribute__ ((packed));

/** @} */

#ifdef __cplusplus
    }
#endif

#endif /* _USB_CDC_ECM_H_ */