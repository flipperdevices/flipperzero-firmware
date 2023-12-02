/*
 * Copyright (c) 2020, Alex Taradov <alex@taradov.com>
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
#include <string.h>
#include <stdbool.h>
#include <stdalign.h>
#include "same70.h"
#include "hal_gpio.h"
#include "utils.h"
#include "usb.h"
#include "usb_std.h"
#include "usb_descriptors.h"

/*- Definitions -------------------------------------------------------------*/
#define USBHS_RAM(ep)  ((volatile uint8_t *)(0xa0100000 + 0x8000 * (ep)))

/*- Constants ---------------------------------------------------------------*/
static const int usb_ep_banks[USB_EP_NUM] = USB_EP_BANKS;

/*- Variables ---------------------------------------------------------------*/
static uint8_t usb_ctrl_out_buf[64];
static void (*usb_control_recv_callback)(uint8_t *data, int size);
static uint8_t *usb_ep_data[USB_EP_NUM];
static int usb_ep_size[USB_EP_NUM];

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_hw_init(void)
{
  PMC->PMC_PCER1 = PMC_PCER1_PID34/*ID_USBHS*/;

  PMC->CKGR_UCKR = CKGR_UCKR_UPLLEN | CKGR_UCKR_UPLLCOUNT(3);
  while (0 == (PMC->PMC_SR & PMC_SR_LOCKU));

  PMC->PMC_USB = PMC_USB_USBS;

  USBHS->USBHS_CTRL = USBHS_CTRL_UIMOD_DEVICE | USBHS_CTRL_USBE;

  while (0 == (USBHS->USBHS_SR & USBHS_SR_CLKUSABLE));

  for (int i = 0; i < USB_EP_NUM; i++)
    usb_reset_endpoint(i, 0);

  usb_detach();
  usb_attach();

#ifdef USB_ASYNC
  NVIC_EnableIRQ(USBHS_IRQn);
#endif
}

//-----------------------------------------------------------------------------
void usb_attach(void)
{
  while (0 == (USBHS->USBHS_SR & USBHS_SR_CLKUSABLE));

  USBHS->USBHS_DEVICR = USBHS_DEVICR_SUSPC | USBHS_DEVICR_MSOFC |
      USBHS_DEVICR_SOFC | USBHS_DEVICR_EORSTC | USBHS_DEVICR_WAKEUPC |
      USBHS_DEVICR_EORSMC | USBHS_DEVICR_UPRSMC;

  USBHS->USBHS_DEVIER = USBHS_DEVIER_EORSTES;

  USBHS->USBHS_DEVCTRL &= ~USBHS_DEVCTRL_DETACH;
}

//-----------------------------------------------------------------------------
void usb_detach(void)
{
  USBHS->USBHS_DEVCTRL |= USBHS_DEVCTRL_DETACH;
}

//-----------------------------------------------------------------------------
void usb_reset_endpoint(int ep, int dir)
{
  USBHS->USBHS_DEVEPT |= (USBHS_DEVEPT_EPRST0 << ep);
  USBHS->USBHS_DEVEPT &= ~(USBHS_DEVEPT_EPRST0 << ep);
  USBHS->USBHS_DEVEPTIER[ep] = USBHS_DEVEPTIER_RSTDTS;
  USBHS->USBHS_DEVEPTIDR[ep] = USBHS_DEVEPTIDR_RXOUTEC | USBHS_DEVEPTIDR_TXINEC;

  usb_ep_size[ep] = 0;
  usb_ep_data[ep] = NULL;

  (void)dir;
}

