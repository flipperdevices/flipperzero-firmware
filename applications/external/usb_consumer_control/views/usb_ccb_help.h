#pragma once

#include <gui/view.h>

typedef struct UsbCcbHelp UsbCcbHelp;

UsbCcbHelp* usb_ccb_help_alloc();

void usb_ccb_help_free(UsbCcbHelp* usb_ccb_help);

View* usb_ccb_help_get_view(UsbCcbHelp* usb_ccb_help);

void usb_ccb_help_set_connected_status(UsbCcbHelp* usb_ccb_help, bool connected);
