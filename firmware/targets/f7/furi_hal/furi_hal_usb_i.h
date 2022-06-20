#pragma once

#include "usb.h"

#define USB_EP0_SIZE 8

/* String descriptors */
enum UsbDevDescStr {
    UsbDevLang = 0,
    UsbDevManuf = 1,
    UsbDevProduct = 2,
    UsbDevSerial = 3,
};

typedef struct FuriHalUsbInterface FuriHalUsbInterface;

typedef struct FuriHalUsbInterface {
    void (*init)(usbd_device* dev, FuriHalUsbInterface* intf, void* ctx);
    void (*deinit)(usbd_device* dev);
    void (*wakeup)(usbd_device* dev);
    void (*suspend)(usbd_device* dev);

    struct usb_device_descriptor* dev_descr;

    void* str_manuf_descr;
    void* str_prod_descr;
    void* str_serial_descr;

    void* cfg_descr;
} FuriHalUsbInterface;
