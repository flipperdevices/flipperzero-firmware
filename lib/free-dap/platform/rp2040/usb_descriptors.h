// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

/*- Includes ----------------------------------------------------------------*/
#include "usb_std.h"
#include "usb_cdc.h"
#include "usb_hid.h"
#include "usb_winusb.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_ENABLE_BOS
#define USB_BCD_VERSION      0x0210

enum
{
  USB_STR_ZERO,
  USB_STR_MANUFACTURER,
  USB_STR_PRODUCT,
  USB_STR_SERIAL_NUMBER,
  USB_STR_CMSIS_DAP_V1,
  USB_STR_CMSIS_DAP_V2,
  USB_STR_COM_PORT,
  USB_STR_COUNT,
};

enum
{
  USB_HID_EP_SEND  = 1,
  USB_HID_EP_RECV  = 2,
  USB_BULK_EP_RECV = 3,
  USB_BULK_EP_SEND = 4,
  USB_CDC_EP_COMM  = 5,
  USB_CDC_EP_SEND  = 6,
  USB_CDC_EP_RECV  = 7,
};

enum
{
  USB_INTF_HID,
  USB_INTF_BULK,
  USB_INTF_CDC_COMM,
  USB_INTF_CDC_DATA,
  USB_INTF_COUNT,
};

/*- Types -------------------------------------------------------------------*/
typedef struct USB_PACK
{
  usb_configuration_descriptor_t                   configuration;

  usb_interface_descriptor_t                       hid_interface;
  usb_hid_descriptor_t                             hid;
  usb_endpoint_descriptor_t                        hid_ep_in;
  usb_endpoint_descriptor_t                        hid_ep_out;

  usb_interface_descriptor_t                       bulk_interface;
  usb_endpoint_descriptor_t                        bulk_ep_out;
  usb_endpoint_descriptor_t                        bulk_ep_in;

  usb_interface_association_descriptor_t           iad;
  usb_interface_descriptor_t                       interface_comm;
  usb_cdc_header_functional_descriptor_t           cdc_header;
  usb_cdc_abstract_control_managment_descriptor_t  cdc_acm;
  usb_cdc_call_managment_functional_descriptor_t   cdc_call_mgmt;
  usb_cdc_union_functional_descriptor_t            cdc_union;
  usb_endpoint_descriptor_t                        ep_comm;
  usb_interface_descriptor_t                       interface_data;
  usb_endpoint_descriptor_t                        ep_in;
  usb_endpoint_descriptor_t                        ep_out;
} usb_configuration_hierarchy_t;

typedef struct USB_PACK
{
  usb_binary_object_store_descriptor_t             bos;
  usb_winusb_capability_descriptor_t               winusb;
} usb_bos_hierarchy_t;

typedef struct USB_PACK
{
  usb_winusb_subset_header_function_t              header;
  usb_winusb_feature_compatble_id_t                comp_id;
  usb_winusb_feature_reg_property_guids_t          property;
} usb_msos_descriptor_subset_t;

typedef struct USB_PACK
{
  usb_winusb_set_header_descriptor_t               header;
  usb_msos_descriptor_subset_t                     subset;
} usb_msos_descriptor_set_t;

//-----------------------------------------------------------------------------
extern const usb_device_descriptor_t usb_device_descriptor;
extern const usb_configuration_hierarchy_t usb_configuration_hierarchy;
extern const usb_bos_hierarchy_t usb_bos_hierarchy;
extern const usb_msos_descriptor_set_t usb_msos_descriptor_set;
extern const uint8_t usb_hid_report_descriptor[28];
extern const usb_string_descriptor_zero_t usb_string_descriptor_zero;
extern const char *usb_strings[];
extern const usb_class_handler_t usb_class_handlers[3];
extern char usb_serial_number[16];

#endif // _USB_DESCRIPTORS_H_
