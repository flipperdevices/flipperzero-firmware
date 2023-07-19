// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2016-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdalign.h>
#include "samd11.h"
#include "hal_gpio.h"
#include "nvm_data.h"
#include "usb.h"
#include "usb_std.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_EP_NUM     8

HAL_GPIO_PIN(USB_DM,   A, 24);
HAL_GPIO_PIN(USB_DP,   A, 25);

enum
{
  USB_DEVICE_EPCFG_EPTYPE_DISABLED    = 0,
  USB_DEVICE_EPCFG_EPTYPE_CONTROL     = 1,
  USB_DEVICE_EPCFG_EPTYPE_ISOCHRONOUS = 2,
  USB_DEVICE_EPCFG_EPTYPE_BULK        = 3,
  USB_DEVICE_EPCFG_EPTYPE_INTERRUPT   = 4,
  USB_DEVICE_EPCFG_EPTYPE_DUAL_BANK   = 5,
};

enum
{
  USB_DEVICE_PCKSIZE_SIZE_8    = 0,
  USB_DEVICE_PCKSIZE_SIZE_16   = 1,
  USB_DEVICE_PCKSIZE_SIZE_32   = 2,
  USB_DEVICE_PCKSIZE_SIZE_64   = 3,
  USB_DEVICE_PCKSIZE_SIZE_128  = 4,
  USB_DEVICE_PCKSIZE_SIZE_256  = 5,
  USB_DEVICE_PCKSIZE_SIZE_512  = 6,
  USB_DEVICE_PCKSIZE_SIZE_1023 = 7,
};

/*- Types -------------------------------------------------------------------*/
typedef union
{
  UsbDeviceDescBank    bank[2];
  struct
  {
    UsbDeviceDescBank  out;
    UsbDeviceDescBank  in;
  };
} udc_mem_t;

/*- Variables ---------------------------------------------------------------*/
static alignas(4) udc_mem_t udc_mem[USB_EP_NUM];
static alignas(4) uint8_t usb_ctrl_in_buf[64];
static alignas(4) uint8_t usb_ctrl_out_buf[64];
static void (*usb_control_recv_callback)(uint8_t *data, int size);
static int usb_setup_length;

/*- Prototypes --------------------------------------------------------------*/
static void usb_reset_endpoints(void);

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_hw_init(void)
{
  HAL_GPIO_USB_DM_pmuxen(PORT_PMUX_PMUXE_G_Val);
  HAL_GPIO_USB_DP_pmuxen(PORT_PMUX_PMUXE_G_Val);

  PM->APBBMASK.reg |= PM_APBBMASK_USB;

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(USB_GCLK_ID) |
      GCLK_CLKCTRL_GEN(0);

  USB->DEVICE.CTRLA.bit.SWRST = 1;
  while (USB->DEVICE.SYNCBUSY.bit.SWRST);

  USB->DEVICE.PADCAL.bit.TRANSN = NVM_READ_CAL(NVM_USB_TRANSN);
  USB->DEVICE.PADCAL.bit.TRANSP = NVM_READ_CAL(NVM_USB_TRANSP);
  USB->DEVICE.PADCAL.bit.TRIM   = NVM_READ_CAL(NVM_USB_TRIM);

  usb_setup_length = -1;

  for (int i = 0; i < (int)sizeof(udc_mem); i++)
    ((uint8_t *)udc_mem)[i] = 0;

  USB->DEVICE.DESCADD.reg = (uint32_t)udc_mem;

  USB->DEVICE.CTRLA.bit.MODE = USB_CTRLA_MODE_DEVICE_Val;
  USB->DEVICE.CTRLA.bit.RUNSTDBY = 1;
  USB->DEVICE.CTRLB.bit.SPDCONF = USB_DEVICE_CTRLB_SPDCONF_FS_Val;
  USB->DEVICE.CTRLB.bit.DETACH = 0;

  USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
  USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;

  USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;

  usb_reset_endpoints();
}

//-----------------------------------------------------------------------------
void usb_attach(void)
{
  USB->DEVICE.CTRLB.bit.DETACH = 0;
}

//-----------------------------------------------------------------------------
void usb_detach(void)
{
  USB->DEVICE.CTRLB.bit.DETACH = 1;
}

