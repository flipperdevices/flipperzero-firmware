#include "usb_device.h"

#include "stm32wbxx.h"
#include "stm32wbxx_hal.h"

#include <stdint.h>

#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include <usbd_cdc_dual.h>
#include "usbd_cdc_if.h"

extern void Error_Handler(void);

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

extern USBD_DescriptorsTypeDef CDC_Desc;

/** Init USB device Library, add supported class and start the library */
void MX_USB_Device_Init_CDC(uint8_t if_num) {

    USBD_ClassTypeDef* class_cb = ((if_num == 1) ? &USBD_CDC_Single : &USBD_CDC_Dual);
    /* Init Device Library, add supported class and start the library. */
    if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, DEVICE_FS) != USBD_OK) {
        Error_Handler();
    }
    if (USBD_RegisterClass(&hUsbDeviceFS, class_cb) != USBD_OK) {
        Error_Handler();
    }
    if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK) {
        Error_Handler();
    }
    if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
        Error_Handler();
    }
}
