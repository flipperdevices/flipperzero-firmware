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

#ifndef _USB_DFU_H_
#define _USB_DFU_H_

#if defined(__cplusplus)
    extern "C" {
#endif

/**\addtogroup USB_MODULE_DFU USB DFU class
 * \brief This module contains USB Device Firmware Upgrade class definitions.
 * \details This module based on
 * + [USB Device Firmware Upgrade Specification, Revision 1.1]
 * (https://www.usb.org/sites/default/files/DFU_1.1.pdf)
 * @{ */

/**\name USB DFU class subclass and protocol definitions
 * @{ */
#define USB_CLASS_DFU                   0xFE    /**<\brief USB DFU class.*/
#define USB_DFU_SUBCLASS_DFU            0x01    /**<\brief USB DFU subclass code.*/
#define USB_DFU_PROTO_RUNTIME           0x01    /**<\brief USB DFU runtime-mode protocol.*/
#define USB_DFU_PROTO_DFU               0x02    /**<\brief USB DFU DFU-mode protocol.*/
/** @{ */

/**\name USB DFU descriptor types */
#define USB_DTYPE_DFU_FUNCTIONAL        0x21    /**<\brief USB DFU functional descriptor type.*/

/**\name USB DFU class-specific requests
 * @{ */
#define USB_DFU_DETACH                  0x00    /**<\brief Initiates a detach-attach sequence on the
                                                 * bus when it sees this request. */
#define USB_DFU_DNLOAD                  0x01    /**<\brief Initiates firmware image downloading */
#define USB_DFU_UPLOAD                  0x02    /**<\brief This request is employed by the host to
                                                 * solicit firmware from the device.*/
#define USB_DFU_GETSTATUS               0x03    /**<\brief The host employs this request to facilitate
                                                 * synchronization with the device.*/
#define USB_DFU_CLRSTATUS               0x04    /**<\brief This request resets DFU machine state to
                                                 * DFU_IDLE.*/
#define USB_DFU_GETSTATE                0x05    /**<\brief This request solicits a report about the
                                                 * state of the device.*/
#define USB_DFU_ABORT                   0x06    /**<\brief This request enables the host to exit from
                                                 * certain states and return to the DFU_IDLE state.*/
/** @} */

/**\anchor USB_DFU_CAPAB
 * \name USB DFU capabilities
 * @{ */
#define USB_DFU_ATTR_WILL_DETACH        0x08    /**<\brief Device will perform a bus detach-attach
                                                 * sequence when it receives a DFU_DETACH request.*/
#define USB_DFU_ATTR_MANIF_TOL          0x04    /**<\brief Device is able to communicate via USB
                                                 * after Manifestation phase.*/
#define USB_DFU_ATTR_CAN_UPLOAD         0x02    /**<\brief Upload capable.*/
#define USB_DFU_ATTR_CAN_DNLOAD         0x01    /**<\brief Download capable.*/
/** @} */

/**\name USB DFU status codes
 * @{ */
#define USB_DFU_STATUS_OK               0x00    /**<\brief No error condition is present.*/
#define USB_DFU_STATUS_ERR_TARGET       0x01    /**<\brief File is not targeted for use by this device.*/
#define USB_DFU_STATUS_ERR_FILE         0x02    /**<\brief File is for this device but fails some
                                                 * vendor specific verification test.*/
#define USB_DFU_STATUS_ERR_WRITE        0x03    /**<\brief Device is unable to write memory.*/
#define USB_DFU_STATUS_ERR_ERASE        0x04    /**<\brief Memory erase function failed.*/
#define USB_DFU_STATUS_ERR_CHECK_ERASED 0x05    /**<\brief Memory erase check failed.*/
#define USB_DFU_STATUS_ERR_PROG         0x06    /**<\brief Program memory function failed.*/
#define USB_DFU_STATUS_ERR_VERIFY       0x07    /**<\brief Programmed memory failed verification.*/
#define USB_DFU_STATUS_ERR_ADDRESS      0x08    /**<\brief Cannot program memory due to received
                                                 * address that is out of range. */
#define USB_DFU_STATUS_ERR_NOTDONE      0x09    /**<\brief Received DFU_DNLOAD with wLength = 0, but
                                                 * device does not think it has all of the data yet.*/
#define USB_DFU_STATUS_ERR_FIRMWARE     0x0A    /**<\brief Device's firmware is corrupt.  It cannot
                                                 * return to run-time (non-DFU) operations.*/
#define USB_DFU_STATUS_ERR_VENDOR       0x0B    /**<\brief iString indicates a vendor-specific error.*/
#define USB_DFU_STATUS_ERR_USBR         0x0C    /**<\brief Device detected unexpected USB reset signaling.*/
#define USB_DFU_STATUS_ERR_POR          0x0D    /**<\brief Device detected unexpected power on reset. */
#define USB_DFU_STATUS_ERR_UNKNOWN      0x0E    /**<\brief Something went wrong, but the device does
                                                 * not know what it was.*/
#define USB_DFU_STATUS_ERR_STALLEDPKT   0x0F    /**<\brief Device stalled an unexpected request.*/
/** @} */

/**\name USB DFU state codes
 * @{ */
#define USB_DFU_STATE_APP_IDLE          0x00    /**<\brief Device is running its normal application.*/
#define USB_DFU_STATE_APP_DETACH        0x01    /**<\brief Device is running its normal application,
                                                 * has received the DFU_DETACH request, and is
                                                 * waiting for a USB reset. */
#define USB_DFU_STATE_DFU_IDLE          0x02    /**<\brief Device is operating in the DFU mode and
                                                 * is waiting for requests. */
#define USB_DFU_STATE_DFU_DNLOADSYNC    0x03    /**<\brief Device has received a block and is waiting
                                                 * for the host to solicit the status via DFU_GETSTATUS. */
#define USB_DFU_STATE_DFU_DNBUSY        0x04    /**<\brief Device is programming a control-write block
                                                 * into its nonvolatile memories. */
#define USB_DFU_STATE_DFU_DNLOADIDLE    0x05    /**<\brief Device is processing a download operation.
                                                 * Expecting DFU_DNLOAD requests. */
#define USB_DFU_STATE_DFU_MANIFESTSYNC  0x06    /**<\brief Device has received the final block of
                                                 * firmware from the host and is waiting for receipt
                                                 * of DFU_GETSTATUS to begin the Manifestation phase;
                                                 * or device has completed the Manifestation phase
                                                 * and is waiting for receipt of DFU_GETSTATUS.
                                                 * \note Devices that can enter this state after the
                                                 * Manifestation phase set bmAttributes bit
                                                 * bitManifestationTolerant to 1. */
#define USB_DFU_STATE_DFU_MANIFEST      0x07    /**<\brief Device is in the Manifestation phase.
                                                 * \note Not all devices will be able to respond to
                                                 * DFU_GETSTATUS when in this state.*/
#define USB_DFU_STATE_DFU_MANIFESTWR    0x08    /**<\brief Device has programmed its memories and is
                                                 * waiting for a USB reset or a power on reset.
                                                 * \note Devices that must enter this state clear
                                                 * bitManifestationTolerant to 0.*/
#define USB_DFU_STATE_DFU_UPLOADIDLE    0x09    /**<\brief The device is processing an upload operation.*/
#define USB_DFU_STATE_DFU_ERROR         0x0A    /**<\brief An error has occurred. */
/** @} */

/**\brief USB DFU functional descriptor */
struct usb_dfu_func_desc {
    uint8_t     bLength;            /**<\brief Descriptor length in bytes.*/
    uint8_t     bDescriptorType;    /**<\brief DFU functional descriptor type.*/
    uint8_t     bmAttributes;       /**<\brief USB DFU capabilities \ref USB_DFU_CAPAB*/
    uint16_t    wDetachTimeout;     /**<\brief USB DFU detach timeout in ms.*/
    uint16_t    wTransferSize;      /**<\brief USB DFU maximum transfer block size in bytes.*/
    uint16_t    bcdDFUVersion;      /**<\brief USB DFU version \ref VERSION_BCD utility macro.*/
}__attribute__((packed));

/**\brief Payload packet to response in DFU_GETSTATUS request */
struct usb_dfu_status {
    uint8_t     bStatus;            /**<\brief An indication of the status resulting from the
                                     * execution of the most recent request.*/
    uint8_t     bPollTimeout;       /**<\brief Minimum time (LSB) in ms, that the host should wait
                                     * before sending a subsequent DFU_GETSTATUS request.*/
    uint16_t    wPollTimeout;       /**<\brief Minimum time (MSB) in ms, that the host should wait
                                     * before sending a subsequent DFU_GETSTATUS request.*/
    uint8_t     bState;             /**<\brief An indication of the state that the device is going
                                     * to enter immediately following transmission of this response.*/
    uint8_t     iString;            /**<\brief Index of the status string descriptor.*/
};

/** @} */

#if defined(__cplusplus)
    }
#endif
#endif /* _USB_DFU_H_ */
