// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdalign.h>
#include "usb_descriptors.h"

/*- Variables ---------------------------------------------------------------*/
const alignas(4) usb_device_descriptor_t usb_device_descriptor =
{
  .bLength            = sizeof(usb_device_descriptor_t),
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = USB_BCD_VERSION,
#ifdef HAL_CONFIG_ENABLE_VCP
  .bDeviceClass       = USB_DEVICE_CLASS_MISCELLANEOUS,
  .bDeviceSubClass    = USB_DEVICE_SUBCLASS_COMMON,
  .bDeviceProtocol    = USB_DEVICE_PROTOCOL_INTERFACE_ASSOCIATION,
#else
  .bDeviceClass       = 0x00,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,
#endif
  .bMaxPacketSize0    = USB_CTRL_EP_SIZE,
  .idVendor           = 0x6666,
#ifdef HAL_CONFIG_ENABLE_VCP
  .idProduct          = 0x9902,
#else
  .idProduct          = 0x9901,
#endif
  .bcdDevice          = 0x0100,
  .iManufacturer      = USB_STR_MANUFACTURER,
  .iProduct           = USB_STR_PRODUCT,
  .iSerialNumber      = USB_STR_SERIAL_NUMBER,
  .bNumConfigurations = 1,
};

const alignas(4) usb_configuration_hierarchy_t usb_configuration_hierarchy =
{
  .configuration =
  {
    .bLength             = sizeof(usb_configuration_descriptor_t),
    .bDescriptorType     = USB_CONFIGURATION_DESCRIPTOR,
    .wTotalLength        = sizeof(usb_configuration_hierarchy_t),
    .bNumInterfaces      = USB_INTF_COUNT,
    .bConfigurationValue = 1,
    .iConfiguration      = 0,
    .bmAttributes        = USB_ATTRIBUTE_BUS_POWERED,
    .bMaxPower           = USB_MAX_POWER(500),
  },

  // CMSIS-DAP v1
  .hid_interface =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = USB_INTF_HID,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 2,
    .bInterfaceClass     = USB_HID_DEVICE_CLASS,
    .bInterfaceSubClass  = 0,
    .bInterfaceProtocol  = 0,
    .iInterface          = USB_STR_CMSIS_DAP_V1,
  },

  .hid =
  {
    .bLength             = sizeof(usb_hid_descriptor_t),
    .bDescriptorType     = USB_HID_DESCRIPTOR,
    .bcdHID              = 0x0111,
    .bCountryCode        = 0,
    .bNumDescriptors     = 1,
    .bDescriptorType1    = USB_HID_REPORT_DESCRIPTOR,
    .wDescriptorLength   = sizeof(usb_hid_report_descriptor),
  },

  .hid_ep_in =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_HID_EP_SEND,
    .bmAttributes        = USB_INTERRUPT_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 1,
  },

  .hid_ep_out =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_OUT_ENDPOINT | USB_HID_EP_RECV,
    .bmAttributes        = USB_INTERRUPT_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 1,
  },

  // CMSIS-DAP v2
  .bulk_interface =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = USB_INTF_BULK,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 2,
    .bInterfaceClass     = USB_DEVICE_CLASS_VENDOR_SPECIFIC,
    .bInterfaceSubClass  = 0,
    .bInterfaceProtocol  = 0,
    .iInterface          = USB_STR_CMSIS_DAP_V2,
  },

  .bulk_ep_out =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_OUT_ENDPOINT | USB_BULK_EP_RECV,
    .bmAttributes        = USB_BULK_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 0,
  },

  .bulk_ep_in =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_BULK_EP_SEND,
    .bmAttributes        = USB_BULK_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 0,
  },

