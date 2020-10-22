#include "api-hal-gpio.h"

// this defined in xx_hal_gpio.c, so...
#define GPIO_NUMBER (16U)

// init GPIO
void hal_gpio_init(GpioPin* gpio, GpioMode mode, GpioPull pull, GpioSpeed speed){
    // TODO: Alternate Functions
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = gpio->pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pull;
    GPIO_InitStruct.Speed = speed;

    HAL_GPIO_Init(gpio->port, &GPIO_InitStruct);
}

// write value to GPIO, false = LOW, true = HIGH
void hal_gpio_write(GpioPin* gpio, bool state){
    // writing to BSSR is an atomic operation
    if (state == true) {
        gpio->port->BSRR = gpio->pin;
    } else {
        gpio->port->BSRR = (uint32_t)gpio->pin << GPIO_NUMBER;
    }
}

// read value from GPIO, false = LOW, true = HIGH
bool hal_gpio_read(GpioPin* gpio){
    if ((gpio->port->IDR & gpio->pin) != 0x00U) {
        return true;
    } else {
        return false;
    }
}