//-----------------------------------------------------------------------------
static void usb_reset_endpoints(void)
{
  for (int i = 0; i < USB_EP_NUM; i++)
    USB->DEVICE.DeviceEndpoint[i].EPCFG.reg = 0;
}

//-----------------------------------------------------------------------------
void usb_configure_endpoint(usb_endpoint_descriptor_t *desc)
{
  int ep, dir, type, size;

  ep = desc->bEndpointAddress & USB_INDEX_MASK;
  dir = desc->bEndpointAddress & USB_DIRECTION_MASK;
  type = desc->bmAttributes & 0x03;
  size = desc->wMaxPacketSize & 0x3ff;

  if (size <= 8)
    size = USB_DEVICE_PCKSIZE_SIZE_8;
  else if (size <= 16)
    size = USB_DEVICE_PCKSIZE_SIZE_16;
  else if (size <= 32)
    size = USB_DEVICE_PCKSIZE_SIZE_32;
  else if (size <= 64)
    size = USB_DEVICE_PCKSIZE_SIZE_64;
  else if (size <= 128)
    size = USB_DEVICE_PCKSIZE_SIZE_128;
  else if (size <= 256)
    size = USB_DEVICE_PCKSIZE_SIZE_256;
  else if (size <= 512)
    size = USB_DEVICE_PCKSIZE_SIZE_512;
  else if (size <= 1023)
    size = USB_DEVICE_PCKSIZE_SIZE_1023;
  else
    while (1);

  if (USB_CONTROL_ENDPOINT == type)
    type = USB_DEVICE_EPCFG_EPTYPE_CONTROL;
  else if (USB_ISOCHRONOUS_ENDPOINT == type)
    type = USB_DEVICE_EPCFG_EPTYPE_ISOCHRONOUS;
  else if (USB_BULK_ENDPOINT == type)
    type = USB_DEVICE_EPCFG_EPTYPE_BULK;
  else
    type = USB_DEVICE_EPCFG_EPTYPE_INTERRUPT;

  if (USB_IN_ENDPOINT == dir)
  {
    USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 = type;
    USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT1 = 1;
    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLIN = 1;
    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK1RDY = 1;
    udc_mem[ep].in.PCKSIZE.bit.SIZE = size;
  }
  else
  {
    USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 = type;
    USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT0 = 1;
    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK0RDY = 1;
    udc_mem[ep].out.PCKSIZE.bit.SIZE = size;
  }
}

//-----------------------------------------------------------------------------
bool usb_endpoint_configured(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    return (USB_DEVICE_EPCFG_EPTYPE_DISABLED != USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1);
  else
    return (USB_DEVICE_EPCFG_EPTYPE_DISABLED != USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0);
}

//-----------------------------------------------------------------------------
int usb_endpoint_get_status(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1;
  else
    return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0;
}

//-----------------------------------------------------------------------------
void usb_endpoint_set_feature(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ1 = 1;
  else
    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ0 = 1;
}