//-----------------------------------------------------------------------------
void usb_configure_endpoint(usb_endpoint_descriptor_t *desc)
{
  int ep, dir, type, size, banks;

  ep = desc->bEndpointAddress & USB_INDEX_MASK;
  dir = desc->bEndpointAddress & USB_DIRECTION_MASK;
  type = desc->bmAttributes & 0x03;
  size = desc->wMaxPacketSize;

  usb_reset_endpoint(desc->bEndpointAddress, dir);

  if (USB_ISOCHRONOUS_ENDPOINT != type)
    USBHS->USBHS_DEVEPTIER[ep] = USBHS_DEVEPTIER_RSTDTS;

  if (size <= 8)
    size = USBHS_DEVEPTCFG_EPSIZE_8_BYTE;
  else if (size <= 16)
    size = USBHS_DEVEPTCFG_EPSIZE_16_BYTE;
  else if (size <= 32)
    size = USBHS_DEVEPTCFG_EPSIZE_32_BYTE;
  else if (size <= 64)
    size = USBHS_DEVEPTCFG_EPSIZE_64_BYTE;
  else if (size <= 128)
    size = USBHS_DEVEPTCFG_EPSIZE_128_BYTE;
  else if (size <= 256)
    size = USBHS_DEVEPTCFG_EPSIZE_256_BYTE;
  else if (size <= 512)
    size = USBHS_DEVEPTCFG_EPSIZE_512_BYTE;
  else if (size <= 1024)
    size = USBHS_DEVEPTCFG_EPSIZE_1024_BYTE;
  else
    while (1);

  if (USB_CONTROL_ENDPOINT == type)
    type = USBHS_DEVEPTCFG_EPTYPE_CTRL;
  else if (USB_ISOCHRONOUS_ENDPOINT == type)
    type = USBHS_DEVEPTCFG_EPTYPE_ISO;
  else if (USB_BULK_ENDPOINT == type)
    type = USBHS_DEVEPTCFG_EPTYPE_BLK;
  else
    type = USBHS_DEVEPTCFG_EPTYPE_INTRPT;

  if (USB_IN_ENDPOINT == dir)
    dir = USBHS_DEVEPTCFG_EPDIR_IN;
  else
    dir = USBHS_DEVEPTCFG_EPDIR_OUT;

  if (1 == usb_ep_banks[ep])
    banks = USBHS_DEVEPTCFG_EPBK_1_BANK;
  else if (2 == usb_ep_banks[ep])
    banks = USBHS_DEVEPTCFG_EPBK_2_BANK;
  else if (3 == usb_ep_banks[ep])
    banks = USBHS_DEVEPTCFG_EPBK_3_BANK;
  else
    while (1);

  USBHS->USBHS_DEVEPTCFG[ep] = type | size | dir | banks | 
      USBHS_DEVEPTCFG_NBTRANS_1_TRANS | USBHS_DEVEPTCFG_ALLOC;

  while (0 == (USBHS->USBHS_DEVEPTISR[ep] & USBHS_DEVEPTISR_CFGOK));

  USBHS->USBHS_DEVEPT |= (USBHS_DEVEPT_EPEN0 << ep);
  USBHS->USBHS_DEVIER = (USBHS_DEVIER_PEP_0 << ep);
}

//-----------------------------------------------------------------------------
bool usb_endpoint_configured(int ep, int dir)
{
  (void)dir;
  return (0 != (USBHS->USBHS_DEVEPT & (USBHS_DEVEPT_EPEN0 << ep)));
}

//-----------------------------------------------------------------------------
int usb_endpoint_get_status(int ep, int dir)
{
  (void)dir;
  return (0 != (USBHS->USBHS_DEVEPTIMR[ep] & USBHS_DEVEPTIMR_STALLRQ));
}

//-----------------------------------------------------------------------------
void usb_endpoint_set_feature(int ep, int dir)
{
  USBHS->USBHS_DEVEPTICR[ep] = USBHS_DEVEPTICR_STALLEDIC;
  USBHS->USBHS_DEVEPTIER[ep] = USBHS_DEVEPTIER_STALLRQS;
  (void)dir;
}

//-----------------------------------------------------------------------------
void usb_endpoint_clear_feature(int ep, int dir)
{
  if (USBHS->USBHS_DEVEPTIMR[ep] & USBHS_DEVEPTIMR_STALLRQ)
  {
    if (USBHS->USBHS_DEVEPTISR[ep] & USBHS_DEVEPTISR_STALLEDI)
      USBHS->USBHS_DEVEPTICR[ep] = USBHS_DEVEPTICR_STALLEDIC;

    USBHS->USBHS_DEVEPTIDR[ep] = USBHS_DEVEPTIDR_STALLRQC;
  }

  USBHS->USBHS_DEVEPTIER[ep] = USBHS_DEVEPTIER_RSTDTS;

  (void)dir;
}

