// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2016-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _USB_STD_H_
#define _USB_STD_H_

/*- Includes ----------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

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
  USB_ATTRIBUTE_REMOTE_WAKEUP = 0x20,
  USB_ATTRIBUTE_SELF_POWERED  = 0x40,
  USB_ATTRIBUTE_BUS_POWERED   = 0x80,
};

enum
{
  USB_DEVICE_CAPABILITY_WIRELESS_USB               = 1,
  USB_DEVICE_CAPABILITY_USB_2_0_EXTENSION          = 2,
  USB_DEVICE_CAPABILITY_SUPERSPEED_USB             = 3,
  USB_DEVICE_CAPABILITY_CONTAINER_ID               = 4,
  USB_DEVICE_CAPABILITY_PLATFORM                   = 5,
  USB_DEVICE_CAPABILITY_POWER_DELIVERY             = 6,
  USB_DEVICE_CAPABILITY_BATTERY_INFO               = 7,
  USB_DEVICE_CAPABILITY_PD_CONSUMER_PORT           = 8,
  USB_DEVICE_CAPABILITY_PD_PROVIDER_PORT           = 9,
  USB_DEVICE_CAPABILITY_SUPERSPEED_PLUS            = 10,
  USB_DEVICE_CAPABILITY_PRECISION_TIME_MEASUREMENT = 11,
  USB_DEVICE_CAPABILITY_WIRELESS_USB_EXT           = 12,
};

enum
{
  USB_DEVICE_CLASS_MISCELLANEOUS   = 0xef,
  USB_DEVICE_CLASS_VENDOR_SPECIFIC = 0xff,
};

enum
{
  USB_DEVICE_SUBCLASS_COMMON = 0x02,
};

enum
{
  USB_DEVICE_PROTOCOL_INTERFACE_ASSOCIATION = 0x01,
};

#define USB_CTRL_EP_SIZE       64
#define USB_LANGID_ENGLISH     0x0409 // English (United States)
#define USB_MAX_POWER(ma)      ((ma) / 2)

#define USB_PACK               __attribute__((packed))
#define USB_LIMIT(a, b)        (((int)(a) > (int)(b)) ? (int)(b) : (int)(a))
#define USB_ARRAY_SIZE(x)      ((int)(sizeof(x) / sizeof(0[x])))

#define USB_CMD_VALUE(req)     (((req)->bRequest << 8) | (req)->bmRequestType)

#define USB_CMD(dir, rcpt, type, cmd) \
    ((USB_##cmd << 8) | (USB_##dir##_TRANSFER << 7) | \
     (USB_##type##_REQUEST << 5) | (USB_##rcpt##_RECIPIENT << 0))

/*- Types -------------------------------------------------------------------*/
typedef struct USB_PACK
{
  uint8_t   bmRequestType;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
} usb_request_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
} usb_descriptor_header_t;

typedef struct USB_PACK
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

typedef struct USB_PACK
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

typedef struct USB_PACK
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

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bEndpointAddress;
  uint8_t   bmAttributes;
  uint16_t  wMaxPacketSize;
  uint8_t   bInterval;
} usb_endpoint_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wLANGID;
} usb_string_descriptor_zero_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bString;
} usb_string_descriptor_t;

typedef struct USB_PACK
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

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wTotalLength;
  uint8_t   bNumDeviceCaps;
} usb_binary_object_store_descriptor_t;

typedef bool (*usb_class_handler_t)(usb_request_t *request);

/*- Prototypes --------------------------------------------------------------*/
void usb_init(void);
void usb_set_send_callback(int ep, void (*callback)(void));
void usb_set_recv_callback(int ep, void (*callback)(int size));
bool usb_handle_standard_request(usb_request_t *request);
void usb_send_callback(int ep);
void usb_recv_callback(int ep, int size);

#endif // _USB_STD_H_
