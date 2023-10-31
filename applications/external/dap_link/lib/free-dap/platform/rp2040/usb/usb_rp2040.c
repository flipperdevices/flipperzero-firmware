// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "rp2040.h"
#include "usb.h"
#include "usb_std.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_EP_NUM             16
#define USB_DPRAM_SIZE         4096
#define USB_DPRAM              ((usb_dpram_t *)USBCTRL_DPRAM_BASE)
#define USB_DPRAM_FIXED_SIZE   0x100
#define USB_DPRAM_BUF_OFFSET   (USB_DPRAM_FIXED_SIZE + USB_CTRL_EP_SIZE*2)

/*- Types -------------------------------------------------------------------*/
typedef struct
{
  __IOM uint8_t SETUP[8];

  struct // Starts with EP1
  {
    __IOM uint32_t IN;
    __IOM uint32_t OUT;
  } EP_CTRL[USB_EP_NUM-1];

  struct // Starts with EP0
  {
    __IOM uint32_t IN;
    __IOM uint32_t OUT;
  } EP_BUF_CTRL[USB_EP_NUM];

  __IOM uint8_t EP0_BUF[USB_CTRL_EP_SIZE];
  __IOM uint8_t DUMMY[USB_CTRL_EP_SIZE];
} usb_dpram_t;

typedef struct
{
  int      in_pid;
  volatile uint8_t *in_buf;
  int      out_pid;
  volatile uint8_t *out_buf;
  uint8_t  *out_data;
} usb_ep_t;

/*- Variables ---------------------------------------------------------------*/
static int usb_ep_buf_ptr = 0;
static usb_ep_t usb_ep[USB_EP_NUM];
static void (*usb_control_recv_callback)(uint8_t *data, int size);
static int usb_setup_length;

/*- Prototypes --------------------------------------------------------------*/
static void usb_reset_endpoints(void);

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_hw_init(void)
{
  RESETS_CLR->RESET = RESETS_RESET_usbctrl_Msk;
  while (0 == RESETS->RESET_DONE_b.usbctrl);

  memset(USBCTRL_DPRAM, 0, USB_DPRAM_SIZE);

  USBCTRL_REGS->USB_MUXING = USBCTRL_REGS_USB_MUXING_TO_PHY_Msk |
      USBCTRL_REGS_USB_MUXING_SOFTCON_Msk;

  USBCTRL_REGS->USB_PWR = USBCTRL_REGS_USB_PWR_VBUS_DETECT_Msk |
      USBCTRL_REGS_USB_PWR_VBUS_DETECT_OVERRIDE_EN_Msk;

  USBCTRL_REGS->MAIN_CTRL = USBCTRL_REGS_MAIN_CTRL_CONTROLLER_EN_Msk;

  USBCTRL_REGS->SIE_CTRL = USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk;

  USBCTRL_REGS->ADDR_ENDP = 0;

  USBCTRL_REGS->INTE = USBCTRL_REGS_INTE_BUFF_STATUS_Msk |
      USBCTRL_REGS_INTE_SETUP_REQ_Msk | USBCTRL_REGS_INTE_BUS_RESET_Msk;

  usb_reset_endpoints();

  usb_attach();
}

//-----------------------------------------------------------------------------
void usb_attach(void)
{
  USBCTRL_REGS_SET->SIE_CTRL = USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk;
}

//-----------------------------------------------------------------------------
void usb_detach(void)
{
  USBCTRL_REGS_CLR->SIE_CTRL = USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk;
}

//-----------------------------------------------------------------------------
static void usb_reset_endpoints(void)
{
  for (int i = 0; i < USB_EP_NUM-1; i++)
  {
    USB_DPRAM->EP_CTRL[i].IN  = 0;
    USB_DPRAM->EP_CTRL[i].OUT = 0;
  }

  memset(&usb_ep, 0, sizeof(usb_ep));

  usb_ep_buf_ptr = USB_DPRAM_BUF_OFFSET;
}

