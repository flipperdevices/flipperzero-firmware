// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2016-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "usb.h"
#include "usb_std.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_EP_NUM     16

/*- Types -------------------------------------------------------------------*/
typedef struct
{
  void (*send)(void);
  void (*recv)(int size);
} usb_ep_callback_t;

/*- Variables ---------------------------------------------------------------*/
static usb_ep_callback_t usb_ep_callbacks[USB_EP_NUM];

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_init(void)
{
  for (int i = 0; i < USB_EP_NUM; i++)
  {
    usb_ep_callbacks[i].send = NULL;
    usb_ep_callbacks[i].recv = NULL;
  }

  usb_hw_init();
}

//-----------------------------------------------------------------------------
void usb_set_send_callback(int ep, void (*callback)(void))
{
  usb_ep_callbacks[ep].send = callback;
}

//-----------------------------------------------------------------------------
void usb_set_recv_callback(int ep, void (*callback)(int size))
{
  usb_ep_callbacks[ep].recv = callback;
}

//-----------------------------------------------------------------------------
bool usb_handle_standard_request(usb_request_t *request)
{
  static uint8_t usb_config = 0;

  switch (USB_CMD_VALUE(request))
  {
    case USB_CMD(IN, DEVICE, STANDARD, GET_DESCRIPTOR):
    {
      int type = request->wValue >> 8;
      int index = request->wValue & 0xff;
      int length = request->wLength;

      if (USB_DEVICE_DESCRIPTOR == type)
      {
        length = USB_LIMIT(length, usb_device_descriptor.bLength);

        usb_control_send((uint8_t *)&usb_device_descriptor, length);
      }
      else if (USB_CONFIGURATION_DESCRIPTOR == type)
      {
        length = USB_LIMIT(length, sizeof(usb_configuration_hierarchy_t));
        usb_control_send((uint8_t *)&usb_configuration_hierarchy, length);
      }
      else if (USB_STRING_DESCRIPTOR == type)
      {
        if (0 == index)
        {
          length = USB_LIMIT(length, usb_string_descriptor_zero.bLength);
          usb_control_send((uint8_t *)&usb_string_descriptor_zero, length);
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

          length = USB_LIMIT(length, size);

          usb_control_send(buf, length);
        }
        else
        {
          return false;
        }
      }
#ifdef USB_ENABLE_BOS
      else if (USB_BINARY_OBJECT_STORE_DESCRIPTOR == type)
      {
        length = USB_LIMIT(length, sizeof(usb_bos_hierarchy_t));
        usb_control_send((uint8_t *)&usb_bos_hierarchy, length);
      }
#endif
      else
      {
        return false;
      }
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, SET_ADDRESS):
    {
      usb_control_send_zlp();
      usb_set_address(request->wValue);
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, SET_CONFIGURATION):
    {
      usb_config = request->wValue;

      usb_control_send_zlp();

      if (usb_config)
      {
        int size = usb_configuration_hierarchy.configuration.wTotalLength;
        usb_descriptor_header_t *desc = (usb_descriptor_header_t *)&usb_configuration_hierarchy;

        while (size)
        {
          if (USB_ENDPOINT_DESCRIPTOR == desc->bDescriptorType)
            usb_configure_endpoint((usb_endpoint_descriptor_t *)desc);

          size -= desc->bLength;
          desc = (usb_descriptor_header_t *)((uint8_t *)desc + desc->bLength);
        }

        usb_configuration_callback(usb_config);
      }
    } break;

    case USB_CMD(IN, DEVICE, STANDARD, GET_CONFIGURATION):
    {
      usb_control_send(&usb_config, sizeof(uint8_t));
    } break;

    case USB_CMD(IN, DEVICE, STANDARD, GET_STATUS):
    case USB_CMD(IN, INTERFACE, STANDARD, GET_STATUS):
    {
      uint16_t status = 0;
      usb_control_send((uint8_t *)&status, sizeof(status));
    } break;

    case USB_CMD(IN, ENDPOINT, STANDARD, GET_STATUS):
    {
      int ep = request->wIndex & USB_INDEX_MASK;
      int dir = request->wIndex & USB_DIRECTION_MASK;
      uint16_t status = 0;

      if (usb_endpoint_configured(ep, dir))
      {
        status = usb_endpoint_get_status(ep, dir);
        usb_control_send((uint8_t *)&status, sizeof(status));
      }
      else
      {
        return false;
      }
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, SET_FEATURE):
    {
      return false;
    } break;

    case USB_CMD(OUT, INTERFACE, STANDARD, SET_FEATURE):
    {
      usb_control_send_zlp();
    } break;

    case USB_CMD(OUT, ENDPOINT, STANDARD, SET_FEATURE):
    {
      int ep = request->wIndex & USB_INDEX_MASK;
      int dir = request->wIndex & USB_DIRECTION_MASK;

      if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir))
      {
        usb_endpoint_set_feature(ep, dir);
        usb_control_send_zlp();
      }
      else
      {
        return false;
      }
    } break;

    case USB_CMD(OUT, DEVICE, STANDARD, CLEAR_FEATURE):
    {
      return false;
    } break;

    case USB_CMD(OUT, INTERFACE, STANDARD, CLEAR_FEATURE):
    {
      usb_control_send_zlp();
    } break;

    case USB_CMD(OUT, ENDPOINT, STANDARD, CLEAR_FEATURE):
    {
      int ep = request->wIndex & USB_INDEX_MASK;
      int dir = request->wIndex & USB_DIRECTION_MASK;

      if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir))
      {
        usb_endpoint_clear_feature(ep, dir);
        usb_control_send_zlp();
      }
      else
      {
        return false;
      }
    } break;

    default:
    {
      for (int i = 0; i < USB_ARRAY_SIZE(usb_class_handlers); i++)
      {
        if (usb_class_handlers[i](request))
          return true;
      }

      return false;
    } break;
  }

  return true;
}

//-----------------------------------------------------------------------------
void usb_send_callback(int ep)
{
  if (usb_ep_callbacks[ep].send)
    usb_ep_callbacks[ep].send();
}

//-----------------------------------------------------------------------------
void usb_recv_callback(int ep, int size)
{
  if (usb_ep_callbacks[ep].recv)
    usb_ep_callbacks[ep].recv(size);
}
