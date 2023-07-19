// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _USB_CDC_H_
#define _USB_CDC_H_

/*- Includes ----------------------------------------------------------------*/
#include "usb_std.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_CDC_BCD_VERSION    0x0110

enum
{
  USB_CDC_SEND_ENCAPSULATED_COMMAND = 0x00,
  USB_CDC_GET_ENCAPSULATED_RESPONSE = 0x01,
  USB_CDC_SET_COMM_FEATURE          = 0x02,
  USB_CDC_GET_COMM_FEATURE          = 0x03,
  USB_CDC_CLEAR_COMM_FEATURE        = 0x04,
  USB_CDC_SET_AUX_LINE_STATE        = 0x10,
  USB_CDC_SET_HOOK_STATE            = 0x11,
  USB_CDC_PULSE_SETUP               = 0x12,
  USB_CDC_SEND_PULSE                = 0x13,
  USB_CDC_SET_PULSE_TIME            = 0x14,
  USB_CDC_RING_AUX_JACK             = 0x15,
  USB_CDC_SET_LINE_CODING           = 0x20,
  USB_CDC_GET_LINE_CODING           = 0x21,
  USB_CDC_SET_CONTROL_LINE_STATE    = 0x22,
  USB_CDC_SEND_BREAK                = 0x23,
  USB_CDC_SET_RINGER_PARMS          = 0x30,
  USB_CDC_GET_RINGER_PARMS          = 0x31,
  USB_CDC_SET_OPERATION_PARMS       = 0x32,
  USB_CDC_GET_OPERATION_PARMS       = 0x33,
  USB_CDC_SET_LINE_PARMS            = 0x34,
  USB_CDC_GET_LINE_PARMS            = 0x35,
  USB_CDC_DIAL_DIGITS               = 0x36,
  USB_CDC_SET_UNIT_PARAMETER        = 0x37,
  USB_CDC_GET_UNIT_PARAMETER        = 0x38,
  USB_CDC_CLEAR_UNIT_PARAMETER      = 0x39,
  USB_CDC_GET_PROFILE               = 0x3a,

  USB_CDC_NOTIFY_RING_DETECT        = 0x09,
  USB_CDC_NOTIFY_SERIAL_STATE       = 0x20,
  USB_CDC_NOTIFY_CALL_STATE_CHANGE  = 0x28,
  USB_CDC_NOTIFY_LINE_STATE_CHANGE  = 0x29,
};

enum
{
  USB_CDC_1_STOP_BIT    = 0,
  USB_CDC_1_5_STOP_BITS = 1,
  USB_CDC_2_STOP_BITS   = 2,
};

enum
{
  USB_CDC_NO_PARITY     = 0,
  USB_CDC_ODD_PARITY    = 1,
  USB_CDC_EVEN_PARITY   = 2,
  USB_CDC_MARK_PARITY   = 3,
  USB_CDC_SPACE_PARITY  = 4,
};

enum
{
  USB_CDC_5_DATA_BITS   = 5,
  USB_CDC_6_DATA_BITS   = 6,
  USB_CDC_7_DATA_BITS   = 7,
  USB_CDC_8_DATA_BITS   = 8,
  USB_CDC_16_DATA_BITS  = 16,
};

enum
{
  USB_CDC_DEVICE_CLASS  = 2,  // USB Communication Device Class
  USB_CDC_COMM_CLASS    = 2,  // CDC Communication Class Interface
  USB_CDC_DATA_CLASS    = 10, // CDC Data Class Interface
};

enum
{
  USB_CDC_NO_SUBCLASS   = 0,
  USB_CDC_DLCM_SUBCLASS = 1, // Direct Line Control Model
  USB_CDC_ACM_SUBCLASS  = 2, // Abstract Control Model
  USB_CDC_TCM_SUBCLASS  = 3, // Telephone Control Model
  USB_CDC_MCCM_SUBCLASS = 4, // Multi-Channel Control Model
  USB_CDC_CCM_SUBCLASS  = 5, // CAPI Control Model
  USB_CDC_ETH_SUBCLASS  = 6, // Ethernet Networking Control Model
  USB_CDC_ATM_SUBCLASS  = 7, // ATM Networking Control Model
};