//-----------------------------------------------------------------------------
void usb_endpoint_clear_feature(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
  {
    if (USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1)
    {
      USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ1 = 1;

      if (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.STALL1)
      {
        USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLIN = 1;
      }
    }
  }
  else
  {
    if (USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0)
    {
      USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ0 = 1;

      if (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.STALL0)
      {
        USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void usb_set_address(int address)
{
  USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | USB_DEVICE_DADD_DADD(address);
}

//-----------------------------------------------------------------------------
void usb_send(int ep, uint8_t *data, int size)
{
  udc_mem[ep].in.ADDR.reg = (uint32_t)data;
  udc_mem[ep].in.PCKSIZE.bit.BYTE_COUNT = size;
  udc_mem[ep].in.PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

  USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK1RDY = 1;
}

//-----------------------------------------------------------------------------
void usb_recv(int ep, uint8_t *data, int size)
{
  udc_mem[ep].out.ADDR.reg = (uint32_t)data;
  udc_mem[ep].out.PCKSIZE.bit.MULTI_PACKET_SIZE = size;
  udc_mem[ep].out.PCKSIZE.bit.BYTE_COUNT = 0;

  USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK0RDY = 1;
}

//-----------------------------------------------------------------------------
void usb_control_send_zlp(void)
{
  udc_mem[0].in.PCKSIZE.bit.BYTE_COUNT = 0;
  USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
  USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

  while (0 == USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);
}

//-----------------------------------------------------------------------------
void usb_control_stall(void)
{
  USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.STALLRQ1 = 1;
}

//-----------------------------------------------------------------------------
void usb_control_send(uint8_t *data, int size)
{
  bool need_zlp = (size < usb_setup_length) &&
      ((size & (usb_device_descriptor.bMaxPacketSize0-1)) == 0);

  // USB controller does not have access to the flash memory, so here we do
  // a manual multi-packet transfer. This way data can be located in in
  // the flash memory (big constant descriptors).

  while (size)
  {
    int transfer_size = USB_LIMIT(size, usb_device_descriptor.bMaxPacketSize0);

    for (int i = 0; i < transfer_size; i++)
      usb_ctrl_in_buf[i] = data[i];

    udc_mem[0].in.ADDR.reg = (uint32_t)usb_ctrl_in_buf;
    udc_mem[0].in.PCKSIZE.bit.BYTE_COUNT = transfer_size;
    udc_mem[0].in.PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

    while (0 == USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);

    size -= transfer_size;
    data += transfer_size;
  }

  if (need_zlp)
    usb_control_send_zlp();
}

//-----------------------------------------------------------------------------
void usb_control_recv(void (*callback)(uint8_t *data, int size))
{
  usb_control_recv_callback = callback;
}

//-----------------------------------------------------------------------------
void usb_task(void)
{
  int flags, epints;

  if (USB->DEVICE.INTFLAG.bit.EORST)
  {
    USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
    USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN;

    usb_reset_endpoints();

    USB->DEVICE.DeviceEndpoint[0].EPCFG.reg =
        USB_DEVICE_EPCFG_EPTYPE0(USB_DEVICE_EPCFG_EPTYPE_CONTROL) |
        USB_DEVICE_EPCFG_EPTYPE1(USB_DEVICE_EPCFG_EPTYPE_CONTROL);
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK0RDY = 1;
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK1RDY = 1;

    udc_mem[0].in.PCKSIZE.bit.SIZE = USB_DEVICE_PCKSIZE_SIZE_64;

    udc_mem[0].out.ADDR.reg = (uint32_t)usb_ctrl_out_buf;
    udc_mem[0].out.PCKSIZE.bit.SIZE = USB_DEVICE_PCKSIZE_SIZE_64;
    udc_mem[0].out.PCKSIZE.bit.MULTI_PACKET_SIZE = 64;
    udc_mem[0].out.PCKSIZE.bit.BYTE_COUNT = 0;

    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;
    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.TRCPT0 = 1;
  }

  if (USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.RXSTP)
  {
    usb_request_t *request = (usb_request_t *)usb_ctrl_out_buf;

    usb_setup_length = request->wLength;

    if (sizeof(usb_request_t) == udc_mem[0].out.PCKSIZE.bit.BYTE_COUNT)
    {
      if (usb_handle_standard_request(request))
      {
        udc_mem[0].out.PCKSIZE.bit.BYTE_COUNT = 0;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK0RDY = 1;
        USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
      }
      else
      {
        usb_control_stall();
      }
    }
    else
    {
      usb_control_stall();
    }

    usb_setup_length = -1;

    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
  }
  else if (USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT0)
  {
    if (usb_control_recv_callback)
    {
      usb_control_recv_callback(usb_ctrl_out_buf, udc_mem[0].out.PCKSIZE.bit.BYTE_COUNT);
      usb_control_recv_callback = NULL;
      usb_control_send_zlp();
    }

    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK0RDY = 1;
    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
  }

  epints = USB->DEVICE.EPINTSMRY.reg;

  for (int i = 1; i < USB_EP_NUM && epints > 0; i++)
  {
    flags = USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg;
    epints &= ~(1 << i);

    if (flags & USB_DEVICE_EPINTFLAG_TRCPT0)
    {
      USB->DEVICE.DeviceEndpoint[i].EPSTATUSSET.bit.BK0RDY = 1;
      USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;

      usb_recv_callback(i, udc_mem[i].out.PCKSIZE.bit.BYTE_COUNT);
    }

    if (flags & USB_DEVICE_EPINTFLAG_TRCPT1)
    {
      USB->DEVICE.DeviceEndpoint[i].EPSTATUSCLR.bit.BK1RDY = 1;
      USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;

      usb_send_callback(i);
    }
  }
}

