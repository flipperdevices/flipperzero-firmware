#pragma once

typedef enum {
    UsbModeNone,
    UsbModeVcpSingle,
    UsbModeVcpDual,
    UsbModeHid,
} UsbMode;

void furi_hal_usb_init(UsbMode mode);

void furi_hal_usb_disable();

void furi_hal_usb_enable();

void furi_hal_usb_on_resume();

void furi_hal_usb_on_suspend();
