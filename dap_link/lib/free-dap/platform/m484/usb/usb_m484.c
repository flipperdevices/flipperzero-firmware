// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "M480.h"
#include "usb.h"
#include "usb_std.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_EP_NUM     12
#define USB_MEM_SIZE   4096

/*- Variables ---------------------------------------------------------------*/
static uint8_t usb_ctrl_out_buf[USB_CTRL_EP_SIZE];
static void (*usb_control_recv_callback)(uint8_t *data, int size);
static uint8_t *usb_ep_data[USB_EP_NUM];
static int usb_ep_size[USB_EP_NUM];
static int usb_setup_length;
static int usb_ep_mem_ptr;

/*- Prototypes --------------------------------------------------------------*/
static void usb_reset_endpoints(void);

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_hw_init(void)
{
  SYS->USBPHY = SYS_USBPHY_HSUSBEN_Msk | SYS_USBPHY_SBO_Msk;

  for (int i = 0; i < 2000; i++) // Delay more than 10 us
    asm("nop");

  SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

  CLK->AHBCLK |= CLK_AHBCLK_HSUSBDCKEN_Msk;

  HSUSBD->PHYCTL = HSUSBD_PHYCTL_PHYEN_Msk;

  while (0 == HSUSBD->EP[0].EPMPS)
    HSUSBD->EP[0].EPMPS = 1;

  HSUSBD->FADDR = 0;
  HSUSBD->OPER  = HSUSBD_OPER_HISPDEN_Msk;

  usb_setup_length = -1;

  usb_reset_endpoints();
}

//-----------------------------------------------------------------------------
void usb_attach(void)
{
  HSUSBD->PHYCTL |= HSUSBD_PHYCTL_DPPUEN_Msk;
}

//-----------------------------------------------------------------------------
void usb_detach(void)
{
  HSUSBD->PHYCTL &= ~HSUSBD_PHYCTL_DPPUEN_Msk;
}

//-----------------------------------------------------------------------------
static void usb_reset_endpoints(void)
{
  for (int i = 0; i < USB_EP_NUM; i++)
  {
    usb_ep_size[i] = 0;
    usb_ep_data[i] = NULL;

    HSUSBD->EP[i].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
    HSUSBD->EP[i].EPCFG    = 0;
    HSUSBD->EP[i].EPMPS    = 0;
    HSUSBD->EP[i].EPBUFST  = 0;
    HSUSBD->EP[i].EPBUFEND = 0;
  }
}

//-----------------------------------------------------------------------------
void usb_configure_endpoint(usb_endpoint_descriptor_t *desc)
{
  int ep, dir, type, size;

  ep = desc->bEndpointAddress & USB_INDEX_MASK;
  dir = desc->bEndpointAddress & USB_DIRECTION_MASK;
  type = desc->bmAttributes & 0x03;
  size = desc->wMaxPacketSize;

  if (USB_CONTROL_ENDPOINT == type)
    while (1);
  else if (USB_ISOCHRONOUS_ENDPOINT == type)
    type = (3/*Isochronous*/ << HSUSBD_EPCFG_EPTYPE_Pos);
  else if (USB_BULK_ENDPOINT == type)
    type = (1/*Bulk*/ << HSUSBD_EPCFG_EPTYPE_Pos);
  else
    type = (2/*Interrupt*/ << HSUSBD_EPCFG_EPTYPE_Pos);

  if (USB_IN_ENDPOINT == dir)
    dir = HSUSBD_EPCFG_EPDIR_Msk;
  else
    dir = 0;

  HSUSBD->EP[ep-1].EPMPS    = size;
  HSUSBD->EP[ep-1].EPBUFST  = usb_ep_mem_ptr;
  HSUSBD->EP[ep-1].EPBUFEND = usb_ep_mem_ptr + size - 1;

  usb_ep_mem_ptr += size;

  while (usb_ep_mem_ptr > USB_MEM_SIZE);

  HSUSBD->EP[ep-1].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk | (1/*Manual*/ << HSUSBD_EPRSPCTL_MODE_Pos);
  HSUSBD->EP[ep-1].EPCFG = HSUSBD_EPCFG_EPEN_Msk | type | dir | (ep << HSUSBD_EPCFG_EPNUM_Pos);
}

