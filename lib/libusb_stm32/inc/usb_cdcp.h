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

/** \ingroup USB_CDC
 *  \addtogroup USB_CDC_PSTN USB CDC PSTN subclass
 *  \brief USB CDC PSTN subclass definitions
 *  \details This module based on "Universal Serial Bus Communications Class Subclass Specification
 * for PSTN Devices" Revision 1.2
 *  \details This module contains definitions for
 * + Direct Line Control Model
 * + Telephony Control Model
 * @{ */

#ifndef _USB_CDC_PSTN_H_
#define _USB_CDC_PSTN_H_

#ifdef __cplusplus
    extern "C" {
#endif


/**\name Communications Class Subclass Codes
 * @{ */
#define USB_CDC_SUBCLASS_DLC            0x01 /**<\brief Direct Line Control Model */
#define USB_CDC_SUBCLASS_TEL            0x03 /**<\brief Telephone Control Model */
 /* @} */

/**\name CDC PSTN subclass specific Functional Descriptors codes
 * @{ */
#define USB_DTYPE_CDC_LINE_MANAGEMENT   0x03 /**<\brief Direct Line Management Functional Descriptor.*/
#define USB_DTYPE_CDC_TEL_RING          0x04 /**<\brief Telephone Ringer Functional Descriptor. */
#define USB_DTYPE_CDC_TEL_CALL          0x05 /**<\brief Telephone Call and Line State Reporting
                                              * Capabilities Functional Descriptor.*/
#define USB_DTYPE_CDC_TEL_OPMODE        0x08 /**<\brief Telephone Operational Modes Functional Descriptor */
/** @} */

/**\name CDC PSTN subclass specific requests
 * @{ */
#define USB_CDC_SET_AUX_LINE_STATE      0x10 /**<\brief Used to connect or disconnect a secondary
                                              *jack to POTS circuit or CODEC, depending on hook state.*/
#define USB_CDC_SET_HOOK_STATE          0x11 /**<\brief Used to set the necessary POTS line relay code
                                              * for on-hook, off-hook, and caller ID states.*/
#define USB_CDC_PULSE_SETU              0x12 /**<\brief Used to prepare for a pulse-dialing cycle.*/
#define USB_CDC_SEND_PULSE              0x13 /**<\brief Used to generate a specified number of
                                              * make/break pulse cycles.*/
#define USB_CDC_SET_PULSE_TIME          0x14 /**<\brief Sets the timing of the make and break periods
                                              * for pulse dialing.*/
#define USB_CDC_RING_AUX_JACK           0x15 /**<\brief Used to generate a ring signal on a secondary
                                              * phone jack.*/
#define USB_CDC_SET_RINGER_PARMS        0x30 /**<\brief Configures the ringer for the communication
                                              * device.*/
#define USB_CDC_GET_RINGER_PARMS        0x31 /**<\brief Returns the ringer capabilities of the device
                                              * and the current status of the device’s ringer.*/
#define USB_CDC_SET_OPERATION_PARMS     0x32 /**<\brief Sets the operational mode for the device,
                                              * between a simple mode, standalone mode and a host
                                              * centric mode. */
#define USB_CDC_GET_OPERATION_PARMS     0x33 /**<\brief Gets the current operational mode for the device.*/
#define USB_CDC_SET_LINE_PARMS          0x34 /**<\brief Used to change the state of the line.*/
#define USB_CDC_GET_LINE_PARMS          0x35 /**<\brief Used to report the state of the line.*/
#define USB_CDC_DIAL_DIGITS             0x36 /**<\brief Dials the DTMF digits over the specified line.*/
/** @} */

/**\name CDC PSTN subclass specific notifications
 * @{ */
#define USB_CDC_NTF_AUX_JACK_HOOK_STATE	0x08  /**<\brief Indicates the loop has changed on the
                                               * auxiliary phone interface. */
#define USB_CDC_NTF_RING_DETECT			0x09  /**<\brief Indicates ring voltage on the POTS line
                                               * interface.*/
#define USB_CDC_NTF_CALL_STATE_CHANGE	0x28  /**<\brief Identifies that a change has occurred to the
                                               * state of a call on the line corresponding to the
                                               * interface or union for the line.*/
#define USB_CDC_NTF_LINE_STATE_CHANGE	0x29  /**<\brief identifies that a change has occurred to the
                                               * state of the line corresponding to theinterface or
                                               * master interface of a union.*/
/** @} */


/**\anchor USB_CDC_DLMGMNTCAP
 * \name USB CDC Direct Line Management capabilities
 * @{ */
#define USB_CDC_DLM_PULSE               0x01 /**<\brief Supports the request combination of Pulse_Setup,
                                              * Send_Pulse, and Set_Pulse_Time. */
#define USB_CDC_DLM_AUX                 0x02 /**<\brief Supports the request combination of Set_Aux_Line_State,
                                              * Ring_Aux_Jack, and notification Aux_Jack_Hook_State. */
#define USB_CDC_DLM_XTRAPULSE           0x04 /**<\brief Device requires extra Pulse_Setup request during
                                              * pulse dialing sequence to disengage holding circuit.*/
/** @} */

/**\anchor USB_CDC_TOMCAP
 * \name USB CDC Telephone Operational Modes capabilities
 * @{ */
#define USB_CDC_TOM_SIMPLE              0x01 /**<\brief Supports Simple mode. */
#define USB_CDC_TOM_STANDALONE          0x02 /**<\brief Supports Standalone mode. */
#define USB_CDC_TOM_CENTRIC             0x04 /**<\brief Supports Computer Centric mode. */
 /** @} */

/**\anchor USB_CDC_TCSCAP
 * \name USB CDC Telephone Call State Reporting capabilities
 * @{ */
#define USB_CDC_TCS_DIALTONE            0x01 /**<\brief Reports interrupted dialtone in addition to
                                              * normal dialtone.*/
#define USB_CDC_TCS_STATE               0x02 /**<\brief Reports ringback, busy, and fast busy states.*/
#define USB_CDC_TCS_CALLERID            0x04 /**<\brief Reports caller ID information. */
#define USB_CDC_TCS_RINGING             0x08 /**<\brief Reports incoming distinctive ringing patterns.*/
#define USB_CDC_TCS_DTMF                0x10 /**<\brief Can report DTMF digits input remotely over
                                              * the telephone line.*/
#define USB_CDC_TCS_NOTIFY              0x20 /**<\brief Does support line state change notification.*/
 /** @} */


/** \brief Direct Line Management Functional Descriptor
 * \details The Direct Line Management functional descriptor describes the commands supported by the
 * Communication Class interface, as defined in Section 3.6.1, with the SubClass code of Direct Line
 * Control Model. It can only occur within the class-specific portion of an Interface descriptor.*/
struct usb_cdc_dlm_desc {
    uint8_t     bFunctionLength;    /**<\brief Size of this functional descriptor, in bytes. */
    uint8_t     bDescriptorType;    /**<\brief CS_INTERFACE descriptor type. */
    uint8_t     bDescriptorSubType; /**<\brief Direct Line Management Functional Descriptor. */
    uint8_t     bmCapabilities;     /**<\brief The line management capabilities that this
                                     * configuration supports */
} __attribute__ ((packed));

/** \brief Telephone Ringer Functional Descriptor
 * \details The Telephone Ringer functional descriptor describes the ringer capabilities supported
 * by the Communication Class interface, as defined in Section 3.6.3.1, with the SubClass code of
 * Telephone Control. It can only occur within the class-specific portion of an Interface descriptor.*/
struct usb_cdc_tring_desc {
    uint8_t     bFunctionLength;    /**<\brief Size of this functional descriptor, in bytes.*/
    uint8_t     bDescriptorType;    /**<\brief CS_INTERFACE descriptor type.*/
    uint8_t     bDescriptorSubType; /**<\brief Direct Line Management Functional Descriptor.*/
    uint8_t     bRingerVolSteps;    /**<\brief Number of discrete steps in volume supported by the ringer.*/
    uint8_t     bNumRingerPatterns; /**<\brief Number of ringer patterns supported.*/
} __attribute__ ((packed));

/** \brief Telephone Operational Modes Functional Descriptor
 * \details The Telephone Operational Modes functional descriptor describes the operational modes
 * supported by the Communication Class interface, as defined in Section 3.6.3.1, with the SubClass
 * code of Telephone Control. It can only occur within the class-specific portion of an Interface
 * descriptor. The modes supported are Simple, Standalone, and Computer Centric. See Section 6.2.18,
 *  “SetOperationParms” for a definition of the various operational modes and Table 53 for the
 * definition of the operational mode values.*/
struct usb_cdc_tom_desc {
    uint8_t     bFunctionLength;    /**<\brief Size of this functional descriptor, in bytes. */
    uint8_t     bDescriptorType;    /**<\brief CS_INTERFACE descriptor type. */
    uint8_t     bDescriptorSubType; /**<\brief Direct Line Management Functional Descriptor. */
    uint8_t     bmCapabilities;     /**<\brief The perational modes capabilities that this
                                     * configuration supports */
} __attribute__ ((packed));

/** \brief Telephone Call State Reporting Capabilities Descriptor
 * \details The Telephone Call and Line State Reporting Capabilities functional descriptor describes
 * the abilities of a telephone device to report optional call and line states.
 */
struct usb_cdc_tcs_desc {
    uint8_t     bFunctionLength;    /**<\brief Size of this functional descriptor, in bytes. */
    uint8_t     bDescriptorType;    /**<\brief CS_INTERFACE descriptor type. */
    uint8_t     bDescriptorSubType; /**<\brief Direct Line Management Functional Descriptor. */
    uint32_t    bmCapabilities;     /**<\brief The call state capabilities that this configuration
                                     * supports */
} __attribute__ ((packed));

/** @} */

#ifdef __cplusplus
    }
#endif
#endif /* _USB_CDC_PSTN_H_ */