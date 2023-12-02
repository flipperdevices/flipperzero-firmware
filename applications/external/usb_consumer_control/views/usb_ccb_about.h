#pragma once

#include <gui/view.h>

typedef struct UsbCcbAbout UsbCcbAbout;

UsbCcbAbout* usb_ccb_about_alloc();

void usb_ccb_about_free(UsbCcbAbout* usb_ccb_about);

View* usb_ccb_about_get_view(UsbCcbAbout* usb_ccb_about);

void usb_ccb_about_set_connected_status(UsbCcbAbout* usb_ccb_about, bool connected);