//-----------------------------------------------------------------------------
bool usb_endpoint_configured(int ep, int dir)
{
  return (0 != (HSUSBD->EP[ep-1].EPCFG & HSUSBD_EPCFG_EPTYPE_Msk));
  (void)dir;
}

//-----------------------------------------------------------------------------
int usb_endpoint_get_status(int ep, int dir)
{
  return (HSUSBD->EP[ep-1].EPRSPCTL & HSUSBD_EPRSPCTL_HALT_Msk);
  (void)dir;
}

//-----------------------------------------------------------------------------
void usb_endpoint_set_feature(int ep, int dir)
{
  HSUSBD->EP[ep-1].EPRSPCTL |= HSUSBD_EPRSPCTL_HALT_Msk;
  (void)dir;
}

//-----------------------------------------------------------------------------
void usb_endpoint_clear_feature(int ep, int dir)
{
  HSUSBD->EP[ep-1].EPRSPCTL = (1/*Manual*/ << HSUSBD_EPRSPCTL_MODE_Pos) | HSUSBD_EPRSPCTL_TOGGLE_Msk;
  (void)dir;
}

//-----------------------------------------------------------------------------
void usb_set_address(int address)
{
  HSUSBD->FADDR = address;
}

//-----------------------------------------------------------------------------
void usb_send(int ep, uint8_t *data, int size)
{
  HSUSBD->DMACNT  = size;
  HSUSBD->DMAADDR = (uint32_t)data;
  HSUSBD->DMACTL  = (ep << HSUSBD_DMACTL_EPNUM_Pos) | HSUSBD_DMACTL_DMARD_Msk |
      HSUSBD_DMACTL_SVINEP_Msk | HSUSBD_DMACTL_DMAEN_Msk;

  while (0 == (HSUSBD->BUSINTSTS & HSUSBD_BUSINTSTS_DMADONEIF_Msk));
  HSUSBD->BUSINTSTS = HSUSBD_BUSINTSTS_DMADONEIF_Msk;

  HSUSBD->EP[ep-1].EPTXCNT = size;
}

//-----------------------------------------------------------------------------
void usb_recv(int ep, uint8_t *data, int size)
{
  usb_ep_size[ep-1] = size;
  usb_ep_data[ep-1] = data;
}

//-----------------------------------------------------------------------------
void usb_control_send_zlp(void)
{
  HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_STSDONEIF_Msk;
  HSUSBD->CEPCTL = 0; // Clear NAK. USB controller will send ZLP automatically.
  while (0 == (HSUSBD->CEPINTSTS & HSUSBD_CEPINTSTS_STSDONEIF_Msk));
}

//-----------------------------------------------------------------------------
void usb_control_stall(void)
{
  HSUSBD->CEPCTL = HSUSBD_CEPCTL_STALLEN_Msk;
}

//-----------------------------------------------------------------------------
void usb_control_send(uint8_t *data, int size)
{
  bool need_zlp = (size < usb_setup_length) &&
      ((size & (usb_device_descriptor.bMaxPacketSize0-1)) == 0);

  HSUSBD->CEPCTL = 0; // Clear NAK

  while (size)
  {
    int transfer_size = USB_LIMIT(size, usb_device_descriptor.bMaxPacketSize0);

    for (int i = 0; i < transfer_size; i++)
      HSUSBD->CEPDAT_BYTE = data[i];

    HSUSBD->CEPTXCNT = transfer_size;
    while (0 == (HSUSBD->CEPINTSTS & HSUSBD_CEPINTSTS_TXPKIF_Msk));
    HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_TXPKIF_Msk;

    size -= transfer_size;
    data += transfer_size;
  }

  if (need_zlp)
  {
    HSUSBD->CEPTXCNT = 0;
    while (0 == (HSUSBD->CEPINTSTS & HSUSBD_CEPINTSTS_TXPKIF_Msk));
    HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_TXPKIF_Msk;
  }
}

//-----------------------------------------------------------------------------
void usb_control_recv(void (*callback)(uint8_t *data, int size))
{
  usb_control_recv_callback = callback;
}

