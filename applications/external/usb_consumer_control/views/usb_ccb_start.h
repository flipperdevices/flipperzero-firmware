#pragma once

#include <gui/view.h>

typedef struct UsbCcbStart UsbCcbStart;

UsbCcbStart* usb_ccb_start_alloc();

void usb_ccb_start_free(UsbCcbStart* usb_ccb_start);

View* usb_ccb_start_get_view(UsbCcbStart* usb_ccb_start);

void usb_ccb_start_set_connected_status(UsbCcbStart* usb_ccb_start, bool connected);
