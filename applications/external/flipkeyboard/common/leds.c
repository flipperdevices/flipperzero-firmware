#include "leds_i.h"

#include "../app_config.h"

// Bit-banging the WS2812b LEDs is a bit tricky. The timing is very strict.
// Hopefully, we will update to a better solution in the future.
#define DWT_CYCCNT (0xE0001004UL)
typedef struct {
    volatile uint32_t COUNT; /*!< E0001000 + Offset: 0x004 (R/W)  Cycle Count Register */
} DWT_Internal;
#define DWT_ACCESS ((DWT_Internal*)DWT_CYCCNT)

/**
 * @brief Allocates a FlipboardLeds struct.
 * @details This method allocates a FlipboardLeds struct.  This is used to
 * control the addressable LEDs on the flipboard.
 * @return The allocated FlipboardLeds struct.
*/
FlipboardLeds* flipboard_leds_alloc() {
    FlipboardLeds* leds = malloc(sizeof(FlipboardLeds));
#ifdef USE_LED_DRIVER
    FURI_LOG_D(TAG, "Using LED driver");
    leds->led_driver = led_driver_alloc(LED_COUNT, pin_ws2812_leds);
#else
    FURI_LOG_D(TAG, "Using Bit-bang LEDs");
    furi_hal_gpio_init_simple(pin_ws2812_leds, GpioModeOutputPushPull);
    leds->led_driver = NULL;
#endif
    furi_hal_gpio_init_simple(pin_status_led, GpioModeOutputPushPull);
    flipboard_leds_reset(leds);
    return leds;
}

/**
 * @brief Frees a FlipboardLeds struct.
 * @param leds The FlipboardLeds struct to free.
*/
void flipboard_leds_free(FlipboardLeds* leds) {
    for(int i = 0; i < (1 << LED_COUNT); i++) {
        flipboard_leds_set(leds, i, 0);
    }
    flipboard_leds_update(leds);
    if(leds->led_driver) {
        led_driver_free(leds->led_driver);
    }

    furi_hal_gpio_init_simple(pin_ws2812_leds, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_status_led, GpioModeAnalog);
}

/**
 * @brief Resets the LEDs to their default colors.
 * @details This method resets the LEDs data to their default color pattern.
 * You must still call flipboard_leds_update to update the LEDs.
 * @param leds The FlipboardLeds struct to reset.
*/
void flipboard_leds_reset(FlipboardLeds* leds) {
    leds->color[0] = 0x2000FF;
    leds->color[1] = 0xFF0000;
    leds->color[2] = 0x00FF00;
    leds->color[3] = 0x0000FF;
}

/**
 * @brief Sets the color of the LEDs.
 * @details This method sets the color of the LEDs.
 * @param leds The FlipboardLeds struct to set the color of.
 * @param led The LED to set the color of.
 * @param color The color to set the LED to (Hex value: RRGGBB).
*/
void flipboard_leds_set(FlipboardLeds* leds, LedIds led, uint32_t color) {
    if(led & LedId1) {
        leds->color[0] = color;
    }
    if(led & LedId2) {
        leds->color[1] = color;
    }
    if(led & LedId3) {
        leds->color[2] = color;
    }
    if(led & LedId4) {
        leds->color[3] = color;
    }
}

/**
 * @brief Updates the LEDs.
 * @details This method changes the LEDs to the colors set by flipboard_leds_set.
 * @param leds The FlipboardLeds struct to update.
*/
void flipboard_leds_update(FlipboardLeds* leds) {
#ifdef USE_LED_DRIVER
    for(int i = 0; i < LED_COUNT; i++) {
        led_driver_set_led(leds->led_driver, i, leds->color[i]);
    }
    led_driver_transmit(leds->led_driver);
    led_driver_transmit(leds->led_driver);
#else
    furi_hal_gpio_write(pin_ws2812_leds, false);
    uint8_t bits[4 * 8 * 3 * 2];
    for(int led = 0, i = 0; led < 4; led++) {
        uint32_t data = leds->color[led];
        UNUSED(data);
        uint32_t mask_red = 0x008000U;
        for(int j = 0; j < 8; j++) {
            bits[i++] = (data & mask_red) ? 38 : 14;
            bits[i++] = (data & mask_red) ? 2 : 20;
            mask_red >>= 1;
        }
        uint32_t mask_green = 0x800000U;
        for(int j = 0; j < 8; j++) {
            bits[i++] = (data & mask_green) ? 38 : 14;
            bits[i++] = (data & mask_green) ? 2 : 20;
            mask_green >>= 1;
        }
        uint32_t mask_blue = 0x000080U;
        for(int j = 0; j < 8; j++) {
            bits[i++] = (data & mask_blue) ? 38 : 14;
            bits[i++] = (data & mask_blue) ? 2 : 20;
            mask_blue >>= 1;
        }
    }
    furi_delay_us(50);

    furi_kernel_lock();
    size_t i = 0;
    while(i < COUNT_OF(bits)) {
        uint32_t delay = bits[i++];
        furi_hal_gpio_write(pin_ws2812_leds, true);
        uint32_t start = DWT_ACCESS->COUNT;
        while((DWT_ACCESS->COUNT - start) < delay) {
        }

        furi_hal_gpio_write(pin_ws2812_leds, false);
        delay = bits[i++];
        start = DWT_ACCESS->COUNT;
        while((DWT_ACCESS->COUNT - start) < delay) {
        }
    }
    furi_kernel_unlock();

    furi_delay_ms(50);
#endif
}

/**
 * @brief Sets the status LED.
 * @details This method sets the status LED to the specified state.
 * @param leds The FlipboardLeds struct to set the status LED of.
 * @param glow True to turn the status LED on, false to turn it off.
*/
void flipboard_status_led(FlipboardLeds* leds, bool glow) {
    UNUSED(leds);
    furi_hal_gpio_write(pin_status_led, glow);
}
