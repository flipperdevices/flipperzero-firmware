#include <furi.h>
#include "usb/usb_midi_driver.h"

int32_t usb_midi_app(void* p) {
    UNUSED(p);

    FuriHalUsbInterface* usb_config_prev;
    usb_config_prev = furi_hal_usb_get_config();
    furi_hal_usb_set_config(&midi_usb_interface, NULL);

    while(1) {
        furi_delay_ms(1000);
    }

    furi_hal_usb_set_config(usb_config_prev, NULL);

    return 0;
}