//-----------------------------------------------------------------------------
void usb_configure_endpoint(usb_endpoint_descriptor_t *desc)
{
  int ep, dir, type, size;

  ep = desc->bEndpointAddress & USB_INDEX_MASK;
  dir = desc->bEndpointAddress & USB_DIRECTION_MASK;
  type = desc->bmAttributes & 0x03;
  size = desc->wMaxPacketSize & 0x3ff;

  if (size <= 64)
    size = 64;
  else if (size <= 128)
    size = 128;
  else if (size <= 256)
    size = 256;
  else if (size <= 512)
    size = 512;
  else
    size = 1024;

  if (USB_IN_ENDPOINT == dir)
  {
    usb_ep[ep].in_buf = (volatile uint8_t *)(USBCTRL_DPRAM_BASE + usb_ep_buf_ptr);

    USB_DPRAM->EP_CTRL[ep-1].IN = USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk |
        USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_Msk |
        (type << USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Pos) |
        (usb_ep_buf_ptr << USBCTRL_DPRAM_EP1_IN_CONTROL_BUFFER_ADDRESS_Pos);
  }
  else
  {
    usb_ep[ep].out_buf = (volatile uint8_t *)(USBCTRL_DPRAM_BASE + usb_ep_buf_ptr);

    USB_DPRAM->EP_CTRL[ep-1].OUT = USBCTRL_DPRAM_EP1_OUT_CONTROL_ENABLE_Msk |
        USBCTRL_DPRAM_EP1_OUT_CONTROL_INTERRUPT_PER_BUFF_Msk |
        (type << USBCTRL_DPRAM_EP1_OUT_CONTROL_ENDPOINT_TYPE_Pos) |
        (usb_ep_buf_ptr << USBCTRL_DPRAM_EP1_OUT_CONTROL_BUFFER_ADDRESS_Pos);
  }

  usb_ep_buf_ptr += size;
}

//-----------------------------------------------------------------------------
bool usb_endpoint_configured(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    return (USB_DPRAM->EP_CTRL[ep-1].IN & USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk) != 0;
  else
    return (USB_DPRAM->EP_CTRL[ep-1].OUT & USBCTRL_DPRAM_EP1_OUT_CONTROL_ENABLE_Msk) != 0;
}

//-----------------------------------------------------------------------------
int usb_endpoint_get_status(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    return (USB_DPRAM->EP_BUF_CTRL[ep].IN & USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk) != 0;
  else
    return (USB_DPRAM->EP_BUF_CTRL[ep].OUT & USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_STALL_Msk) != 0;
}

//-----------------------------------------------------------------------------
void usb_endpoint_set_feature(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    USB_DPRAM->EP_BUF_CTRL[ep].IN = USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk;
  else
    USB_DPRAM->EP_BUF_CTRL[ep].OUT = USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_STALL_Msk;
}

//-----------------------------------------------------------------------------
void usb_endpoint_clear_feature(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
  {
    usb_ep[ep].in_pid = 0;
    USB_DPRAM->EP_BUF_CTRL[ep].IN &= ~USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk;
  }
  else
  {
    usb_ep[0].out_pid = 0;
    USB_DPRAM->EP_BUF_CTRL[ep].OUT &= ~USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_STALL_Msk;
  }
}

//-----------------------------------------------------------------------------
void usb_set_address(int address)
{
  USBCTRL_REGS->ADDR_ENDP = address;
}

//-----------------------------------------------------------------------------
static void usb_start_in_transfer(int ep, int size)
{
  uint32_t v = size | USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_Msk |
      (usb_ep[ep].in_pid ? USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_PID_0_Msk : 0);
  usb_ep[ep].in_pid ^= 1;

  USB_DPRAM->EP_BUF_CTRL[ep].IN = v;
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  USB_DPRAM->EP_BUF_CTRL[ep].IN = v | USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_Msk;
}

//-----------------------------------------------------------------------------
static void usb_start_out_transfer(int ep, int size)
{
  uint32_t v = size | (usb_ep[ep].out_pid ? USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_PID_0_Msk : 0);
  usb_ep[ep].out_pid ^= 1;

  USB_DPRAM->EP_BUF_CTRL[ep].OUT = v;
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  USB_DPRAM->EP_BUF_CTRL[ep].OUT = v | USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_AVAILABLE_0_Msk;
}

//-----------------------------------------------------------------------------
void usb_send(int ep, uint8_t *data, int size)
{
  for (int i = 0; i < size; i++)
    usb_ep[ep].in_buf[i] = data[i];

  usb_start_in_transfer(ep, size);
}

//-----------------------------------------------------------------------------
void usb_recv(int ep, uint8_t *data, int size)
{
  usb_ep[ep].out_data = data;
  usb_start_out_transfer(ep, size);
}

//-----------------------------------------------------------------------------
void usb_control_send_zlp(void)
{
  usb_start_in_transfer(0, 0);

  while (0 == (USBCTRL_REGS->BUFF_STATUS & USBCTRL_REGS_BUFF_STATUS_EP0_IN_Msk));
  USBCTRL_REGS->BUFF_STATUS = USBCTRL_REGS_BUFF_STATUS_EP0_IN_Msk;
}

