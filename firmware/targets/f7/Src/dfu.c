#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>
#include <alt_boot.h>
#include <u8g2_glue.h>
#include <assets_icons.h>

// USB pins
#define BOOT_USB_PORT GPIOA
#define BOOT_USB_DM_PIN LL_GPIO_PIN_11
#define BOOT_USB_DP_PIN LL_GPIO_PIN_12
#define BOOT_USB_PIN (BOOT_USB_DM_PIN | BOOT_USB_DP_PIN)

static void flipper_boot_dfu_setup_usb() {
    // USB D+
    LL_GPIO_SetPinMode(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
    // USB D-
    LL_GPIO_SetPinMode(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
}

static void flipper_boot_dfu_usb_wire_reset() {
    LL_GPIO_ResetOutputPin(BOOT_USB_PORT, BOOT_USB_PIN);
}

static void flipper_boot_dfu_init() {
    // Screen init
    furi_hal_spi_init();
    // Prepare gpio
    furi_hal_gpio_init_simple(&gpio_display_rst, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_display_di, GpioModeOutputPushPull);

    furi_hal_compress_icon_init();
}

void flipper_boot_dfu_show_splash() {
    // Initialize
    u8g2_t* fb = malloc(sizeof(u8g2_t));
    memset(fb, 0, sizeof(u8g2_t));
    u8g2_Setup_st756x_flipper(fb, U8G2_R0, u8x8_hw_spi_stm32, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(fb);
    u8g2_SetDrawColor(fb, 0x01);
    uint8_t* splash_data = NULL;
    furi_hal_compress_icon_decode(icon_get_data(&I_DFU_128x50), &splash_data);
    u8g2_DrawXBM(fb, 0, 64 - 50, 128, 50, splash_data);
    u8g2_SetFont(fb, u8g2_font_helvB08_tr);
    u8g2_DrawStr(fb, 2, 8, "Update & Recovery Mode");
    u8g2_DrawStr(fb, 2, 21, "DFU started");
    u8g2_SetPowerSave(fb, 0);
    u8g2_SendBuffer(fb);
}

void flipper_boot_dfu_exec() {
    (void)flipper_boot_dfu_init;
    // This remains commented out until core init is fixed & stops breaking DFU.
    //flipper_boot_dfu_init();
    //flipper_boot_dfu_show_splash();

    // Errata 2.2.9, Flash OPTVERR flag is always set after system reset
    //__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    WRITE_REG(FLASH->SR, FLASH_SR_OPTVERR);

    flipper_boot_dfu_usb_wire_reset();
    flipper_boot_dfu_setup_usb();
    flipper_boot_dfu_usb_wire_reset();
    // Remap memory to system bootloader
    LL_SYSCFG_SetRemapMemory(LL_SYSCFG_REMAP_SYSTEMFLASH);
    // Jump
    furi_hal_boot_switch(0x0);
}
