/*
 * Copyright (c) 2016-2017, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _USB_STD_H_
#define _USB_STD_H_

/*- Includes ----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

/*- Definitions -------------------------------------------------------------*/
enum
{
  USB_GET_STATUS        = 0,
  USB_CLEAR_FEATURE     = 1,
  USB_SET_FEATURE       = 3,
  USB_SET_ADDRESS       = 5,
  USB_GET_DESCRIPTOR    = 6,
  USB_SET_DESCRIPTOR    = 7,
  USB_GET_CONFIGURATION = 8,
  USB_SET_CONFIGURATION = 9,
  USB_GET_INTERFACE     = 10,
  USB_SET_INTERFACE     = 11,
  USB_SYNCH_FRAME       = 12,
};

enum
{
  USB_DEVICE_DESCRIPTOR                    = 1,
  USB_CONFIGURATION_DESCRIPTOR             = 2,
  USB_STRING_DESCRIPTOR                    = 3,
  USB_INTERFACE_DESCRIPTOR                 = 4,
  USB_ENDPOINT_DESCRIPTOR                  = 5,
  USB_DEVICE_QUALIFIER_DESCRIPTOR          = 6,
  USB_OTHER_SPEED_CONFIGURATION_DESCRIPTOR = 7,
  USB_INTERFACE_POWER_DESCRIPTOR           = 8,
  USB_OTG_DESCRIPTOR                       = 9,
  USB_DEBUG_DESCRIPTOR                     = 10,
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR     = 11,
  USB_BINARY_OBJECT_STORE_DESCRIPTOR       = 15,
  USB_DEVICE_CAPABILITY_DESCRIPTOR         = 16,
  USB_CS_INTERFACE_DESCRIPTOR              = 36,
};

enum
{
  USB_DEVICE_RECIPIENT     = 0,
  USB_INTERFACE_RECIPIENT  = 1,
  USB_ENDPOINT_RECIPIENT   = 2,
  USB_OTHER_RECIPIENT      = 3,
};

enum
{
  USB_STANDARD_REQUEST     = 0,
  USB_CLASS_REQUEST        = 1,
  USB_VENDOR_REQUEST       = 2,
};

enum
{
  USB_OUT_TRANSFER         = 0,
  USB_IN_TRANSFER          = 1,
};

enum
{
  USB_IN_ENDPOINT          = 0x80,
  USB_OUT_ENDPOINT         = 0x00,
  USB_INDEX_MASK           = 0x7f,
  USB_DIRECTION_MASK       = 0x80,
};

enum
{
  USB_CONTROL_ENDPOINT     = 0 << 0,
  USB_ISOCHRONOUS_ENDPOINT = 1 << 0,
  USB_BULK_ENDPOINT        = 2 << 0,
  USB_INTERRUPT_ENDPOINT   = 3 << 0,

  USB_NO_SYNCHRONIZATION   = 0 << 2,
  USB_ASYNCHRONOUS         = 1 << 2,
  USB_ADAPTIVE             = 2 << 2,
  USB_SYNCHRONOUS          = 3 << 2,

  USB_DATA_ENDPOINT        = 0 << 4,
  USB_FEEDBACK_ENDPOINT    = 1 << 4,
  USB_IMP_FB_DATA_ENDPOINT = 2 << 4,
};

enum
{
  USB_DEVICE_CLASS_MISCELLANEOUS = 0xef,
};

enum
{
  USB_DEVICE_SUBCLASS_COMMON = 0x02,
};

enum
{
  USB_DEVICE_PROTOCOL_INTERFACE_ASSOCIATION = 0x01,
};

#define USB_CMD(dir, rcpt, type, cmd) \
    ((USB_##cmd << 8) | (USB_##dir##_TRANSFER << 7) | \
     (USB_##type##_REQUEST << 5) | (USB_##rcpt##_RECIPIENT << 0))

/*- Types -------------------------------------------------------------------*/
typedef struct PACK
{
  uint8_t   bmRequestType;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
} usb_request_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
} usb_descriptor_header_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdUSB;
  uint8_t   bDeviceClass;
  uint8_t   bDeviceSubClass;
  uint8_t   bDeviceProtocol;
  uint8_t   bMaxPacketSize0;
  uint16_t  idVendor;
  uint16_t  idProduct;
  uint16_t  bcdDevice;
  uint8_t   iManufacturer;
  uint8_t   iProduct;
  uint8_t   iSerialNumber;
  uint8_t   bNumConfigurations;
} usb_device_descriptor_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wTotalLength;
  uint8_t   bNumInterfaces;
  uint8_t   bConfigurationValue;
  uint8_t   iConfiguration;
  uint8_t   bmAttributes;
  uint8_t   bMaxPower;
} usb_configuration_descriptor_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bInterfaceNumber;
  uint8_t   bAlternateSetting;
  uint8_t   bNumEndpoints;
  uint8_t   bInterfaceClass;
  uint8_t   bInterfaceSubClass;
  uint8_t   bInterfaceProtocol;
  uint8_t   iInterface;
} usb_interface_descriptor_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bEndpointAddress;
  uint8_t   bmAttributes;
  uint16_t  wMaxPacketSize;
  uint8_t   bInterval;
} usb_endpoint_descriptor_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wLANGID;
} usb_string_descriptor_zero_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bString;
} usb_string_descriptor_t;

typedef struct PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bFirstInterface;
  uint8_t   bInterfaceCount;
  uint8_t   bFunctionClass;
  uint8_t   bFunctionSubClass;
  uint8_t   bFunctionProtocol;
  uint8_t   iFunction;
} usb_interface_association_descriptor_t;

/*- Prototypes --------------------------------------------------------------*/
void usb_init(void);
void usb_set_callback(int ep, void (*callback)(int size));
bool usb_handle_standard_request(usb_request_t *request);
void usb_send_callback(int ep);
void usb_recv_callback(int ep, int size);

#endif // _USB_STD_H_

