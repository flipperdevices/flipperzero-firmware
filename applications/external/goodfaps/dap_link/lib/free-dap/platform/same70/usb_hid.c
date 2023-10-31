/*
 * Copyright (c) 2019, Alex Taradov <alex@taradov.com>
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

/*- Includes ----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdalign.h>
#include <string.h>
#include "utils.h"
#include "usb.h"
#include "usb_std.h"
#include "usb_hid.h"
#include "usb_descriptors.h"

/*- Prototypes --------------------------------------------------------------*/
static void usb_hid_ep_send_callback(int size);
static void usb_hid_ep_recv_callback(int size);

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_hid_init(void)
{
  usb_set_callback(USB_HID_EP_SEND, usb_hid_ep_send_callback);
  usb_set_callback(USB_HID_EP_RECV, usb_hid_ep_recv_callback);
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
static void usb_hid_ep_send_callback(int size)
{
  usb_hid_send_callback();
  (void)size;
}

//-----------------------------------------------------------------------------
static void usb_hid_ep_recv_callback(int size)
{
  usb_hid_recv_callback(size);
}

//-----------------------------------------------------------------------------
bool usb_hid_handle_request(usb_request_t *request)
{
  int length = request->wLength;

  switch ((request->bRequest << 8) | request->bmRequestType)
  {
    case USB_CMD(IN, INTERFACE, STANDARD, GET_DESCRIPTOR):
    {
      length = LIMIT(length, sizeof(usb_hid_report_descriptor));

      usb_control_send((uint8_t *)usb_hid_report_descriptor, length);
    } break;

    default:
      return false;
  }

  return true;
}

