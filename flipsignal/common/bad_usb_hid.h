#pragma once
// Copied from https://github.com/flipperdevices/flipperzero-firmware/blob/6de2934394cd67f24d7dca52cb49303bfc06ddb2/applications/main/bad_usb/helpers/bad_usb_hid.h

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <furi_hal.h>

typedef enum {
    BadUsbHidInterfaceUsb,
    BadUsbHidInterfaceBle,
} BadUsbHidInterface;

typedef struct {
    void* (*init)(FuriHalUsbHidConfig* hid_cfg);
    void (*deinit)(void* inst);
    void (*set_state_callback)(void* inst, HidStateCallback cb, void* context);
    bool (*is_connected)(void* inst);

    bool (*kb_press)(void* inst, uint16_t button);
    bool (*kb_release)(void* inst, uint16_t button);
    bool (*consumer_press)(void* inst, uint16_t button);
    bool (*consumer_release)(void* inst, uint16_t button);
    bool (*release_all)(void* inst);
    uint8_t (*get_led_state)(void* inst);
} BadUsbHidApi;

const BadUsbHidApi* bad_usb_hid_get_interface(BadUsbHidInterface interface);

void bad_usb_hid_ble_remove_pairing(void);

#ifdef __cplusplus
}
#endif