#ifdef HAL_CONFIG_ENABLE_VCP
  // VCP
  .iad =
  {
    .bLength             = sizeof(usb_interface_association_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_ASSOCIATION_DESCRIPTOR,
    .bFirstInterface     = USB_INTF_CDC_COMM,
    .bInterfaceCount     = 2,
    .bFunctionClass      = USB_CDC_COMM_CLASS,
    .bFunctionSubClass   = USB_CDC_ACM_SUBCLASS,
    .bFunctionProtocol   = 0,
    .iFunction           = USB_STR_COM_PORT,
  },

  .interface_comm =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = USB_INTF_CDC_COMM,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 1,
    .bInterfaceClass     = USB_CDC_COMM_CLASS,
    .bInterfaceSubClass  = USB_CDC_ACM_SUBCLASS,
    .bInterfaceProtocol  = 0,
    .iInterface          = 0,
  },

  .cdc_header =
  {
    .bFunctionalLength   = sizeof(usb_cdc_header_functional_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_HEADER_SUBTYPE,
    .bcdCDC              = USB_CDC_BCD_VERSION,
  },

  .cdc_acm =
  {
    .bFunctionalLength   = sizeof(usb_cdc_abstract_control_managment_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_ACM_SUBTYPE,
    .bmCapabilities      = USB_CDC_ACM_SUPPORT_LINE_REQUESTS | USB_CDC_ACM_SUPPORT_SENDBREAK_REQUESTS,
  },

  .cdc_call_mgmt =
  {
    .bFunctionalLength   = sizeof(usb_cdc_call_managment_functional_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_CALL_MGMT_SUBTYPE,
    .bmCapabilities      = USB_CDC_CALL_MGMT_OVER_DCI,
    .bDataInterface      = USB_INTF_CDC_DATA,
  },

  .cdc_union =
  {
    .bFunctionalLength   = sizeof(usb_cdc_union_functional_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_UNION_SUBTYPE,
    .bMasterInterface    = USB_INTF_CDC_COMM,
    .bSlaveInterface0    = USB_INTF_CDC_DATA,
  },

  .ep_comm =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_CDC_EP_COMM,
    .bmAttributes        = USB_INTERRUPT_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 1,
  },

  .interface_data =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = USB_INTF_CDC_DATA,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 2,
    .bInterfaceClass     = USB_CDC_DATA_CLASS,
    .bInterfaceSubClass  = USB_CDC_NO_SUBCLASS,
    .bInterfaceProtocol  = 0,
    .iInterface          = 0,
  },

  .ep_in =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_CDC_EP_SEND,
    .bmAttributes        = USB_BULK_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 0,
  },

  .ep_out =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_OUT_ENDPOINT | USB_CDC_EP_RECV,
    .bmAttributes        = USB_BULK_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 0,
  },
#endif
};

const alignas(4) usb_bos_hierarchy_t usb_bos_hierarchy =
{
  .bos =
  {
    .bLength             = sizeof(usb_binary_object_store_descriptor_t),
    .bDescriptorType     = USB_BINARY_OBJECT_STORE_DESCRIPTOR,
    .wTotalLength        = sizeof(usb_bos_hierarchy_t),
    .bNumDeviceCaps      = 1,
  },

  .winusb =
  {
    .bLength                = sizeof(usb_winusb_capability_descriptor_t),
    .bDescriptorType        = USB_DEVICE_CAPABILITY_DESCRIPTOR,
    .bDevCapabilityType     = USB_DEVICE_CAPABILITY_PLATFORM,
    .bReserved              = 0,
    .PlatformCapabilityUUID = USB_WINUSB_PLATFORM_CAPABILITY_ID,
    .dwWindowsVersion       = USB_WINUSB_WINDOWS_VERSION,
    .wMSOSDescriptorSetTotalLength = sizeof(usb_msos_descriptor_set_t),
    .bMS_VendorCode         = USB_WINUSB_VENDOR_CODE,
    .bAltEnumCode           = 0,
  },
};

