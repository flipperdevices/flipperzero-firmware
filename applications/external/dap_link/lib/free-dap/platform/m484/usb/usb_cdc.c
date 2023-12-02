// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "usb.h"
#include "usb_std.h"
#include "usb_cdc.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define ONE_SHOT_STATES (USB_CDC_SERIAL_STATE_BREAK | USB_CDC_SERIAL_STATE_RING | \
  USB_CDC_SERIAL_STATE_FRAMING | USB_CDC_SERIAL_STATE_PARITY | USB_CDC_SERIAL_STATE_OVERRUN)

/*- Prototypes --------------------------------------------------------------*/
static void usb_cdc_send_state_notify(void);
static void usb_cdc_ep_comm_callback(void);

/*- Variables ---------------------------------------------------------------*/
static usb_cdc_line_coding_t usb_cdc_line_coding =
{
  .dwDTERate   = 115200,
  .bCharFormat = USB_CDC_1_STOP_BIT,
  .bParityType = USB_CDC_NO_PARITY,
  .bDataBits   = USB_CDC_8_DATA_BITS,
};

static alignas(4) usb_cdc_notify_serial_state_t usb_cdc_notify_message;
static int usb_cdc_serial_state;
static bool usb_cdc_comm_busy;

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_cdc_init(void)
{
  usb_set_send_callback(USB_CDC_EP_COMM, usb_cdc_ep_comm_callback);
  usb_set_send_callback(USB_CDC_EP_SEND, usb_cdc_send_callback);
  usb_set_recv_callback(USB_CDC_EP_RECV, usb_cdc_recv_callback);

  usb_cdc_notify_message.request.bmRequestType = USB_IN_TRANSFER |
      USB_INTERFACE_RECIPIENT | USB_CLASS_REQUEST;
  usb_cdc_notify_message.request.bRequest = USB_CDC_NOTIFY_SERIAL_STATE;
  usb_cdc_notify_message.request.wValue = 0;
  usb_cdc_notify_message.request.wIndex = 0;
  usb_cdc_notify_message.request.wLength = sizeof(uint16_t);
  usb_cdc_notify_message.value = 0;

  usb_cdc_serial_state = 0;
  usb_cdc_comm_busy = false;

  usb_cdc_line_coding_updated(&usb_cdc_line_coding);
}

//-----------------------------------------------------------------------------
void usb_cdc_send(uint8_t *data, int size)
{
  usb_send(USB_CDC_EP_SEND, data, size);
}

//-----------------------------------------------------------------------------
void usb_cdc_recv(uint8_t *data, int size)
{
  usb_recv(USB_CDC_EP_RECV, data, size);
}

//-----------------------------------------------------------------------------
void usb_cdc_set_state(int mask)
{
  usb_cdc_serial_state |= mask;

  usb_cdc_send_state_notify();
}

//-----------------------------------------------------------------------------
void usb_cdc_clear_state(int mask)
{
  usb_cdc_serial_state &= ~mask;

  usb_cdc_send_state_notify();
}

//-----------------------------------------------------------------------------
usb_cdc_line_coding_t *usb_cdc_get_line_coding(void)
{
  return &usb_cdc_line_coding;
}

//-----------------------------------------------------------------------------
static void usb_cdc_send_state_notify(void)
{
  if (usb_cdc_comm_busy)
    return;

  if (usb_cdc_serial_state != usb_cdc_notify_message.value)
  {
    usb_cdc_comm_busy = true;
    usb_cdc_notify_message.value = usb_cdc_serial_state;
    usb_cdc_serial_state &= ~ONE_SHOT_STATES;
    usb_send(USB_CDC_EP_COMM, (uint8_t *)&usb_cdc_notify_message, sizeof(usb_cdc_notify_serial_state_t));
  }
}

//-----------------------------------------------------------------------------
static void usb_cdc_ep_comm_callback(void)
{
  usb_cdc_comm_busy = false;
  usb_cdc_notify_message.value &= ~ONE_SHOT_STATES;
  usb_cdc_send_state_notify();
}

//-----------------------------------------------------------------------------
static void usb_cdc_set_line_coding_handler(uint8_t *data, int size)
{
  usb_cdc_line_coding_t *line_coding = (usb_cdc_line_coding_t *)data;

  if (sizeof(usb_cdc_line_coding_t) != size)
    return;

  usb_cdc_line_coding = *line_coding;

  usb_cdc_line_coding_updated(&usb_cdc_line_coding);
}

//-----------------------------------------------------------------------------
bool usb_cdc_handle_request(usb_request_t *request)
{
  int length = request->wLength;

  switch (USB_CMD_VALUE(request))
  {
    case USB_CMD(OUT, INTERFACE, CLASS, CDC_SET_LINE_CODING):
    {
      length = USB_LIMIT(length, sizeof(usb_cdc_line_coding_t));

      usb_control_recv(usb_cdc_set_line_coding_handler);
    } break;

    case USB_CMD(IN, INTERFACE, CLASS, CDC_GET_LINE_CODING):
    {
      length = USB_LIMIT(length, sizeof(usb_cdc_line_coding_t));

      usb_control_send((uint8_t *)&usb_cdc_line_coding, length);
    } break;

    case USB_CMD(OUT, INTERFACE, CLASS, CDC_SET_CONTROL_LINE_STATE):
    {
      usb_cdc_control_line_state_update(request->wValue);

      usb_control_send_zlp();
    } break;

    case USB_CMD(OUT, INTERFACE, CLASS, CDC_SEND_BREAK):
    {
      usb_cdc_send_break(request->wValue);

      usb_control_send_zlp();
    } break;

    default:
      return false;
  }

  return true;
}
