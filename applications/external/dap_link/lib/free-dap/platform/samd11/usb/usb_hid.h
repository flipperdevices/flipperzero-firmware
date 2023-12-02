// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _USB_HID_H_
#define _USB_HID_H_

/*- Includes ----------------------------------------------------------------*/
#include "usb_std.h"

/*- Definitions -------------------------------------------------------------*/
enum
{
  USB_HID_DESCRIPTOR          = 0x21,
  USB_HID_REPORT_DESCRIPTOR   = 0x22,
  USB_HID_PHYSICAL_DESCRIPTOR = 0x23,
};

enum
{
  USB_HID_DEVICE_CLASS = 3,  // USB Human Interface Device Class
};

/*- Types -------------------------------------------------------------------*/
typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdHID;
  uint8_t   bCountryCode;
  uint8_t   bNumDescriptors;
  uint8_t   bDescriptorType1;
  uint16_t  wDescriptorLength;
} usb_hid_descriptor_t;

/*- Prototypes --------------------------------------------------------------*/
void usb_hid_init(void);
bool usb_hid_handle_request(usb_request_t *request);
void usb_hid_send(uint8_t *data, int size);
void usb_hid_recv(uint8_t *data, int size);

void usb_hid_send_callback(void);
void usb_hid_recv_callback(int size);

#endif // _USB_HID_H_