//-----------------------------------------------------------------------------
void usb_task(void)
{
  int busint = HSUSBD->BUSINTSTS;
  int cepint = HSUSBD->CEPINTSTS;
  int epint;

  if (busint & HSUSBD_BUSINTSTS_VBUSDETIF_Msk)
  {
    HSUSBD->BUSINTSTS = HSUSBD_BUSINTSTS_VBUSDETIF_Msk;

    if (HSUSBD->PHYCTL & HSUSBD_PHYCTL_VBUSDET_Msk)
      usb_attach();
    else
      usb_detach();
  }

  if (busint & HSUSBD_BUSINTSTS_RSTIF_Msk)
  {
    HSUSBD->BUSINTSTS = HSUSBD_BUSINTSTS_RSTIF_Msk;

    HSUSBD->FADDR  = 0;
    HSUSBD->DMACNT = 0;
    HSUSBD->DMACTL = HSUSBD_DMACTL_DMARST_Msk;
    HSUSBD->DMACTL = 0;

    usb_reset_endpoints();

    HSUSBD->CEPBUFST  = 0;
    HSUSBD->CEPBUFEND = usb_device_descriptor.bMaxPacketSize0-1;

    usb_ep_mem_ptr = usb_device_descriptor.bMaxPacketSize0;

    while (0 == (HSUSBD->OPER & HSUSBD_OPER_CURSPD_Msk)); // Block if FS
  }

  if (cepint & HSUSBD_CEPINTSTS_SETUPPKIF_Msk)
  {
    usb_request_t request;

    HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_SETUPPKIF_Msk;

    request.bmRequestType = HSUSBD->SETUP1_0;
    request.bRequest      = HSUSBD->SETUP1_0 >> 8;
    request.wValue        = HSUSBD->SETUP3_2;
    request.wIndex        = HSUSBD->SETUP5_4;
    request.wLength       = HSUSBD->SETUP7_6;

    usb_setup_length = request.wLength;

    if (!usb_handle_standard_request(&request))
      usb_control_stall();

    usb_setup_length = -1;
  }

  if (cepint & HSUSBD_CEPINTSTS_RXPKIF_Msk)
  {
    if (usb_control_recv_callback)
    {
      int size = HSUSBD->CEPRXCNT;

      for (int i = 0; i < size; i++)
        usb_ctrl_out_buf[i] = HSUSBD->CEPDAT_BYTE;

      usb_control_recv_callback(usb_ctrl_out_buf, size);
      usb_control_recv_callback = NULL;
      usb_control_send_zlp();
    }
    else
    {
      HSUSBD->CEPCTL = 0; // Clear NAK
    }

    HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_RXPKIF_Msk;
  }

  for (int ep = 0; ep < USB_EP_NUM; ep++)
  {
    epint = HSUSBD->EP[ep].EPINTSTS;

    if (epint & HSUSBD_EPINTSTS_RXPKIF_Msk)
    {
      int size = HSUSBD->EP[ep].EPDATCNT & HSUSBD_EPDATCNT_DATCNT_Msk;

      if (NULL == usb_ep_data[ep])
        continue;

      HSUSBD->EP[ep].EPINTSTS = HSUSBD_EPINTSTS_RXPKIF_Msk;

      HSUSBD->DMACNT  = size; // Note: currently the buffer must be at least endpoint size
      HSUSBD->DMAADDR = (uint32_t)usb_ep_data[ep];
      HSUSBD->DMACTL  = ((ep+1) << HSUSBD_DMACTL_EPNUM_Pos) | HSUSBD_DMACTL_DMAEN_Msk;

      while (0 == (HSUSBD->BUSINTSTS & HSUSBD_BUSINTSTS_DMADONEIF_Msk));
      HSUSBD->BUSINTSTS = HSUSBD_BUSINTSTS_DMADONEIF_Msk;

      usb_ep_data[ep] = NULL;

      usb_recv_callback(ep+1, size);
    }

    if (epint & HSUSBD_EPINTSTS_TXPKIF_Msk)
    {
      HSUSBD->EP[ep].EPINTSTS = HSUSBD_EPINTSTS_TXPKIF_Msk;
      usb_send_callback(ep+1);
    }
  }
}