enum
{
  USB_CDC_HEADER_SUBTYPE    = 0, // Header Functional Descriptor
  USB_CDC_CALL_MGMT_SUBTYPE = 1, // Call Management
  USB_CDC_ACM_SUBTYPE       = 2, // Abstract Control Management
  USB_CDC_UNION_SUBTYPE     = 6, // Union Functional Descriptor
};

// USB CDC Call Management Capabilities
enum
{
  USB_CDC_CALL_MGMT_SUPPORTED = (1 << 0),
  USB_CDC_CALL_MGMT_OVER_DCI  = (1 << 1),
};

// USB CDC ACM Capabilities
enum
{
  // Device supports the request combination of Set_Comm_Feature,
  // Clear_Comm_Feature, and Get_Comm_Feature.
  USB_CDC_ACM_SUPPORT_FEATURE_REQUESTS   = (1 << 0),

  // Device supports the request combination of Set_Line_Coding, Set_Control_Line_State,
  // Get_Line_Coding, and the notification Serial_State.
  USB_CDC_ACM_SUPPORT_LINE_REQUESTS      = (1 << 1),

  // Device supports the request Send_Break.
  USB_CDC_ACM_SUPPORT_SENDBREAK_REQUESTS = (1 << 2),

  // Device supports the notification Network_Connection.
  USB_CDC_ACM_SUPPORT_NOTIFY_REQUESTS    = (1 << 3),
};

enum
{
  USB_CDC_CTRL_SIGNAL_DTE_PRESENT        = (1 << 0), // DTR
  USB_CDC_CTRL_SIGNAL_ACTIVATE_CARRIER   = (1 << 1), // RTS
};

enum
{
  USB_CDC_SERIAL_STATE_DCD     = (1 << 0),
  USB_CDC_SERIAL_STATE_DSR     = (1 << 1),
  USB_CDC_SERIAL_STATE_BREAK   = (1 << 2),
  USB_CDC_SERIAL_STATE_RING    = (1 << 3),
  USB_CDC_SERIAL_STATE_FRAMING = (1 << 4),
  USB_CDC_SERIAL_STATE_PARITY  = (1 << 5),
  USB_CDC_SERIAL_STATE_OVERRUN = (1 << 6),
};

#define USB_CDC_BREAK_DURATION_DISABLE   0
#define USB_CDC_BREAK_DURATION_INFINITE  0xffff

/*- Types -------------------------------------------------------------------*/
typedef struct USB_PACK
{
  uint8_t   bFunctionalLength;
  uint8_t   bDescriptorType;
  uint8_t   bDescriptorSubtype;
  uint16_t  bcdCDC;
} usb_cdc_header_functional_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bFunctionalLength;
  uint8_t   bDescriptorType;
  uint8_t   bDescriptorSubtype;
  uint8_t   bmCapabilities;
} usb_cdc_abstract_control_managment_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bFunctionalLength;
  uint8_t   bDescriptorType;
  uint8_t   bDescriptorSubtype;
  uint8_t   bmCapabilities;
  uint8_t   bDataInterface;
} usb_cdc_call_managment_functional_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bFunctionalLength;
  uint8_t   bDescriptorType;
  uint8_t   bDescriptorSubtype;
  uint8_t   bMasterInterface;
  uint8_t   bSlaveInterface0;
} usb_cdc_union_functional_descriptor_t;

typedef struct USB_PACK
{
  uint32_t  dwDTERate;
  uint8_t   bCharFormat;
  uint8_t   bParityType;
  uint8_t   bDataBits;
} usb_cdc_line_coding_t;

typedef struct USB_PACK
{
  usb_request_t request;
  uint16_t      value;
} usb_cdc_notify_serial_state_t;

/*- Prototypes --------------------------------------------------------------*/
void usb_cdc_init(void);
bool usb_cdc_handle_request(usb_request_t *request);
void usb_cdc_send(uint8_t *data, int size);
void usb_cdc_recv(uint8_t *data, int size);
void usb_cdc_set_state(int mask);
void usb_cdc_clear_state(int mask);
usb_cdc_line_coding_t *usb_cdc_get_line_coding(void);

void usb_cdc_send_callback(void);
void usb_cdc_recv_callback(int size);
void usb_cdc_line_coding_updated(usb_cdc_line_coding_t *line_coding);
void usb_cdc_control_line_state_update(int line_state);
void usb_cdc_send_break(int duration);

#endif // _USB_CDC_H_

