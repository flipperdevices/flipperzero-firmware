#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>
#include <alt_boot.h>

// USB pins
#define BOOT_USB_PORT GPIOA
#define BOOT_USB_DM_PIN LL_GPIO_PIN_11
#define BOOT_USB_DP_PIN LL_GPIO_PIN_12
#define BOOT_USB_PIN (BOOT_USB_DM_PIN | BOOT_USB_DP_PIN)

void target_usb_wire_reset() {
    LL_GPIO_ResetOutputPin(BOOT_USB_PORT, BOOT_USB_PIN);
}

static void flipper_update_init() {
    // USB D+
    LL_GPIO_SetPinMode(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
    // USB D-
    LL_GPIO_SetPinMode(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_OUTPUT_OPENDRAIN);

    //furi_hal_spi_init();
}

void flipper_boot_dfu_exec() {
    flipper_update_init();

    // Errata 2.2.9, Flash OPTVERR flag is always set after system reset
    //__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    WRITE_REG(FLASH->SR, FLASH_SR_OPTVERR);

    target_usb_wire_reset();
    // Mark system as tainted, it will be soon
    //LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, BOOT_REQUEST_TAINTED);
    // Remap memory to system bootloader
    LL_SYSCFG_SetRemapMemory(LL_SYSCFG_REMAP_SYSTEMFLASH);
    // Jump
    flipper_boot_target_switch(0x0);
}