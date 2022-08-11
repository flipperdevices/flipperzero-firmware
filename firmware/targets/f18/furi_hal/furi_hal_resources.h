#pragma once

#include <furi.h>

#include <stm32wbxx.h>
#include <stm32wbxx_ll_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Input Related Constants */
#define INPUT_DEBOUNCE_TICKS 30

/* Input Keys */
typedef enum {
    InputKeyUp,
    InputKeyDown,
    InputKeyRight,
    InputKeyLeft,
    InputKeyOk,
    InputKeyBack,
} InputKey;

/* Light */
typedef enum {
    LightRed = (1 << 0),
    LightGreen = (1 << 1),
    LightBlue = (1 << 2),
    LightBacklight = (1 << 3),
} Light;

typedef struct {
    const GpioPin* gpio;
    const InputKey key;
    const bool inverted;
    const char* name;
} InputPin;

typedef struct {
    const GpioPin* pin;
    const char* name;
    const bool debug;
} GpioPinRecord;

extern const InputPin input_pins[];
extern const size_t input_pins_count;

extern const GpioPinRecord gpio_pins[];
extern const size_t gpio_pins_count;

extern const GpioPin vibro_gpio;
extern const GpioPin ibutton_gpio;

extern const GpioPin gpio_cc1101_g0;
extern const GpioPin gpio_rf_sw_0;

extern const GpioPin gpio_subghz_cs;
extern const GpioPin gpio_display_cs;
extern const GpioPin gpio_display_rst_n;
extern const GpioPin gpio_display_di;
extern const GpioPin gpio_sdcard_cs;
extern const GpioPin gpio_sdcard_cd;
extern const GpioPin gpio_nfc_cs;

extern const GpioPin gpio_button_up;
extern const GpioPin gpio_button_down;
extern const GpioPin gpio_button_right;
extern const GpioPin gpio_button_left;
extern const GpioPin gpio_button_ok;
extern const GpioPin gpio_button_back;

extern const GpioPin gpio_spi_d_miso;
extern const GpioPin gpio_spi_d_mosi;
extern const GpioPin gpio_spi_d_sck;
extern const GpioPin gpio_spi_r_miso;
extern const GpioPin gpio_spi_r_mosi;
extern const GpioPin gpio_spi_r_sck;

extern const GpioPin gpio_ext_pc0;
extern const GpioPin gpio_ext_pc1;
extern const GpioPin gpio_ext_pc3;
extern const GpioPin gpio_ext_pb2;
extern const GpioPin gpio_ext_pb3;
extern const GpioPin gpio_ext_pa4;
extern const GpioPin gpio_ext_pa6;
extern const GpioPin gpio_ext_pa7;

extern const GpioPin gpio_usart_tx;
extern const GpioPin gpio_usart_rx;
extern const GpioPin gpio_i2c_power_sda;
extern const GpioPin gpio_i2c_power_scl;

extern const GpioPin gpio_speaker;

extern const GpioPin periph_power;

extern const GpioPin gpio_usb_dm;
extern const GpioPin gpio_usb_dp;

void furi_hal_resources_init_early();

void furi_hal_resources_deinit_early();

void furi_hal_resources_init();

#ifdef __cplusplus
}
#endif
