// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "usb.h"
#include "usb_std.h"
#include "usb_hid.h"
#include "usb_descriptors.h"

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_hid_init(void)
{
  usb_set_send_callback(USB_HID_EP_SEND, usb_hid_send_callback);
  usb_set_recv_callback(USB_HID_EP_RECV, usb_hid_recv_callback);
}

//-----------------------------------------------------------------------------
void usb_hid_send(uint8_t *data, int size)
{
  usb_send(USB_HID_EP_SEND, data, size);
}

//-----------------------------------------------------------------------------
void usb_hid_recv(uint8_t *data, int size)
{
  usb_recv(USB_HID_EP_RECV, data, size);
}

//-----------------------------------------------------------------------------
bool usb_hid_handle_request(usb_request_t *request)
{
  int length = request->wLength;

  switch (USB_CMD_VALUE(request))
  {
    case USB_CMD(IN, INTERFACE, STANDARD, GET_DESCRIPTOR):
    {
      length = USB_LIMIT(length, sizeof(usb_hid_report_descriptor));

      usb_control_send((uint8_t *)usb_hid_report_descriptor, length);
    } break;

    default:
      return false;
  }

  return true;
}
