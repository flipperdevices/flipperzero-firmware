/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32.h"
#include "usb.h"
#include "usb_cdc.h"
#include "usb_hid.h"
#include "hid_usage_desktop.h"
#include "hid_usage_button.h"

#define CDC_EP0_SIZE    0x08
#define CDC_RXD_EP      0x01
#define CDC_TXD_EP      0x81
#define CDC_DATA_SZ     0x40
#define CDC_NTF_EP      0x82
#define CDC_NTF_SZ      0x08
#define HID_RIN_EP      0x83
#define HID_RIN_SZ      0x10

#define CDC_LOOPBACK
#define ENABLE_HID_COMBO

//#define SIGNAL_MODEM  /* uncomment to signal modem capabilities */
//#define CDC_USE_IRQ   /* uncomment to build interrupt-based demo */

#if defined(SIGNAL_MODEM)
#define CDC_PROTOCOL USB_CDC_PROTO_V25TER
#else
#define CDC_PROTOCOL USB_PROTO_NONE
#endif

/* Declaration of the report descriptor */
struct cdc_config {
    struct usb_config_descriptor        config;
    struct usb_iad_descriptor           comm_iad;
    struct usb_interface_descriptor     comm;
    struct usb_cdc_header_desc          cdc_hdr;
    struct usb_cdc_call_mgmt_desc       cdc_mgmt;
    struct usb_cdc_acm_desc             cdc_acm;
    struct usb_cdc_union_desc           cdc_union;
    struct usb_endpoint_descriptor      comm_ep;
    struct usb_interface_descriptor     data;
    struct usb_endpoint_descriptor      data_eprx;
    struct usb_endpoint_descriptor      data_eptx;
#ifdef ENABLE_HID_COMBO
    struct usb_interface_descriptor     hid;
    struct usb_hid_descriptor           hid_desc;
    struct usb_endpoint_descriptor      hid_ep;
#endif //ENABLE_HID_COMBO
} __attribute__((packed));

/* HID mouse report desscriptor. 2 axis 5 buttons */
static const uint8_t hid_report_desc[] = {
    HID_USAGE_PAGE(HID_PAGE_DESKTOP),
    HID_USAGE(HID_DESKTOP_MOUSE),
    HID_COLLECTION(HID_APPLICATION_COLLECTION),
        HID_USAGE(HID_DESKTOP_POINTER),
        HID_COLLECTION(HID_PHYSICAL_COLLECTION),
            HID_USAGE(HID_DESKTOP_X),
            HID_USAGE(HID_DESKTOP_Y),
            HID_LOGICAL_MINIMUM(-127),
            HID_LOGICAL_MAXIMUM(127),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(2),
            HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE ),
            HID_USAGE_PAGE(HID_PAGE_BUTTON),
            HID_USAGE_MINIMUM(1),
            HID_USAGE_MAXIMUM(5),
            HID_LOGICAL_MINIMUM(0),
            HID_LOGICAL_MAXIMUM(1),
            HID_REPORT_SIZE(1),
            HID_REPORT_COUNT(5),
            HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE ),
            HID_REPORT_SIZE(1),
            HID_REPORT_COUNT(3),
            HID_INPUT(HID_IOF_CONSTANT),
        HID_END_COLLECTION,
    HID_END_COLLECTION,
};

/* Device descriptor */
static const struct usb_device_descriptor device_desc = {
    .bLength            = sizeof(struct usb_device_descriptor),
    .bDescriptorType    = USB_DTYPE_DEVICE,
    .bcdUSB             = VERSION_BCD(2,0,0),
    .bDeviceClass       = USB_CLASS_IAD,
    .bDeviceSubClass    = USB_SUBCLASS_IAD,
    .bDeviceProtocol    = USB_PROTO_IAD,
    .bMaxPacketSize0    = CDC_EP0_SIZE,
    .idVendor           = 0x0483,
    .idProduct          = 0x5740,
    .bcdDevice          = VERSION_BCD(1,0,0),
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = INTSERIALNO_DESCRIPTOR,
    .bNumConfigurations = 1,
};

