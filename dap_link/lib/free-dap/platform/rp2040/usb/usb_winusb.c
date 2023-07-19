// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "usb.h"
#include "usb_std.h"
#include "usb_winusb.h"
#include "usb_descriptors.h"

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
bool usb_winusb_handle_request(usb_request_t *request)
{
  int length = request->wLength;

  switch (USB_CMD_VALUE(request))
  {
    case USB_CMD(IN, DEVICE, VENDOR, WINUSB_VENDOR_CODE):
    {
      if (USB_WINUSB_DESCRIPTOR_INDEX == request->wIndex)
      {
        length = USB_LIMIT(length, sizeof(usb_msos_descriptor_set_t));
        usb_control_send((uint8_t *)&usb_msos_descriptor_set, length);
      }
      else
      {
        return false;
      }
    } break;

    default:
      return false;
  }

  return true;
}