const alignas(4) usb_msos_descriptor_set_t usb_msos_descriptor_set =
{
  .header =
  {
    .wLength             = sizeof(usb_winusb_set_header_descriptor_t),
    .wDescriptorType     = USB_WINUSB_SET_HEADER_DESCRIPTOR,
    .dwWindowsVersion    = USB_WINUSB_WINDOWS_VERSION,
    .wDescriptorSetTotalLength = sizeof(usb_msos_descriptor_set_t),
  },

  .subset =
  {
    .header = {
      .wLength           = sizeof(usb_winusb_subset_header_function_t),
      .wDescriptorType   = USB_WINUSB_SUBSET_HEADER_FUNCTION,
      .bFirstInterface   = USB_INTF_BULK,
      .bReserved         = 0,
      .wSubsetLength     = sizeof(usb_msos_descriptor_subset_t),
    },

    .comp_id =
    {
      .wLength           = sizeof(usb_winusb_feature_compatble_id_t),
      .wDescriptorType   = USB_WINUSB_FEATURE_COMPATBLE_ID,
      .CompatibleID      = "WINUSB\0\0",
      .SubCompatibleID   = { 0 },
    },

    .property =
    {
      .wLength             = sizeof(usb_winusb_feature_reg_property_guids_t),
      .wDescriptorType     = USB_WINUSB_FEATURE_REG_PROPERTY,
      .wPropertyDataType   = USB_WINUSB_PROPERTY_DATA_TYPE_MULTI_SZ,
      .wPropertyNameLength = sizeof(usb_msos_descriptor_set.subset.property.PropertyName),
      .PropertyName        = {
          'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
          'G',0,'U',0,'I',0,'D',0,'s',0, 0, 0 },
      .wPropertyDataLength = sizeof(usb_msos_descriptor_set.subset.property.PropertyData),
      .PropertyData        = {
          '{',0,'C',0,'D',0,'B',0,'3',0,'B',0,'5',0,'A',0,'D',0,'-',0,'2',0,'9',0,'3',0,'B',0,'-',0,
          '4',0,'6',0,'6',0,'3',0,'-',0,'A',0,'A',0,'3',0,'6',0,'-',0,'1',0,'A',0,'A',0,'E',0,'4',0,
          '6',0,'4',0,'6',0,'3',0,'7',0,'7',0,'6',0,'}',0, 0, 0, 0, 0 },
    },
  },
};

const alignas(4) uint8_t usb_hid_report_descriptor[28] =
{
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x00,        // Usage (Undefined)
  0xa1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xff, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x40,        //   Report Count (64)
  0x09, 0x00,        //   Usage (Undefined)
  0x81, 0x82,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x40,        //   Report Count (64)
  0x09, 0x00,        //   Usage (Undefined)
  0x91, 0x82,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
  0xc0,              // End Collection
};

const alignas(4) usb_string_descriptor_zero_t usb_string_descriptor_zero =
{
  .bLength         = sizeof(usb_string_descriptor_zero_t),
  .bDescriptorType = USB_STRING_DESCRIPTOR,
  .wLANGID         = USB_LANGID_ENGLISH,
};

const char *usb_strings[] =
{
  [USB_STR_MANUFACTURER]  = "Alex Taradov",
#ifdef HAL_CONFIG_ENABLE_VCP
  [USB_STR_PRODUCT]       = "Combined VCP and CMSIS-DAP Adapter",
#else
  [USB_STR_PRODUCT]       = "Generic CMSIS-DAP Adapter",
#endif
  [USB_STR_SERIAL_NUMBER] = usb_serial_number,
  [USB_STR_CMSIS_DAP_V1]  = "CMSIS-DAP v1 Adapter",
  [USB_STR_CMSIS_DAP_V2]  = "CMSIS-DAP v2 Adapter",
#ifdef HAL_CONFIG_ENABLE_VCP
  [USB_STR_COM_PORT]      = "Virtual COM-Port",
#endif
};

#ifdef HAL_CONFIG_ENABLE_VCP
const usb_class_handler_t usb_class_handlers[3] =
{
  usb_hid_handle_request,
  usb_cdc_handle_request,
  usb_winusb_handle_request,
};
#else
const usb_class_handler_t usb_class_handlers[2] =
{
  usb_hid_handle_request,
  usb_winusb_handle_request,
};
#endif

char usb_serial_number[16];