//-----------------------------------------------------------------------------
void usb_set_address(int address)
{
  USBHS->USBHS_DEVCTRL &= ~USBHS_DEVCTRL_ADDEN;
  USBHS->USBHS_DEVCTRL &= ~USBHS_DEVCTRL_UADD_Msk;
  USBHS->USBHS_DEVCTRL |= address;
  USBHS->USBHS_DEVCTRL |= USBHS_DEVCTRL_ADDEN;
}

//-----------------------------------------------------------------------------
static bool endpoint_busy(int ep)
{
  int busy = ((USBHS->USBHS_DEVEPTISR[ep] & USBHS_DEVEPTISR_NBUSYBK_Msk) >> USBHS_DEVEPTISR_NBUSYBK_Pos);
  return (busy == usb_ep_banks[ep]);
}

//-----------------------------------------------------------------------------
static void dma_transfer(int ep, uint8_t *data, int size)
{
  int ch = ep-1;

  USBHS->USBHS_DEVDMA[ch].USBHS_DEVDMANXTDSC  = 0;
  USBHS->USBHS_DEVDMA[ch].USBHS_DEVDMAADDRESS = (uint32_t)data;
  USBHS->USBHS_DEVDMA[ch].USBHS_DEVDMACONTROL = USBHS_DEVDMACONTROL_CHANN_ENB |
      USBHS_DEVDMACONTROL_BUFF_LENGTH(size);
  while (0 == (USBHS->USBHS_DEVDMA[ch].USBHS_DEVDMASTATUS & USBHS_DEVDMASTATUS_END_BF_ST));
}

//-----------------------------------------------------------------------------
void usb_send(int ep, uint8_t *data, int size)
{
  while (endpoint_busy(ep));

  dma_transfer(ep, data, size);

  USBHS->USBHS_DEVEPTICR[ep] = USBHS_DEVEPTICR_TXINIC;
  USBHS->USBHS_DEVEPTIER[ep] = USBHS_DEVEPTIER_TXINES;
  USBHS->USBHS_DEVEPTIDR[ep] = USBHS_DEVEPTIDR_FIFOCONC;
}

//-----------------------------------------------------------------------------
void usb_recv(int ep, uint8_t *data, int size)
{
  usb_ep_size[ep] = size;
  usb_ep_data[ep] = data;

  USBHS->USBHS_DEVEPTICR[ep] = USBHS_DEVEPTICR_RXOUTIC;
  USBHS->USBHS_DEVEPTIER[ep] = USBHS_DEVEPTIER_RXOUTES;
  USBHS->USBHS_DEVEPTIDR[ep] = USBHS_DEVEPTIDR_FIFOCONC;
}

//-----------------------------------------------------------------------------
void usb_control_send_zlp(void)
{
  USBHS->USBHS_DEVEPTICR[0] = USBHS_DEVEPTICR_TXINIC;
  while (0 == (USBHS->USBHS_DEVEPTISR[0] & USBHS_DEVEPTISR_TXINI));
}

//-----------------------------------------------------------------------------
void usb_control_stall(void)
{
  USBHS->USBHS_DEVEPTIER[0] = USBHS_DEVEPTIER_STALLRQS;
}

//-----------------------------------------------------------------------------
void usb_control_send(uint8_t *data, int size)
{
  volatile uint8_t *usb_ram = USBHS_RAM(0);

  while (size)
  {
    int transfer_size = LIMIT(size, usb_device_descriptor.bMaxPacketSize0);

    for (int i = 0; i < transfer_size; i++)
    {
      usb_ram[i] = data[i];
      __DSB();
    }

    USBHS->USBHS_DEVEPTICR[0] = USBHS_DEVEPTICR_TXINIC;
    while (0 == (USBHS->USBHS_DEVEPTISR[0] & USBHS_DEVEPTISR_TXINI));

    size -= transfer_size;
    data += transfer_size;
  }
}

//-----------------------------------------------------------------------------
void usb_control_recv(void (*callback)(uint8_t *data, int size))
{
  usb_control_recv_callback = callback;
}

//-----------------------------------------------------------------------------
static int get_ep_byte_count(int ep)
{
  return ((USBHS->USBHS_DEVEPTISR[ep] & USBHS_DEVEPTISR_BYCT_Msk) >> USBHS_DEVEPTISR_BYCT_Pos);
}

