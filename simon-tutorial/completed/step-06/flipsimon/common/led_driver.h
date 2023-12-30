#include <furi.h>
#include <furi_hal.h>

typedef struct LedDriver LedDriver;

/**
 * @brief      Allocate and initialize LedDriver structure.
 * @details    This function allocate and initialize LedDriver structure.
 * @return     Pointer to allocated LedDriver structure.
 */
LedDriver* led_driver_alloc(int count_leds, const GpioPin* gpio);

/**
 * @brief Frees a led driver.
 * @details Frees a led driver.
 * @param led_driver The led driver to free.
 */
void led_driver_free(LedDriver* led_driver);

/**
 * @brief Sets the LED at the given index to the given color.
 * @note You must still call led_driver_transmit to actually update the LEDs.
 * @param led_driver The led driver to use.
 * @param index The index of the LED to set.
 * @param rrggbb The color to set the LED to.
 * @return The previous color of the LED.
 */
uint32_t led_driver_set_led(LedDriver* led_driver, uint32_t index, uint32_t rrggbb);

/**
 * @brief Gets the LED at the given index.
 * @param led_driver The led driver to use.
 * @param index The index of the LED to get.
 * @return The color of the LED (0xrrggbb format).
 */
uint32_t led_driver_get_led(LedDriver* led_driver, uint32_t index);

/**
 * @brief Send the LED data to the LEDs.
 * @param led_driver The led driver to use.
 */
void led_driver_transmit(LedDriver* led_driver);