/* Device configuration descriptor */
static const struct cdc_config config_desc = {
    .config = {
        .bLength                = sizeof(struct usb_config_descriptor),
        .bDescriptorType        = USB_DTYPE_CONFIGURATION,
        .wTotalLength           = sizeof(struct cdc_config),
#ifdef ENABLE_HID_COMBO
        .bNumInterfaces         = 3,
#else
        .bNumInterfaces         = 2,
#endif //ENABLE_HID_COMBO
        .bConfigurationValue    = 1,
        .iConfiguration         = NO_DESCRIPTOR,
        .bmAttributes           = USB_CFG_ATTR_RESERVED | USB_CFG_ATTR_SELFPOWERED,
        .bMaxPower              = USB_CFG_POWER_MA(100),
    },
    .comm_iad = {
        .bLength = sizeof(struct usb_iad_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFASEASSOC,
        .bFirstInterface        = 0,
        .bInterfaceCount        = 2,
        .bFunctionClass         = USB_CLASS_CDC,
        .bFunctionSubClass      = USB_CDC_SUBCLASS_ACM,
        .bFunctionProtocol      = CDC_PROTOCOL,
        .iFunction              = NO_DESCRIPTOR,
    },
    .comm = {
        .bLength                = sizeof(struct usb_interface_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFACE,
        .bInterfaceNumber       = 0,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 1,
        .bInterfaceClass        = USB_CLASS_CDC,
        .bInterfaceSubClass     = USB_CDC_SUBCLASS_ACM,
        .bInterfaceProtocol     = CDC_PROTOCOL,
        .iInterface             = NO_DESCRIPTOR,
    },
    .cdc_hdr = {
        .bFunctionLength        = sizeof(struct usb_cdc_header_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_HEADER,
        .bcdCDC                 = VERSION_BCD(1,1,0),
    },
    .cdc_mgmt = {
        .bFunctionLength        = sizeof(struct usb_cdc_call_mgmt_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_CALL_MANAGEMENT,
        .bmCapabilities         = 0,
        .bDataInterface         = 1,

    },
    .cdc_acm = {
        .bFunctionLength        = sizeof(struct usb_cdc_acm_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_ACM,
        .bmCapabilities         = 0,
    },
    .cdc_union = {
        .bFunctionLength        = sizeof(struct usb_cdc_union_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_UNION,
        .bMasterInterface0      = 0,
        .bSlaveInterface0       = 1,
    },
    .comm_ep = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = CDC_NTF_EP,
        .bmAttributes           = USB_EPTYPE_INTERRUPT,
        .wMaxPacketSize         = CDC_NTF_SZ,
        .bInterval              = 0xFF,
    },
    .data = {
        .bLength                = sizeof(struct usb_interface_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFACE,
        .bInterfaceNumber       = 1,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 2,
        .bInterfaceClass        = USB_CLASS_CDC_DATA,
        .bInterfaceSubClass     = USB_SUBCLASS_NONE,
        .bInterfaceProtocol     = USB_PROTO_NONE,
        .iInterface             = NO_DESCRIPTOR,
    },
    .data_eprx = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = CDC_RXD_EP,
        .bmAttributes           = USB_EPTYPE_BULK,
        .wMaxPacketSize         = CDC_DATA_SZ,
        .bInterval              = 0x01,
    },
    .data_eptx = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = CDC_TXD_EP,
        .bmAttributes           = USB_EPTYPE_BULK,
        .wMaxPacketSize         = CDC_DATA_SZ,
        .bInterval              = 0x01,
    },
#ifdef ENABLE_HID_COMBO
    .hid = {
        .bLength                = sizeof(struct usb_interface_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFACE,
        .bInterfaceNumber       = 2,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 1,
        .bInterfaceClass        = USB_CLASS_HID,
        .bInterfaceSubClass     = USB_HID_SUBCLASS_NONBOOT,
        .bInterfaceProtocol     = USB_HID_PROTO_NONBOOT,
        .iInterface             = NO_DESCRIPTOR,
    },
    .hid_desc = {
        .bLength                = sizeof(struct usb_hid_descriptor),
        .bDescriptorType        = USB_DTYPE_HID,
        .bcdHID                 = VERSION_BCD(1,0,0),
        .bCountryCode           = USB_HID_COUNTRY_NONE,
        .bNumDescriptors        = 1,
        .bDescriptorType0       = USB_DTYPE_HID_REPORT,
        .wDescriptorLength0     = sizeof(hid_report_desc),
    },
    .hid_ep = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = HID_RIN_EP,
        .bmAttributes           = USB_EPTYPE_INTERRUPT,
        .wMaxPacketSize         = HID_RIN_SZ,
        .bInterval              = 50,
    },
#endif // ENABLE_HID_COMBO
};

static const struct usb_string_descriptor lang_desc     = USB_ARRAY_DESC(USB_LANGID_ENG_US);
static const struct usb_string_descriptor manuf_desc_en = USB_STRING_DESC("Open source USB stack for STM32");
static const struct usb_string_descriptor prod_desc_en  = USB_STRING_DESC("CDC Loopback demo");
static const struct usb_string_descriptor *const dtable[] = {
    &lang_desc,
    &manuf_desc_en,
    &prod_desc_en,
};

usbd_device udev;
uint32_t	ubuf[0x20];
uint8_t     fifo[0x200];
uint32_t    fpos = 0;

static struct usb_cdc_line_coding cdc_line = {
    .dwDTERate          = 38400,
    .bCharFormat        = USB_CDC_1_STOP_BITS,
    .bParityType        = USB_CDC_NO_PARITY,
    .bDataBits          = 8,
};

static struct {
    int8_t      x;
    int8_t      y;
    uint8_t     buttons;
} __attribute__((packed)) hid_report_data;

static usbd_respond cdc_getdesc (usbd_ctlreq *req, void **address, uint16_t *length) {
    const uint8_t dtype = req->wValue >> 8;
    const uint8_t dnumber = req->wValue & 0xFF;
    const void* desc;
    uint16_t len = 0;
    switch (dtype) {
    case USB_DTYPE_DEVICE:
        desc = &device_desc;
        break;
    case USB_DTYPE_CONFIGURATION:
        desc = &config_desc;
        len = sizeof(config_desc);
        break;
    case USB_DTYPE_STRING:
        if (dnumber < 3) {
            desc = dtable[dnumber];
        } else {
            return usbd_fail;
        }
        break;
    default:
        return usbd_fail;
    }
    if (len == 0) {
        len = ((struct usb_header_descriptor*)desc)->bLength;
    }
    *address = (void*)desc;
    *length = len;
    return usbd_ack;
}


static usbd_respond cdc_control(usbd_device *dev, usbd_ctlreq *req, usbd_rqc_callback *callback) {
    if (((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) == (USB_REQ_INTERFACE | USB_REQ_CLASS)
        && req->wIndex == 0 ) {
        switch (req->bRequest) {
        case USB_CDC_SET_CONTROL_LINE_STATE:
            return usbd_ack;
        case USB_CDC_SET_LINE_CODING:
            memcpy(&cdc_line, req->data, sizeof(cdc_line));
            return usbd_ack;
        case USB_CDC_GET_LINE_CODING:
            dev->status.data_ptr = &cdc_line;
            dev->status.data_count = sizeof(cdc_line);
            return usbd_ack;
        default:
            return usbd_fail;
        }
    }
#ifdef ENABLE_HID_COMBO
    if (((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) == (USB_REQ_INTERFACE | USB_REQ_CLASS)
        && req->wIndex == 2 ) {
        switch (req->bRequest) {
        case USB_HID_SETIDLE:
            return usbd_ack;
        case USB_HID_GETREPORT:
            dev->status.data_ptr = &hid_report_data;
            dev->status.data_count = sizeof(hid_report_data);
            return usbd_ack;
        default:
            return usbd_fail;
        }
    }
    if (((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) == (USB_REQ_INTERFACE | USB_REQ_STANDARD)
        && req->wIndex == 2
        && req->bRequest == USB_STD_GET_DESCRIPTOR) {
        switch (req->wValue >> 8) {
        case USB_DTYPE_HID:
            dev->status.data_ptr = (uint8_t*)&(config_desc.hid_desc);
            dev->status.data_count = sizeof(config_desc.hid_desc);
            return usbd_ack;
        case USB_DTYPE_HID_REPORT:
            dev->status.data_ptr = (uint8_t*)hid_report_desc;
            dev->status.data_count = sizeof(hid_report_desc);
            return usbd_ack;
        default:
            return usbd_fail;
        }
    }
#endif // ENABLE_HID_COMBO
    return usbd_fail;
}


static void cdc_rxonly (usbd_device *dev, uint8_t event, uint8_t ep) {
   usbd_ep_read(dev, ep, fifo, CDC_DATA_SZ);
}

static void cdc_txonly(usbd_device *dev, uint8_t event, uint8_t ep) {
    uint8_t _t = dev->driver->frame_no();
    memset(fifo, _t, CDC_DATA_SZ);
    usbd_ep_write(dev, ep, fifo, CDC_DATA_SZ);
}

static void cdc_rxtx(usbd_device *dev, uint8_t event, uint8_t ep) {
    if (event == usbd_evt_eptx) {
        cdc_txonly(dev, event, ep);
    } else {
        cdc_rxonly(dev, event, ep);
    }
}

/* HID mouse IN endpoint callback */
static void hid_mouse_move(usbd_device *dev, uint8_t event, uint8_t ep) {
    static uint8_t t = 0;
    if (t < 0x10) {
        hid_report_data.x = 1;
        hid_report_data.y = 0;
    } else if (t < 0x20) {
        hid_report_data.x = 1;
        hid_report_data.y = 1;
    } else if (t < 0x30) {
        hid_report_data.x = 0;
        hid_report_data.y = 1;
    } else if (t < 0x40) {
        hid_report_data.x = -1;
        hid_report_data.y = 1;
    } else if (t < 0x50) {
        hid_report_data.x = -1;
        hid_report_data.y = 0;
    } else if (t < 0x60) {
        hid_report_data.x = -1;
        hid_report_data.y = -1;
    } else if (t < 0x70) {
        hid_report_data.x = 0;
        hid_report_data.y = -1;
    } else  {
        hid_report_data.x = 1;
        hid_report_data.y = -1;
    }
    t = (t + 1) & 0x7F;
    usbd_ep_write(dev, ep, &hid_report_data, sizeof(hid_report_data));
}

/* CDC loop callback. Both for the Data IN and Data OUT endpoint */
static void cdc_loopback(usbd_device *dev, uint8_t event, uint8_t ep) {
    int _t;
    if (fpos <= (sizeof(fifo) - CDC_DATA_SZ)) {
        _t = usbd_ep_read(dev, CDC_RXD_EP, &fifo[fpos], CDC_DATA_SZ);
        if (_t > 0) {
            fpos += _t;
        }
    }
    if (fpos > 0) {
        _t = usbd_ep_write(dev, CDC_TXD_EP, &fifo[0], (fpos < CDC_DATA_SZ) ? fpos : CDC_DATA_SZ);
        if (_t > 0) {
            memmove(&fifo[0], &fifo[_t], fpos - _t);
            fpos -= _t;
        }
    }
}

static usbd_respond cdc_setconf (usbd_device *dev, uint8_t cfg) {
    switch (cfg) {
    case 0:
        /* deconfiguring device */
#ifdef ENABLE_HID_COMBO
        usbd_ep_deconfig(dev, HID_RIN_EP);
        usbd_reg_endpoint(dev, HID_RIN_EP, 0);
#endif // ENABLE_HID_COMBO
        usbd_ep_deconfig(dev, CDC_NTF_EP);
        usbd_ep_deconfig(dev, CDC_TXD_EP);
        usbd_ep_deconfig(dev, CDC_RXD_EP);
        usbd_reg_endpoint(dev, CDC_RXD_EP, 0);
        usbd_reg_endpoint(dev, CDC_TXD_EP, 0);
        return usbd_ack;
    case 1:
        /* configuring device */
        usbd_ep_config(dev, CDC_RXD_EP, USB_EPTYPE_BULK /*| USB_EPTYPE_DBLBUF*/, CDC_DATA_SZ);
        usbd_ep_config(dev, CDC_TXD_EP, USB_EPTYPE_BULK /*| USB_EPTYPE_DBLBUF*/, CDC_DATA_SZ);
        usbd_ep_config(dev, CDC_NTF_EP, USB_EPTYPE_INTERRUPT, CDC_NTF_SZ);
#if defined(CDC_LOOPBACK)
        usbd_reg_endpoint(dev, CDC_RXD_EP, cdc_loopback);
        usbd_reg_endpoint(dev, CDC_TXD_EP, cdc_loopback);
#elif ((CDC_TXD_EP & 0x7F) == (CDC_RXD_EP & 0x7F))
        usbd_reg_endpoint(dev, CDC_RXD_EP, cdc_rxtx);
        usbd_reg_endpoint(dev, CDC_TXD_EP, cdc_rxtx);
#else
        usbd_reg_endpoint(dev, CDC_RXD_EP, cdc_rxonly);
        usbd_reg_endpoint(dev, CDC_TXD_EP, cdc_txonly);
#endif
#ifdef ENABLE_HID_COMBO
        usbd_ep_config(dev, HID_RIN_EP, USB_EPTYPE_INTERRUPT, HID_RIN_SZ);
        usbd_reg_endpoint(dev, HID_RIN_EP, hid_mouse_move);
        usbd_ep_write(dev, HID_RIN_EP, 0, 0);
#endif // ENABLE_HID_COMBO
        usbd_ep_write(dev, CDC_TXD_EP, 0, 0);
        return usbd_ack;
    default:
        return usbd_fail;
    }
}

static void cdc_init_usbd(void) {
    usbd_init(&udev, &usbd_hw, CDC_EP0_SIZE, ubuf, sizeof(ubuf));
    usbd_reg_config(&udev, cdc_setconf);
    usbd_reg_control(&udev, cdc_control);
    usbd_reg_descr(&udev, cdc_getdesc);
}

#if defined(CDC_USE_IRQ)
#if defined(STM32L052xx) || defined(STM32F070xB) || \
	defined(STM32F042x6)
#define USB_HANDLER     USB_IRQHandler
    #define USB_NVIC_IRQ    USB_IRQn
#elif defined(STM32L100xC) || defined(STM32G4)
    #define USB_HANDLER     USB_LP_IRQHandler
    #define USB_NVIC_IRQ    USB_LP_IRQn
#elif defined(USBD_PRIMARY_OTGHS) && \
    (defined(STM32F446xx) || defined(STM32F429xx))
    #define USB_HANDLER     OTG_HS_IRQHandler
    #define USB_NVIC_IRQ    OTG_HS_IRQn
    /* WA. With __WFI/__WFE interrupt will not be fired
     * faced with F4 series and OTGHS only
     */
    #undef  __WFI
    #define __WFI __NOP
#elif defined(STM32L476xx) || defined(STM32F429xx) || \
      defined(STM32F105xC) || defined(STM32F107xC) || \
      defined(STM32F446xx)
    #define USB_HANDLER     OTG_FS_IRQHandler
    #define USB_NVIC_IRQ    OTG_FS_IRQn
#elif defined(STM32F103x6)
    #define USB_HANDLER     USB_LP_CAN1_RX0_IRQHandler
    #define USB_NVIC_IRQ    USB_LP_CAN1_RX0_IRQn
#elif defined(STM32F103xE)
    #define USB_HANDLER     USB_LP_CAN1_RX0_IRQHandler
    #define USB_NVIC_IRQ    USB_LP_CAN1_RX0_IRQn
#else
    #error Not supported
#endif

void USB_HANDLER(void) {
    usbd_poll(&udev);
}

void main(void) {
    cdc_init_usbd();
    NVIC_EnableIRQ(USB_NVIC_IRQ);
    usbd_enable(&udev, true);
    usbd_connect(&udev, true);
    while(1) {
        __WFI();
    }
}
#else
int main(void) {
    cdc_init_usbd();
    usbd_enable(&udev, true);
    usbd_connect(&udev, true);
    while(1) {
        usbd_poll(&udev);
    }
    return 0;
}
#endif
