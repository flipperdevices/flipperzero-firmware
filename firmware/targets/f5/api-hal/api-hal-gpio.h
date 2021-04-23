#pragma once
#include "main.h"
#include "stdbool.h"
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_system.h>
#include <stm32wbxx_ll_exti.h>

#ifdef __cplusplus
extern "C" {
#endif

// this defined in xx_hal_gpio.c, so...
#define GPIO_NUMBER (16U)

/** 
 * Interrupt callback prototype
 */
typedef void (*GpioExtiCallback) (void* , void*);

typedef enum {
    GpioModeInput,
    GpioModeOutputPushPull,
    GpioModeOutputOpenDrain,
    GpioModeAltFunctionPushPull,
    GpioModeAltFunctionOpenDrain,
    GpioModeAnalog,
    GpioModeInterruptRise,
    GpioModeInterruptFall,
    GpioModeInterruptRiseFall,
    GpioModeEventRise,
    GpioModeEventFall,
    GpioModeEventRiseFall,
} GpioMode;

typedef enum {
    GpioPullNo = LL_GPIO_PULL_NO,
    GpioPullUp = LL_GPIO_PULL_UP,
    GpioPullDown = LL_GPIO_PULL_DOWN,
} GpioPull;

typedef enum {
    GpioSpeedLow = LL_GPIO_SPEED_FREQ_LOW,
    GpioSpeedMedium = LL_GPIO_SPEED_FREQ_MEDIUM,
    GpioSpeedHigh = LL_GPIO_SPEED_FREQ_HIGH,
    GpioSpeedVeryHigh = LL_GPIO_SPEED_FREQ_VERY_HIGH,
} GpioSpeed;

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GpioPin;

// init GPIO
void hal_gpio_init(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed);

void api_hal_gpio_init(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed);

void api_hal_gpio_set_callback(const GpioPin* gpio, GpioExtiCallback cb);

// write value to GPIO, false = LOW, true = HIGH
static inline void hal_gpio_write(const GpioPin* gpio, const bool state) {
    // writing to BSSR is an atomic operation
    if(state == true) {
        gpio->port->BSRR = gpio->pin;
    } else {
        gpio->port->BSRR = (uint32_t)gpio->pin << GPIO_NUMBER;
    }
}

// read value from GPIO, false = LOW, true = HIGH
static inline bool hal_gpio_read(const GpioPin* gpio) {
    if((gpio->port->IDR & gpio->pin) != 0x00U) {
        return true;
    } else {
        return false;
    }
}

#ifdef __cplusplus
}
#endif
