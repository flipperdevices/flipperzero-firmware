/*
 * Copyright (c) 2016, Alex Taradov <alex@taradov.com>
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
#include "udc.h"
#include "usb.h"
#include "utils.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_CMD(dir, rcpt, type, cmd) \
    ((USB_##cmd << 8) | (USB_##dir##_TRANSFER << 7) | (USB_##type##_REQUEST << 5) | (USB_##rcpt##_RECIPIENT << 0))

/*- Types -------------------------------------------------------------------*/
typedef struct
{
  void         (*callback)(void);
} usb_endpoint_bank_t;

typedef struct
{
  usb_endpoint_bank_t out;
  usb_endpoint_bank_t in;
} usb_endpoint_t;

/*- Variables ---------------------------------------------------------------*/
static usb_endpoint_t usb_endpoints[UDC_EPT_NUM];
static int usb_config;

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_init(void)
{
  usb_config = 0;

  udc_init();
}

//-----------------------------------------------------------------------------
void usb_send(int ep, uint8_t *data, int size, void (*callback)(void))
{
  usb_endpoints[ep].in.callback = callback;

  udc_send(ep, data, size);
}

//-----------------------------------------------------------------------------
void usb_recv(int ep, uint8_t *data, int size, void (*callback)(void))
{
  usb_endpoints[ep].out.callback = callback;

  udc_recv(ep, data, size);
}

//-----------------------------------------------------------------------------
void usb_handle_standard_request(usb_request_t *request)
{
  switch ((request->bRequest << 8) | request->bmRequestType)
  {
    case USB_CMD(IN, DEVICE, STANDARD, GET_DESCRIPTOR):
    {
      uint8_t type = request->wValue >> 8;
      uint8_t index = request->wValue & 0xff;
      uint16_t length = request->wLength;

      if (USB_DEVICE_DESCRIPTOR == type)
      {
        length = LIMIT(length, usb_device_descriptor.bLength);

        udc_control_send((uint8_t *)&usb_device_descriptor, length);
      }
      else if (USB_CONFIGURATION_DESCRIPTOR == type)
      {
        length = LIMIT(length, usb_configuration_hierarchy.configuration.wTotalLength);

        udc_control_send((uint8_t *)&usb_configuration_hierarchy, length);
      }
      else if (USB_STRING_DESCRIPTOR == type)
      {
        if (0 == index)
        {
          length = LIMIT(length, usb_string_descriptor_zero.bLength);

          udc_control_send((uint8_t *)&usb_string_descriptor_zero, length);
        }
        else if (index < USB_STR_COUNT)
        {
          const char *str = usb_strings[index];
          int len = strlen(str);
          int size = len*2 + 2;
          alignas(4) uint8_t buf[size];

          buf[0] = size;
          buf[1] = USB_STRING_DESCRIPTOR;

          for (int i = 0; i < len; i++)
          {
            buf[2 + i*2] = str[i];
            buf[3 + i*2] = 0;
          }

          length = LIMIT(length, size);

          udc_control_send(buf, length);
        }
        else
        {
          udc_control_stall();
        }
      }
      else
        udc_control_stall();
    }  break;

    case USB_CMD(OUT, DEVICE, STANDARD, SET_ADDRESS):
    {
      udc_control_send_zlp();
      udc_set_address(request->wValue);
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, SET_CONFIGURATION):
    {
      usb_config = request->wValue;

      udc_control_send_zlp();

      if (usb_config)
      {
        int size = usb_configuration_hierarchy.configuration.wTotalLength;
        usb_descriptor_header_t *desc = (usb_descriptor_header_t *)&usb_configuration_hierarchy;

        while (size)
        {
          if (USB_ENDPOINT_DESCRIPTOR == desc->bDescriptorType)
            udc_configure_endpoint((usb_endpoint_descriptor_t *)desc);

          size -= desc->bLength;
          desc = (usb_descriptor_header_t *)((uint8_t *)desc + desc->bLength);
        }

        usb_configuration_callback(usb_config);
      }
    } break;

    case USB_CMD(IN, DEVICE, STANDARD, GET_CONFIGURATION):
    {
      uint8_t config = usb_config;
      udc_control_send(&config, sizeof(config));
    } break;

    case USB_CMD(IN, DEVICE, STANDARD, GET_STATUS):
    case USB_CMD(IN, INTERFACE, STANDARD, GET_STATUS):
    {
      uint16_t status = 0;
      udc_control_send((uint8_t *)&status, sizeof(status));
    } break;

    case USB_CMD(IN, ENDPOINT, STANDARD, GET_STATUS):
    {
      int ep = request->wIndex & USB_INDEX_MASK;
      int dir = request->wIndex & USB_DIRECTION_MASK;
      uint16_t status = 0;

      if (udc_endpoint_configured(ep, dir))
      {
        status = udc_endpoint_get_status(ep, dir);
        udc_control_send((uint8_t *)&status, sizeof(status));
      }
      else
      {
        udc_control_stall();
      }
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, SET_FEATURE):
    {
      udc_control_stall();
    } break;

    case USB_CMD(OUT, INTERFACE, STANDARD, SET_FEATURE):
    {
      udc_control_send_zlp();
    } break;

    case USB_CMD(OUT, ENDPOINT, STANDARD, SET_FEATURE):
    {
      int ep = request->wIndex & USB_INDEX_MASK;
      int dir = request->wIndex & USB_DIRECTION_MASK;

      if (0 == request->wValue && ep && udc_endpoint_configured(ep, dir))
      {
        udc_endpoint_set_feature(ep, dir);
        udc_control_send_zlp();
      }
      else
      {
        udc_control_stall();
      }
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, CLEAR_FEATURE):
    {
      udc_control_stall();
    } break;

    case USB_CMD(OUT, INTERFACE, STANDARD, CLEAR_FEATURE):
    {
      udc_control_send_zlp();
    } break;

    case USB_CMD(OUT, ENDPOINT, STANDARD, CLEAR_FEATURE):
    {
      int ep = request->wIndex & USB_INDEX_MASK;
      int dir = request->wIndex & USB_DIRECTION_MASK;

      if (0 == request->wValue && ep && udc_endpoint_configured(ep, dir))
      {
        udc_endpoint_clear_feature(ep, dir);
        udc_control_send_zlp();
      }
      else
      {
        udc_control_stall();
      }
    } break;

    // TODO: This actually belongs to HID implementation
    case USB_CMD(IN, INTERFACE, STANDARD, GET_DESCRIPTOR):
    {
      uint16_t length = request->wLength;

      length = LIMIT(length, sizeof(usb_hid_report_descriptor));

      udc_control_send((uint8_t *)usb_hid_report_descriptor, length);
    } break;

    default:
    {
      udc_control_stall();
    } break;
  }
}

//-----------------------------------------------------------------------------
void udc_recv_callback(int ep)
{
  if (usb_endpoints[ep].out.callback)
    usb_endpoints[ep].out.callback();
}

//-----------------------------------------------------------------------------
void udc_send_callback(int ep)
{
  if (usb_endpoints[ep].in.callback)
    usb_endpoints[ep].in.callback();
}

