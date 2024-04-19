#include <furi_hal.h>

#include "rgbleds.h"
#include "led_driver.h"

#define LED_COUNT 16

struct RgbLeds {
    uint16_t num_leds;
    uint32_t* color;
    uint16_t brightness;
    LedDriver* led_driver;
};

/**
 * @brief Allocates a RgbLeds struct.
 * @details This method allocates a RgbLeds struct.  This is used to
 * control the addressable LEDs.
 * @param num_leds The number of LEDs to allocate.
 * @param leds_pin The GPIO pin to use for the LEDs. (&gpio_ext_pc3)
 * @return The allocated RgbLeds struct.
*/
RgbLeds* rgbleds_alloc(uint16_t num_leds, const GpioPin* const leds_pin) {
    RgbLeds* leds = malloc(sizeof(RgbLeds));
    leds->num_leds = num_leds;
    leds->color = malloc(sizeof(uint32_t) * leds->num_leds);
    leds->brightness = 255;
    leds->led_driver = led_driver_alloc(leds->num_leds, leds_pin);

    rgbleds_reset(leds);
    return leds;
}

/**
 * @brief Frees a RgbLeds struct.
 * @param leds The RgbLeds struct to free.
*/
void rgbleds_free(RgbLeds* leds) {
    if(leds->led_driver) {
        led_driver_free(leds->led_driver);
    }
    free(leds->color);
    free(leds);
}

/**
 * @brief Resets the LEDs to their default color pattern (off).
 * @details This method resets the LEDs data to their default color pattern (off).
 * You must still call rgbleds_update to update the LEDs.
 * @param leds The RgbLeds struct to reset.
*/
void rgbleds_reset(RgbLeds* leds) {
    for(int i = 0; i < leds->num_leds; i++) {
        leds->color[i] = 0x000000;
    }
}

/**
 * @brief Sets the color of the LEDs.
 * @details This method sets the color of the LEDs.
 * @param leds The RgbLeds struct to set the color of.
 * @param led The LED index to set the color of.
 * @param color The color to set the LED to (Hex value: RRGGBB).
 * @return True if the LED was set, false if the LED was out of range.
*/
bool rgbleds_set(RgbLeds* leds, uint16_t led, uint32_t color) {
    if(led > leds->num_leds) {
        return false;
    }

    leds->color[led] = color;
    return true;
}

/**
 * @brief Gets the color of the LEDs.
 * @details This method gets the color of the LEDs.
 * @param leds The RgbLeds struct to get the color of.
 * @param led The LED index to get the color of.
 * @return The color of the LED (Hex value: RRGGBB).
*/
uint32_t rgbleds_get(RgbLeds* leds, uint16_t led) {
    if(led > leds->num_leds) {
        return 0;
    }

    return leds->color[led];
}

/**
 * @brief Sets the brightness of the LEDs.
 * @details This method sets the brightness of the LEDs.
 * @param leds The RgbLeds struct to set the brightness of.
 * @param brightness The brightness to set the LEDs to (0-255).
*/
void rgbleds_set_brightness(RgbLeds* leds, uint8_t brightness) {
    leds->brightness = brightness;
}

/**
 * @brief Adjusts the brightness of a color.
 * @details This method adjusts the brightness of a color.
 * @param color The color to adjust.
 * @param brightness The brightness to adjust the color to (0-255).
 * @return The adjusted color.
*/
static uint32_t adjust_color_brightness(uint32_t color, uint8_t brightness) {
    uint32_t red = (color & 0xFF0000) >> 16;
    uint32_t green = (color & 0x00FF00) >> 8;
    uint32_t blue = (color & 0x0000FF);

    red = (red * brightness) / 255;
    green = (green * brightness) / 255;
    blue = (blue * brightness) / 255;

    return (red << 16) | (green << 8) | blue;
}

/**
 * @brief Updates the LEDs.
 * @details This method changes the LEDs to the colors set by rgbleds_set.
 * @param leds The RgbLeds struct to update.
*/
void rgbleds_update(RgbLeds* leds) {
    for(int i = 0; i < leds->num_leds; i++) {
        uint32_t color = adjust_color_brightness(leds->color[i], leds->brightness);
        led_driver_set_led(leds->led_driver, i, color);
    }

    led_driver_transmit(leds->led_driver);
}