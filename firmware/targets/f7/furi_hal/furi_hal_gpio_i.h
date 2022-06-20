#pragma once
#include <stm32wbxx_ll_gpio.h>

/**
 * Gpio structure
 */
struct GpioPin {
    GPIO_TypeDef* port;
    uint16_t pin;
};

/**
 * GPIO read pin
 * @param port GPIO port
 * @param pin pin mask
 * @return true / false
 */
void furi_hal_gpio_write_port_pin(GPIO_TypeDef* port, uint16_t pin, const bool state);

/**
 * GPIO read pin
 * @param port GPIO port
 * @param pin pin mask
 * @return true / false
 */
bool furi_hal_gpio_read_port_pin(GPIO_TypeDef* port, uint16_t pin);
