#include <furi.h>
#include <furi_hal.h>

#define MAX_LED_COUNT 512

typedef struct LedDriver LedDriver;

LedDriver* led_driver_alloc(int count_leds, const GpioPin* gpio);
void led_driver_free(LedDriver* led_driver);
void led_driver_set_pin(LedDriver* led_driver, const GpioPin* gpio);
uint32_t led_driver_set_led(LedDriver* led_driver, uint32_t index, uint32_t rrggbb);
uint32_t led_driver_get_led(LedDriver* led_driver, uint32_t index);
void led_driver_transmit(LedDriver* led_driver, bool transmit_if_clean);