//-----------------------------------------------------------------------------
static void usb_handler(void)
{
  if (USBHS->USBHS_DEVISR & USBHS_DEVISR_EORST)
  {
    for (int i = 0; i < USB_EP_NUM; i++)
      usb_reset_endpoint(i, 0);

    usb_set_address(0);

    USBHS->USBHS_DEVEPTIER[0] = USBHS_DEVEPTIER_RSTDTS;
    USBHS->USBHS_DEVEPTCFG[0] = USBHS_DEVEPTCFG_EPTYPE_CTRL |
        USBHS_DEVEPTCFG_ALLOC | USBHS_DEVEPTCFG_EPSIZE_64_BYTE |
        USBHS_DEVEPTCFG_EPBK_1_BANK | USBHS_DEVEPTCFG_NBTRANS_1_TRANS;

    while (0 == (USBHS->USBHS_DEVEPTISR[0] & USBHS_DEVEPTISR_CFGOK));

    USBHS->USBHS_DEVEPT = USBHS_DEVEPT_EPEN0;
    USBHS->USBHS_DEVIER = USBHS_DEVIER_PEP_0;

    USBHS->USBHS_DEVEPTICR[0] = USBHS_DEVEPTICR_RXSTPIC;
    USBHS->USBHS_DEVEPTIER[0] = USBHS_DEVEPTIER_RXSTPES;

    USBHS->USBHS_DEVICR = USBHS_DEVICR_EORSTC;
  }

  if (USBHS->USBHS_DEVEPTISR[0] & USBHS_DEVEPTISR_RXSTPI)
  {
    volatile uint8_t *data = USBHS_RAM(0);
    int size = get_ep_byte_count(0);

    for (int i = 0; i < (int)sizeof(usb_request_t); i++)
      usb_ctrl_out_buf[i] = data[i];

    USBHS->USBHS_DEVEPTICR[0] = USBHS_DEVEPTICR_RXSTPIC;

    if (size == sizeof(usb_request_t))
    {
      if (!usb_handle_standard_request((usb_request_t *)usb_ctrl_out_buf))
        usb_control_stall();
    }
    else
    {
      usb_control_stall();
    }
  }

  else if (USBHS->USBHS_DEVEPTISR[0] & USBHS_DEVEPTISR_RXOUTI)
  {
    USBHS->USBHS_DEVEPTICR[0] = USBHS_DEVEPTICR_RXOUTIC;

    // NOTE: This was not tested

    if (usb_control_recv_callback)
    {
      volatile uint8_t *data = USBHS_RAM(0);
      int size = LIMIT(get_ep_byte_count(0), usb_device_descriptor.bMaxPacketSize0);

      for (int i = 0; i < size; i++)
        usb_ctrl_out_buf[i] = data[i];

      usb_control_recv_callback(usb_ctrl_out_buf, size);
      usb_control_recv_callback = NULL;
      usb_control_send_zlp();
    }
  }

  for (int ep = 1; ep < USB_EP_NUM; ep++)
  {
    int sr = USBHS->USBHS_DEVEPTISR[ep] & USBHS->USBHS_DEVEPTIMR[ep];

    if (sr & USBHS_DEVEPTISR_RXOUTI)
    {
      int size = LIMIT(get_ep_byte_count(ep), usb_ep_size[ep]);

      dma_transfer(ep, usb_ep_data[ep], size);

      USBHS->USBHS_DEVEPTICR[ep] = USBHS_DEVEPTICR_RXOUTIC;
      USBHS->USBHS_DEVEPTIDR[ep] = USBHS_DEVEPTIDR_RXOUTEC;

      usb_recv_callback(ep, size);
    }

    if (sr & USBHS_DEVEPTISR_TXINI)
    {
      USBHS->USBHS_DEVEPTICR[ep] = USBHS_DEVEPTICR_TXINIC;
      USBHS->USBHS_DEVEPTIDR[ep] = USBHS_DEVEPTIDR_TXINEC;

      usb_send_callback(ep);
    }
  }
}

//-----------------------------------------------------------------------------
void irq_handler_usbhs(void)
{
#ifdef USB_ASYNC
  usb_handler();
#endif
}

//-----------------------------------------------------------------------------
void usb_task(void)
{
#ifndef USB_ASYNC
  usb_handler();
#endif
}


