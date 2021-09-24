#include "furi-hal-usb.h"
#include "furi-hal-vcp_i.h"
#include <furi.h>
#include <usbd_cdc_if.h>
#include "usbd_def.h"
#include "usbd_core.h"
#include <usb_device.h>

#define USB_RECONNECT_DELAY 500

extern USBD_HandleTypeDef hUsbDeviceFS;

typedef struct {
    UsbMode mode_cur;
    UsbMode mode_next;
    bool enabled;
} UsbCfg;

static UsbCfg usb_config = { 0 };

static void furi_hal_usb_tmr_cb(void* context);

void furi_hal_usb_init(UsbMode new_mode) {
    static osTimerId_t reconnect_tmr = NULL;
    if (new_mode != usb_config.mode_cur) {
        if (usb_config.enabled) {
            usb_config.mode_next = new_mode;
            if (reconnect_tmr == NULL)
                reconnect_tmr = osTimerNew(furi_hal_usb_tmr_cb, osTimerOnce, NULL, NULL);
            furi_hal_usb_disable();
            osTimerStart(reconnect_tmr, USB_RECONNECT_DELAY);
        }
        else {
            if (new_mode == UsbModeVcpSingle) {
                MX_USB_Device_Init_CDC(1);
            } else if (new_mode == UsbModeVcpDual) {
                MX_USB_Device_Init_CDC(2);
            } else {

            }

            FURI_LOG_I("FuriHalUsb", "Set mode to %u", new_mode);
            usb_config.enabled = true;
            usb_config.mode_cur = new_mode;
        }
    }
}

static void furi_hal_usb_tmr_cb(void* context) {
    furi_hal_usb_init(usb_config.mode_next);
}

void furi_hal_usb_disable() {
    if (usb_config.enabled) {
        USBD_Stop(&hUsbDeviceFS);
        usb_config.enabled = false;
    }
}

void furi_hal_usb_enable() {
    if (!usb_config.enabled) {
        USBD_Start(&hUsbDeviceFS);
        usb_config.enabled = true;
    }
}

void furi_hal_usb_on_resume() {
    if ((usb_config.mode_cur == UsbModeVcpSingle) || (usb_config.mode_cur == UsbModeVcpDual))
        furi_hal_vcp_on_usb_resume();
    else if (usb_config.mode_cur == UsbModeHid)
        ;
}

void furi_hal_usb_on_suspend() {
    furi_hal_vcp_on_usb_suspend();
}
