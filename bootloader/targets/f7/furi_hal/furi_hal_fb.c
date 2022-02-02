#include <furi_hal.h>

#include <u8g2_glue.h>

static u8g2_t fb;

void furi_hal_fb_init() {
    // Prepare gpio
    hal_gpio_init_simple(&gpio_display_rst, GpioModeOutputPushPull);
    hal_gpio_init_simple(&gpio_display_di, GpioModeOutputPushPull);
    // Initialize
    
    u8g2_Setup_st756x_flipper(&fb, U8G2_R0, u8x8_hw_spi_stm32, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(&fb);
}

void furi_hal_fb_clear() {
    u8g2_ClearBuffer(&fb);
}

void furi_hal_fb_present() {
    // Send buffer
    u8g2_SetPowerSave(&fb, 0);
    u8g2_SendBuffer(&fb);
}

u8g2_t* furi_hal_fb_get() {
    return &fb;
}