//-----------------------------------------------------------------------------
void usb_control_stall(void)
{
  USBCTRL_REGS->EP_STALL_ARM = USBCTRL_REGS_EP_STALL_ARM_EP0_IN_Msk;
  USB_DPRAM->EP_BUF_CTRL[0].IN = USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk;
}

//-----------------------------------------------------------------------------
void usb_control_send(uint8_t *data, int size)
{
  bool need_zlp = (size < usb_setup_length) &&
      ((size & (usb_device_descriptor.bMaxPacketSize0-1)) == 0);

  while (size)
  {
    int transfer_size = USB_LIMIT(size, usb_device_descriptor.bMaxPacketSize0);

    for (int i = 0; i < transfer_size; i++)
      usb_ep[0].in_buf[i] = data[i];

    usb_start_in_transfer(0, transfer_size);

    while (0 == (USBCTRL_REGS->BUFF_STATUS & USBCTRL_REGS_BUFF_STATUS_EP0_IN_Msk));
    USBCTRL_REGS->BUFF_STATUS = USBCTRL_REGS_BUFF_STATUS_EP0_IN_Msk;

    size -= transfer_size;
    data += transfer_size;
  }

  if (need_zlp)
    usb_control_send_zlp();

  usb_start_out_transfer(0, USB_CTRL_EP_SIZE);
}

//-----------------------------------------------------------------------------
void usb_control_recv(void (*callback)(uint8_t *data, int size))
{
  usb_control_recv_callback = callback;

  usb_start_out_transfer(0, USB_CTRL_EP_SIZE);
}

//-----------------------------------------------------------------------------
void usb_task(void)
{
  uint32_t status = USBCTRL_REGS->INTS;
  uint32_t flags;

  if (status & USBCTRL_REGS_INTS_BUS_RESET_Msk)
  {
    USBCTRL_REGS_CLR->SIE_STATUS = USBCTRL_REGS_SIE_STATUS_BUS_RESET_Msk;
    USBCTRL_REGS->ADDR_ENDP = 0;

    usb_reset_endpoints();

    usb_ep[0].in_buf = (volatile uint8_t *)(USBCTRL_DPRAM_BASE + USB_DPRAM_FIXED_SIZE);
    usb_ep[0].out_buf = (volatile uint8_t *)(USBCTRL_DPRAM_BASE + USB_DPRAM_FIXED_SIZE);
  }

  if (status & USBCTRL_REGS_INTS_SETUP_REQ_Msk)
  {
    usb_request_t *request = (usb_request_t *)USBCTRL_DPRAM;

    usb_setup_length = request->wLength;

    usb_ep[0].in_pid  = 1;
    usb_ep[0].out_pid = 1;

    USBCTRL_REGS_CLR->SIE_STATUS = USBCTRL_REGS_SIE_STATUS_SETUP_REC_Msk;

    if (!usb_handle_standard_request(request))
      usb_control_stall();

    usb_setup_length = -1;
  }

  if (status & USBCTRL_REGS_INTS_BUFF_STATUS_Msk)
  {
    status = USBCTRL_REGS->BUFF_STATUS;

    if (status & USBCTRL_REGS_BUFF_STATUS_EP0_OUT_Msk)
    {
      int size = USB_DPRAM->EP_BUF_CTRL[0].OUT & USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_LENGTH_0_Msk;

      if (usb_control_recv_callback)
      {
        usb_control_recv_callback((uint8_t *)usb_ep[0].out_buf, size);
        usb_control_recv_callback = NULL;
        usb_control_send_zlp();
      }
    }

    flags = status >> 2;

    for (int ep = 1; ep < USB_EP_NUM && flags > 0; ep++)
    {
      if (flags & 1) // IN
      {
        usb_send_callback(ep);
      }

      if (flags & 2) // OUT
      {
        int size = USB_DPRAM->EP_BUF_CTRL[ep].OUT & USBCTRL_DPRAM_EP0_OUT_BUFFER_CONTROL_LENGTH_0_Msk;

        for (int i = 0; i < size; i++)
          usb_ep[ep].out_data[i] = usb_ep[ep].out_buf[i];

        usb_recv_callback(ep, size);
      }

      flags >>= 2;
    }

    USBCTRL_REGS->BUFF_STATUS = status;
  }
}
